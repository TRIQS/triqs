#!/usr/bin/env python3
# Copyright (c) 2026 Simons Foundation
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You may obtain a copy of the License at
#     https://www.gnu.org/licenses/gpl-3.0.txt
#
# Authors: Nils Wentzell

"""Detect and fix Python SyntaxWarnings across a source tree.

Primary target: invalid string escape sequences (e.g. LaTeX ``"$\\mu$"`` where
``\\m`` is not a valid Python escape). Python only *warns* today but will raise
``SyntaxError`` in a future release, so these must be cleaned up. Useful both as
a one-shot fixer and as a CI/manual gate (non-zero exit when issues remain).

Two classes of problem are handled:

  1. Invalid escapes  -> Python emits ``SyntaxWarning`` (caught via compile()).
     ``--fix`` rewrites the offending string literal so its value is unchanged
     and the warning disappears: it converts to a raw string when every
     backslash is literal, otherwise it doubles only the offending backslash.

  2. Silently-corrupted valid escapes  -> a LaTeX command that happens to be a
     valid Python escape (``\\beta``->backspace, ``\\nu``->newline, ``\\tau``->tab,
     ...). These produce NO warning and cannot be auto-fixed value-preservingly
     (the corrupt value *is* the current value), so they are *reported* for a
     human to convert to a raw string. Surfaced under ``--strict`` and always
     under ``--fix`` -- they typically sit right next to an invalid escape we do
     fix, e.g. ``:math:`G(\\omega, \\nu)``` where ``\\omega`` is doubled but the
     sibling ``\\nu`` stays a newline.

When fixing, a literal whose backslashes are all literal (LaTeX/regex) is
converted to a raw string (cleanest result); otherwise only the offending
backslash is doubled. With --prefer-raw this raw conversion is also applied to
warning-free literals, normalizing pre-existing ``"\\d+"`` / ``"\\\\frac"`` style
strings to ``r"\\d+"`` / ``r"\\frac"``.

Usage:
  fix_escape_sequences.py [PATHS...]            # detect, report, exit 1 if any
  fix_escape_sequences.py --fix [PATHS...]      # rewrite invalid escapes in place
  fix_escape_sequences.py --fix --prefer-raw .  # + normalize \\-literals to raw
  fix_escape_sequences.py --strict [PATHS...]   # also report silent suspects
  fix_escape_sequences.py --fix --diff [PATHS]  # show diffs instead of writing

With no PATHS, scans the current directory. Directories named build*, deps,
.git, and typical virtualenv/cache dirs are skipped. Symlinked trees ARE
followed (TRIQS app dirs are frequently symlinks).
"""
from __future__ import annotations
import argparse
import ast
import difflib
import io
import os
import sys
import tokenize
import warnings
from pathlib import Path

SKIP_DIR_PARTS = {".git", "__pycache__", ".pytest_cache", ".mypy_cache",
                  ".venv", "venv", "deps", "node_modules", ".ccache"}

def iter_py_files(paths):
    # os.walk(followlinks=True) so symlinked source trees (common in dev setups
    # where app dirs are symlinks) are traversed -- Path.rglob skips those.
    for raw in paths:
        p = Path(raw)
        if p.is_file() and p.suffix == ".py":
            yield p
        elif p.is_dir():
            for root, dirs, names in os.walk(p, followlinks=True):
                dirs[:] = [d for d in dirs
                           if d not in SKIP_DIR_PARTS and not d.startswith("build")]
                for name in sorted(names):
                    if name.endswith(".py"):
                        yield Path(root) / name

# Errors that mean "can't read this file as Python source" -- always skip.
_READ_ERRORS = (OSError, UnicodeDecodeError, SyntaxError, ValueError, LookupError)

def read_source(path: Path):
    """Read a file the way Python's tokenizer does: honour the declared coding,
    strip a UTF-8 BOM, and preserve the original newlines. Returns (text, encoding)."""
    data = path.read_bytes()
    enc = tokenize.detect_encoding(io.BytesIO(data).readline)[0]
    return data.decode(enc), enc  # 'utf-8-sig' strips/re-adds the BOM symmetrically

