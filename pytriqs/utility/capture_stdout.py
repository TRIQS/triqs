from __future__ import unicode_literals
from future import standard_library
standard_library.install_aliases()
from io import StringIO
import sys

# This class allows us to capture the stdout into a list of strings
class capture_stdout(list):
    def __enter__(self):
        self._stdout = sys.stdout
        sys.stdout = self._stringio = StringIO()
        return self
    def __exit__(self, *args):
        self.extend(self._stringio.getvalue().splitlines())
        del self._stringio
        sys.stdout = self._stdout
