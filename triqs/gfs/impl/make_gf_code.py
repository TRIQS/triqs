from mako.template import Template
fi = open("gf.mako.hpp",'r').read()
tpl = Template(fi.replace('GF','${GF}'))

for GF, GRVC in [('gf','regular'), ('gf_view','view'), ('gf_const_view','const_view')]:
    with open(GF + ".hpp",'w') as f:
        f.write(tpl.render(GF= GF, GRVC =GRVC).strip())