def write_source(path: Path, text: str, encoding: str):
    path.write_bytes(text.encode(encoding))

# ---------------------------------------------------------------------------
# Detection of all SyntaxWarnings via compile() -- comprehensive & future-proof
# ---------------------------------------------------------------------------
def detect_syntax_warnings(src: str, name: str):
    """Compile already-read source and return [(lineno, message)] for every
    SyntaxWarning it raises (operates in memory so the caller reads once)."""
    out = []
    with warnings.catch_warnings(record=True) as caught:
        warnings.simplefilter("always")
        try:
            compile(src, name, "exec")
        except SyntaxError:
            return []  # genuinely broken file; not our concern
        for w in caught:
            if issubclass(w.category, SyntaxWarning):
                out.append((w.lineno, str(w.message)))
    return out

# ---------------------------------------------------------------------------
# Escape analysis helpers
# ---------------------------------------------------------------------------
# Characters that may legally follow a backslash in a (non-raw) str literal.
_VALID_ESCAPE_NEXT = set("\n\\'\"abfnrtv01234567xNuU")

def split_prefix(tok_text: str):
    i = 0
    while i < len(tok_text) and tok_text[i] not in ("'", '"'):
        i += 1
    return tok_text[:i], tok_text[i:]

def _escape_spans(s: str):
    """Yield (offset, char_after_backslash) for each backslash escape in s.
    The escape consumes two chars, so backslashes are never double-counted."""
    i, n = 0, len(s)
    while i < n:
        if s[i] == "\\":
            yield i, (s[i + 1] if i + 1 < n else "")
            i += 2
        else:
            i += 1

def _invalid_escape_offsets(body: str):
    """Return offsets (into body) of backslashes that start an INVALID escape."""
    return [i for i, nxt in _escape_spans(body) if nxt not in _VALID_ESCAPE_NEXT]

def _double_invalid_escapes(body: str):
    """Double every backslash that starts an invalid escape (value-preserving);
    returns body unchanged when there are none."""
    chars = list(body)
    for o in reversed(_invalid_escape_offsets(body)):
        chars[o] = "\\\\"
    return "".join(chars)

def _quote_of(body: str):
    return body[:3] if body[:3] in ("'''", '"""') else body[:1]

def _rawify(tok_text: str, prefix: str, body: str):
    """Convert the literal to a raw string iff every backslash in it is meant
    literally -- i.e. all escapes are either ``\\\\`` or invalid (LaTeX/regex).
    Collapses ``\\\\`` -> ``\\`` so the result reads naturally. Returns the raw
    literal, or None if the value cannot be represented as a raw string."""
    q = _quote_of(body)
    inner = body[len(q):-len(q)]
    out, prev = [], 0
    for i, nxt in _escape_spans(inner):
        out.append(inner[prev:i])
        if nxt == "\\":
            out.append("\\")             # \\  -> literal backslash
        elif nxt not in _VALID_ESCAPE_NEXT:
            out.append("\\" + nxt)       # invalid escape -> kept literal
        else:
            return None  # a real escape (\n, \t, \x..): not rawifiable
        prev = i + 2
    out.append(inner[prev:])
    raw_inner = "".join(out)
    # Raw strings cannot contain the closing delimiter nor end in a backslash.
    if raw_inner.endswith("\\") or q in raw_inner:
        return None
    candidate = prefix + "r" + q + raw_inner + q
    try:
        with warnings.catch_warnings():
            warnings.simplefilter("ignore")
            if ast.literal_eval(candidate) == ast.literal_eval(tok_text):
                return candidate
    except (ValueError, SyntaxError):
        pass
    return None

