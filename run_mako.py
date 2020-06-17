from mako.template import Template
import sys

# --- Parsing the arguments of the script and options
import argparse

parser = argparse.ArgumentParser(description="""
Generate the C++ file from the mako/C++  template
""")

parser.add_argument('filename', help = "Name of the file")
parser.add_argument('--outputname', '-o',  help="Name of the xxx_desc.py file [default is same as the filename]", default = '')
args = parser.parse_args()

#------------

output = args.outputname or args.filename.replace(r'.mako','')
tpl = Template(filename=args.filename)
rendered = tpl.render().strip()
print(rendered)
with open(output,'w') as f:
    f.write(rendered)

