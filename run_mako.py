# Copyright (c) 2015 Commissariat à l'énergie atomique et aux énergies alternatives (CEA)
# Copyright (c) 2015 Centre national de la recherche scientifique (CNRS)
# Copyright (c) 2020 Simons Foundation
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
# Authors: Olivier Parcollet, Nils Wentzell

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

