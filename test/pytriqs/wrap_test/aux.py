
def ffg( *args, **kw) : 
    """ my doc of ffg in module """
    print "calling ffg, with :"
    print args
    print kw
    #return [2*x for x in args], kw
    return tuple(2*x for x in args), kw


def post1(res) : 
    return [res]


def pure_py1(self, i) : 
    """ 
      doc of pure_py1 : a nice funciton ,...
    """
    i = i/2
    print " I am in pure python method pure_py1 %s "%i
    return ["pure_py1 return list"]
