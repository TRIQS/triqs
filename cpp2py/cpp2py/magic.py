"""
=====================
Cpp2py magic
=====================

{CPP2PY_DOC}

"""
import os
from IPython.core.error import UsageError
from IPython.core.magic import Magics, magics_class, line_magic, cell_magic
from IPython.core import display, magic_arguments
from IPython.utils import py3compat
from IPython.utils.io import capture_output
from IPython.paths import get_ipython_cache_dir

__version__ = '0.3.0'

from compiler import compile, print_out
from IPython.core.display import display_javascript

js = """
require(['notebook/js/codecell'], function(codecell) {
  codecell.CodeCell.options_default.highlight_modes['text/x-c++src'] = {'reg':[/^%%cpp2py/]};
  Jupyter.notebook.events.one('kernel_ready.Kernel', function(){
      Jupyter.notebook.get_cells().map(function(cell){
          if (cell.cell_type == 'code'){ cell.auto_highlight(); } }) ;
  });
});
"""

@magics_class
class Cpp2pyMagics(Magics):

    def __init__(self, shell):
        super(Cpp2pyMagics, self).__init__(shell=shell)
        self._reloads = {}
        self._code_cache = {}
        self._lib_dir = os.path.join(get_ipython_cache_dir(), 'cpp2py')
        if not os.path.exists(self._lib_dir):
            os.makedirs(self._lib_dir)
        display_javascript(js, raw=True)

    @magic_arguments.magic_arguments()
    @magic_arguments.argument( "-v", "--verbosity", type=int, help="increase output verbosity")
    @magic_arguments.argument( '-o', "--only", action='append', default=[], help="""Which object to wrap""")
    @magic_arguments.argument( '-C', "--converters", action='append', default=[], help="""Modules""")
    @magic_arguments.argument( '--cxxflags', action='append', default = [], help="""Additional compiler flags""")
    @magic_arguments.argument( "--no_clean", action='store_true', default=[], help="""""")
    @cell_magic
    def cpp2py(self, line, cell=None):
        """Compile and import everything from a Cpp2py code cell.

        Takes the c++ code, call c++2py on it and compile the whole thing
        into a module which is then loaded and 
        all of its symbols are injected into the user's namespace.

        Usage
        =====
        Prepend ``%%cpp2py`` to your cpp2py code in a cell::

        ``%%cpp2py

        ! put your code here.
        ``
        """

        try:
            # custom saved arguments
            saved_defaults = vars(
                magic_arguments.parse_argstring(self.cpp2py,
                                                self.shell.db['cpp2py']))
            self.cpp2py.parser.set_defaults(**saved_defaults)
        except KeyError:
            saved_defaults = {'verbosity': 0}

        if '-v' in line:
            self.cpp2py.parser.set_defaults(verbosity=0)

        args = magic_arguments.parse_argstring(self.cpp2py, line)
        code = cell if cell.endswith('\n') else cell + '\n'
        module = compile(code, verbosity = args.verbosity, only = args.only, cxxflags = ''.join(args.cxxflags), modules = ''.join(args.converters), no_clean = args.no_clean)

        # import all object and function in the main namespace
        imported = []
        for k, v in module.__dict__.items():
            if not k.startswith('_'):
                self.shell.push({k: v})
                imported.append(k)
        if args.verbosity > 0 and imported:
            print_out("Success", "The following objects are ready to use: %s" % ", ".join(imported))

__doc__ = __doc__.format(CPP2PY_DOC=' ' * 8 + Cpp2pyMagics.cpp2py.__doc__)

def load_ipython_extension(ip):
    """Load the extension in IPython."""
    ip.register_magics(Cpp2pyMagics)