def fix_string_token(tok_text: str, prefer_raw: bool = False):
    """Return a fixed literal with identical value but no invalid escapes,
    or None if nothing to fix / cannot fix safely. With prefer_raw, also
    normalize warning-free literals whose backslashes are all literal to raw."""
    prefix, body = split_prefix(tok_text)
    if "r" in prefix.lower() or "\\" not in body:
        return None  # already raw, or no backslash to worry about
    has_invalid = bool(_invalid_escape_offsets(body))
    if not has_invalid and not prefer_raw:
        return None  # no invalid escape and not normalizing -> leave alone
    # Strategy 1: convert to a raw string when all backslashes are literal
    # (cleanest result for LaTeX/regex literals); falls through otherwise.
    raw = _rawify(tok_text, prefix, body)
    if raw is not None:
        return raw
    # Strategy 2: double only the offending backslashes (value-preserving).
    if has_invalid:
        return prefix + _double_invalid_escapes(body)
    return None  # prefer_raw-only literal that can't be rawified -> leave alone

def fix_source(src: str, prefer_raw: bool = False):
    """Return (new_src, num_fixes). Rewrites invalid escapes in str literals."""
    try:
        toks = list(tokenize.generate_tokens(io.StringIO(src).readline))
    except (tokenize.TokenError, SyntaxError):
        return src, 0
    lines = src.splitlines(keepends=True)
    line_start = [0]
    for ln in lines:
        line_start.append(line_start[-1] + len(ln))
    def abs_off(row, col):
        return line_start[row - 1] + col
    FSTRING_START = getattr(tokenize, "FSTRING_START", -1)
    FSTRING_MIDDLE = getattr(tokenize, "FSTRING_MIDDLE", -1)
    FSTRING_END = getattr(tokenize, "FSTRING_END", -1)
    fstring_raw = []  # stack: is the enclosing f-string raw? (rf"...")
    edits = []  # (start, end, newtext)
    for t in toks:
        new_text = None
        if t.type == FSTRING_START:
            fstring_raw.append("r" in split_prefix(t.string)[0].lower())
        elif t.type == FSTRING_END:
            if fstring_raw:
                fstring_raw.pop()
        elif t.type == tokenize.STRING:
            new_text = fix_string_token(t.string, prefer_raw)
        elif t.type == FSTRING_MIDDLE and not (fstring_raw and fstring_raw[-1]):
            # text chunk of a non-raw f-string: only backslash-doubling applies
            # (raw f-strings keep their backslashes literal -> leave untouched)
            new_text = _double_invalid_escapes(t.string)
        if new_text is not None and new_text != t.string:
            s = abs_off(t.start[0], t.start[1])
            e = abs_off(t.end[0], t.end[1])
            edits.append((s, e, new_text))
    if not edits:
        return src, 0
    out = src
    for s, e, txt in sorted(edits, reverse=True):
        out = out[:s] + txt + out[e:]
    return out, len(edits)

# ---------------------------------------------------------------------------
# Silent-corruption suspects (report-only): LaTeX cmd using a valid escape char
# ---------------------------------------------------------------------------
# Single-letter escapes that double as LaTeX command initials (\tau, \nu, ...);
# when followed by more letters they were almost certainly meant as LaTeX.
_SILENT_ESC_LETTERS = set("abfnrtv")

def _has_silent_latex_escape(body: str):
    """True if body has a *real* escape like ``\\t``/``\\n`` immediately followed
    by a letter (e.g. ``\\tau`` -> tab+'au'). Walks actual escape spans so an
    escaped backslash (``\\\\tau`` -> literal '\\tau') is NOT a false positive."""
    return any(nxt in _SILENT_ESC_LETTERS and i + 2 < len(body) and body[i + 2].isalpha()
               for i, nxt in _escape_spans(body))

