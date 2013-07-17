import tempfile
# -*- coding: utf-8 -*-
# seems to be executed at the level of the conf.py
# so we need to link the lib at that place...
"""
"""
import os
from subprocess import Popen,PIPE

from docutils import nodes
from sphinx.util.compat import Directive
from docutils.parsers.rst import directives
from sphinx.errors import SphinxError

class CompileBlockError(SphinxError):
    category = 'compileblock error'

class AutoCompile(object):
    #here = os.path.abspath(__file__)
    #pycon = os.path.join(os.path.dirname(here),'pycon.py')
    config = dict(
        #pycon = 'python ' + pycon,
        #pycon_prefix_chars = 4,
        #pycon_show_source = False,
        #console = 'bash',
        #console_prefix_chars = 1 ,
        compiler = "clang++",
        link = "",
        include = "",
        definitions = "",
        ld_library_path = "",
    )
    @classmethod
    def builder_init(cls,app):
        #cls.config.update(app.builder.config.autorun_languages)
        cls.config.update(app.builder.config.autocompile_opts)
    
class CompileBlock(Directive):
    has_content = True
    required_arguments = 0
    optional_arguments = 1
    final_argument_whitespace = False
    option_spec = {
        'linenos': directives.flag,
    }

    def run(self):
        config = AutoCompile.config

        # Get configuration values for the language
        input_encoding = 'ascii' #config.get(language+'_input_encoding','ascii')
        output_encoding = 'ascii' #config.get(language+'_output_encoding','ascii')
        show_source = True 

        # Build the code text
        code = u'\n'.join(self.content).encode(input_encoding)
       
        # write the temp file for compilation 
        fil = tempfile.NamedTemporaryFile(suffix='.cpp')
        fil.write(code)
        fil.flush()
         
        args = "%(compiler)s "%config + fil.name + " %(definitions)s %(include)s %(link)s"%config
        print "Compiling ...."
        #print args
        import subprocess as S
        error = True
        try : 
            stdout = S.check_output(args,stderr=S.STDOUT,shell=True)
            try : 
                resout = S.check_output(" ./a.out"%config ,stderr=S.STDOUT,shell=True)
                #resout = S.check_output("LD_LIBRARY_PATH=$LD_LIBRAY_PATH:%(ld_library_path)s && ./a.out"%config ,stderr=S.STDOUT,shell=True)
                if resout : 
                   stdout +='---------- Result is -------\n' + resout
                error = False
            except S.CalledProcessError as E : 
                stdout ='---------- RunTime error -------\n'
                stdout += E.output
        except S.CalledProcessError as E : 
            stdout ='---------- Compilation error -------\n'
            stdout += E.output
        print "... done"

        # Process output 
        if stdout:
            stdout = stdout.decode(output_encoding,'ignore')
            out = u''.join(stdout).decode(output_encoding)
        else:
            out = '' #.join(stderr).decode(output_encoding)
        
        # Get the original code with prefixes
        if show_source:
            code = u'\n'.join(self.content)
        else:
            code = ''
        code_out = u'\n'.join((code,out))

        if error : # report on console
            print " Error in processing "
            print code_out 

        literal = nodes.literal_block(code_out,code_out)
        literal['language'] = 'c' 
        literal['linenos'] = 'linenos' in self.options
        return [literal]

def setup(app):
    app.add_directive('compileblock', CompileBlock)
    app.connect('builder-inited',AutoCompile.builder_init)
    #app.add_config_value('autorun_languages', AutoRun.config, 'env')
    app.add_config_value('autocompile_opts', AutoCompile.config, 'env')

