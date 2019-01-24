import os

def prepare_example(filename, decal):
    """From the filename, prepare the doc1, doc2, before and after the code
       and compute the lineno of the code for inclusion"""
    filename += ".cpp"
    if not os.path.exists(filename) :
        #print "example file %s (in %s) does not exist"%(filename,os.getcwd())
        return None, None, None, 0, 0
    ls = open(filename).read().strip().split('\n')
    r = [i for i, l in enumerate(ls) if not (re.match(r"^\s*/?\*",l) or re.match("^\s*//",l))]
    s, e = r[0],r[-1]+1
    assert r == range(s,e)
    def cls(w) :
        w = re.sub(r"^\s*/?\*\s?/?",'',w)
        w = re.sub(r"^\s*//\s?",'',w)
        return w
    doc1 = '\n'.join(cls(x) for x in ls[0:s])
    code = '\n'.join(decal*' ' + x.strip() for x in ls[s:e])
    doc2 = '\n'.join(cls(x) for x in ls[e:])
    return code, doc1, doc2, s, e

