# Copyright (c) 2019-2020 Simons Foundation
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
#     https:#www.gnu.org/licenses/gpl-3.0.txt
#
# Authors: Nils Wentzell

"""
Context manager that captures ``sys.stdout`` into a list of lines.

Defines :class:`capture_stdout`, a ``list`` subclass usable in a
``with`` block. Inside the block ``sys.stdout`` is redirected to an
in-memory :class:`io.StringIO` buffer; on exit the buffered text is
split on newlines and appended to ``self``.

Only Python-level prints are captured. C++ output written to file
descriptor ``1`` is *not* captured here -- use
:mod:`triqs.utility.redirect` for that.
"""

from io import StringIO
import sys

class capture_stdout(list):
    """
    Context manager that captures ``sys.stdout`` into a list of lines.

    Subclass of :class:`list`. While the ``with`` block is active,
    ``sys.stdout`` is redirected to an internal :class:`io.StringIO`
    buffer. On exit the buffered text is split on newlines and the
    resulting list of strings is appended to ``self``, so that after
    the block iterating over the instance yields one captured line at
    a time.

    Notes
    -----
    Captures only Python-level writes to ``sys.stdout``. Output sent
    by C/C++ code directly to file descriptor ``1`` is not affected
    and requires :func:`triqs.utility.redirect.start_redirect` instead.
    """
    def __enter__(self):
        """Redirect ``sys.stdout`` to an internal buffer and return ``self``."""
        self._stdout = sys.stdout
        sys.stdout = self._stringio = StringIO()
        return self
    def __exit__(self, *args):
        """Restore ``sys.stdout`` and append the captured lines to ``self``."""
        self.extend(self._stringio.getvalue().splitlines())
        del self._stringio
        sys.stdout = self._stdout
