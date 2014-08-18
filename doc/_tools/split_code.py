import glob,re,os

def work(arg, dirname, names):

    for fil in glob.glob(dirname+ "/*.rst") : 

        fi = fil[:-4]
        lines = open(fil).readlines()
        out = open(fi + ".rst", 'w')
        def get_decal(line) : return len(line) - len(line.lstrip())
        
        reading = False
        ex_list = []
        for line in lines : 

            if re.match("\s*.. compileblock\s*::.*",line) :
                assert not reading
                decal = get_decal(line)
                reading = True
                current_ex = []
                out.write(decal*' ' + ".. triqs_example:: ./" + fi.rsplit('/',1)[-1] + "_%s"%len(ex_list) + ".cpp\n")
                continue

            if reading :
                if line.strip()=='' or get_decal(line) > decal : 
                    current_ex.append(line)
                    continue
                else :
                    reading = False
                    ex_list.append(current_ex)

            out.write(line)

        if reading : ex_list.append(current_ex)

        for n,ex in enumerate(ex_list) : 
            filename = fi  + "_%s.cpp"%n
            out = open(filename,'w')
            print filename
            for l in ex :
                out.write(l)
            #os.system("vim -c '% 

os.path.walk('./', work, None)

# then
#find . -name "*_?.cpp" | xargs mvim
#macro ggVG Ctrl K
# :bufdo execute "normal! @a" | w:bufdo execute "normal! @a" | w
