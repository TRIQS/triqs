TypeError: coercing to Unicode: need string or buffer, dict found
-----------------------------------------------------------------

If this appears when the documentation is produced, just delete the
.matplotlib directory in your home and start again. The reason for
the bug is that different versions of matplotlib produce different
formats for font caches that are not compatible