def detect_silent_suspects(src: str):
    try:
        toks = list(tokenize.generate_tokens(io.StringIO(src).readline))
    except (tokenize.TokenError, SyntaxError):
        return []
    out = []
    for t in toks:
        if t.type != tokenize.STRING:
            continue
        prefix, body = split_prefix(t.string)
        if "r" in prefix.lower():
            continue
        # Flag only when the literal clearly carries *literal* backslash commands
        # (LaTeX/regex), to keep false positives low. Strong indicators:
        #   - math markup: $...$, \math.., \frac, or an RST :math: role
        #   - a co-occurring INVALID escape (e.g. \omega): proof in itself that the
        #     author writes literal backslashes, so a sibling \nu/\tau is one too.
        if not any(m in body for m in ("$", "\\math", "\\frac", ":math:")) \
                and not _invalid_escape_offsets(body):
            continue
        if _has_silent_latex_escape(body):
            out.append((t.start[0], body[:60]))
    return out

# ---------------------------------------------------------------------------
def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("paths", nargs="*", default=["."])
    ap.add_argument("--fix", action="store_true", help="rewrite invalid escapes in place")
    ap.add_argument("--diff", action="store_true", help="with --fix, print diff, don't write")
    ap.add_argument("--prefer-raw", action="store_true", dest="prefer_raw",
                    help="with --fix, also convert warning-free literals whose "
                         "backslashes are all literal (LaTeX/regex) to raw strings")
    ap.add_argument("--strict", action="store_true",
                    help="also report silent-corruption suspects (valid-escape LaTeX)")
    args = ap.parse_args(argv)
    if args.prefer_raw and not args.fix:
        ap.error("--prefer-raw requires --fix")

    files = list(iter_py_files(args.paths))
    total_warn = total_fixed = total_suspect = 0

    for f in files:
        try:
            src, enc = read_source(f)  # one read per file; everything else is in memory
        except _READ_ERRORS:
            continue  # unreadable/odd encoding -> nothing we can do, skip
        warns = detect_syntax_warnings(src, str(f))
        # A valid-escape LaTeX command (\nu->newline) sitting next to escapes we
        # do fix is the one thing --fix cannot repair value-preservingly, so
        # surface these suspects under --fix as well as --strict. Detected on the
        # original source, where the co-occurring invalid escape is still present.
        suspects = detect_silent_suspects(src) if (args.strict or args.fix) else []
        if args.fix and (warns or args.prefer_raw):
            new_src, nfix = fix_source(src, prefer_raw=args.prefer_raw)
            if nfix:
                total_fixed += nfix
                if args.diff:
                    sys.stdout.writelines(difflib.unified_diff(
                        src.splitlines(keepends=True),
                        new_src.splitlines(keepends=True),
                        fromfile=str(f), tofile=str(f) + " (fixed)"))
                else:
                    write_source(f, new_src, enc)
                    # re-detect (in memory) to report anything the fixer could not handle
                    warns = detect_syntax_warnings(new_src, str(f))
            for lineno, msg in warns:
                if "escape sequence" not in msg:
                    print(f"{f}:{lineno}: NOT auto-fixed: {msg}")
        else:
            for lineno, msg in warns:
                print(f"{f}:{lineno}: {msg}")
                total_warn += 1
        for lineno, body in suspects:
            print(f"{f}:{lineno}: SUSPECT silent-escape (LaTeX?) -- "
                  f"review, convert to a raw string by hand: {body!r}")
            total_suspect += 1

    if args.fix:
        what = "literal(s)" if args.prefer_raw else "invalid escape(s)"
        msg = f"\nRewrote {total_fixed} {what} in {len(files)} scanned file(s)."
        if total_suspect:
            msg += (f" {total_suspect} silent-escape suspect(s) need manual review "
                    "(cannot be repaired without changing the value).")
        print(msg)
        return 1 if total_suspect else 0
    if total_warn or total_suspect:
        parts = [f"{total_warn} SyntaxWarning(s)"]
        if args.strict:
            parts.append(f"{total_suspect} silent-escape suspect(s)")
        print(f"\n{', '.join(parts)} across {len(files)} file(s). "
              "Run with --fix to repair escapes.")
        return 1
    print(f"OK: no SyntaxWarnings in {len(files)} file(s).")
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
