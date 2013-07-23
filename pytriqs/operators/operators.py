
################################################################################
#
# TRIQS: a Toolbox for Research in Interacting Quantum Systems
#
# Copyright (C) 2011 by M. Ferrero, O. Parcollet
#
# TRIQS is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# TRIQS. If not, see <http://www.gnu.org/licenses/>.
#
################################################################################

import string
from copy import deepcopy
"""
 State : represented by a dict : n-> lambda_n
 n = binary decomposition is the occupation number as usual
"""
        
class OpC:
    """ C or B (?) operator """
    def __init__(self,dagger,n,*args):
        self.name,self.dag = args,dagger
        self.number,self.two_num = n,2**n
        self.char = {}
        self.isFermion = True

    WhichOrder=0

    def num(self) :
        if self.dag and OpC.WhichOrder != 0 :
            return -self.number
        else :
            return self.number

    def dagger(self) :
        if self.dag : return apply(C,self.name).L[0][2][0]
        else : return apply(Cdag,self.name).L[0][2][0]
        
    def __repr__(self):
        s = reduce (lambda x,y : str(x) + ',%s'%y,self.name)
        if self.dag:
            return('C^(%s)'%s)
        else :
            return('C(%s)'%s)
        
    def __call__(self,state):
        """ State : cf header of the file"""
        res,fil = {}, self.two_num
        def signe(p):
            r=0
            for i in xrange(self.number):
                r += p & 1
                p >>= 1
            return 1 - 2* (r%2)
        if self.dag :
            for n,l in state.items() :
                if n & fil ==0 :
                    res[n+fil] = signe(n)*l
        else:
            for n,l in state.items() :
                if n & fil !=0 :
                    res[n-fil] = signe(n)*l
        return res

################################################################

class Operator:
    """ # L is a list of [ float, [liste of name of var],[list of OPbase to mult]] """
    def __init__ (self,*args):
        """Construct from xxxx"""
        if args:
            self.L=[ [ 1 , [] ,[ apply(OpC,args) ] ]  ]
        else:
            self.L=[ ]
        #self.const = 0

    def copy(self) :
        res = Operator()
        res.L = [ [c,vl[:],mono[:]] for c,vl,mono in self.L]
        return res
    
    def remove_quadratic(self):
        res = Operator()
        res.L = [ [c,vl[:],mono[:]] for c,vl,mono in self.L if len(mono) >2 ]
        return res


    def normal_order(self):
        """Returns an new operator put in normal order : Cdag first, C at the right.
           Cdag are in increasing order, C in decreasing order."""
        OpC.WhichOrder = 1
        #res = self
        self.__redu()
        OpC.WhichOrder = 0
        return self 
    
    def __redu(self) :
        """ Internal use"""
        def tri1(IN):
            oplist = IN[2]
            for i in range(1,len(oplist)):
                n,c= oplist[i].num(),1
                d = n - oplist[i-1].num()
                if (d<0) :
                    j,c = i-1,1
                    while j>=0 and n<oplist[j].num() : c *= -1; j -= 1
                    oplist.insert(j+1,oplist.pop(i))
                    return  [ [c*IN[0], IN[1][:], oplist] ]
                if (d==0) :
                    if oplist[i].dag ==  oplist[i-1].dag : return []
                    if oplist[i].dag :
                        oplist.insert(i-1,oplist.pop(i))
                        res = [ [ -IN[0], IN[1][:], oplist] ]
                        l2 =oplist[0:i-1] + oplist[i+1:]
                        res +=  [ [IN[0],IN[1][:], l2 ] ]
                        return res
        Fini =[]
        while self.L :
            Work = []
            for l in self.L :
                #print "TRI : input",l
                r = tri1(l)
                #print "TRI RETURNS",r
                if r==None : Fini.append(l)
                else :  Work+=r 
            self.L = Work
            #print "------------------------ LOOP",Work
        self.L = Fini
                   
	for n,l  in enumerate(self.L):
            for b2 in self.L[n+1:] :
		if b2[1]==l[1] and b2[2]==l[2] :
		    l[0] +=b2[0]
                    b2[0]=0
        self.L = filter( lambda x : (abs(x[0])>=1.e-14),self.L)

    def dagger(self) :
        res = Operator()
        for coef,vl,mono in self.L :
            res.L.append([coef.conjugate(),vl[:],[ o.dagger() for o in reversed(mono)]])
        res.__redu()
        return res

    def is_zero(self):
        return self.L==[] or max([abs(x[0]) for x in self.L])<1.e-13

    def __add__(self,b):
        assert type(b)==type(self)
        res = Operator()
        # we do not want to copy by reference
        for l in self.L:
            res.L.append([ l[0],list(l[1]),list(l[2]) ])
        for l in b.L:
            res.L.append([ l[0],list(l[1]),list(l[2]) ])
        res.__redu()
        return(res)

    def __sub__(self,b):
        assert type(b)==type(self)
        res = Operator()
        for l in self.L:
            res.L.append([ l[0],list(l[1]),list(l[2]) ])
        for l in b.L:
            res.L.append([ -l[0],list(l[1]),list(l[2]) ])
        res.__redu()
        return(res)
    
    def __mul__(self,b):
        assert type(b)==type(self)
        res = Operator()
        for m1 in self.L :
            for m2 in b.L :
                res.L.append( [m1[0]*m2[0],sorted(m1[1]+m2[1]), list(m1[2]+m2[2])] )
        res.__redu()
        return(res)

    def __idiv__(self,b):
        assert type(b) in [type(1), type(1.0), type(1j)]
        for l in self.L :
            l[0] /=b
        #self.__redu()
        return self
    
    def __div__(self,b):
        res = self.copy()
        res /= b
        return(res)

    def __rmul__(self,b):
        if type(b)==type(self) :
            return(b*self)
        elif type(b) in [type(1),type(1j),type(1.0) ]:
            res = Operator()
            res.L = [ [ b*l[0],list(l[1]),list(l[2]) ] for l in self.L]
            res.__redu()
            return(res)
    
    def __repr__(self):
        s=''
        for bl in self.L:
            if type(bl[0]) is not complex:
                x,signe,s1= bl[0],1,''
                if (x<0) : signe,x = -1,abs(x)
                if (abs(x)<1.e-10) : signe=0
                if (abs(x-1)>1.e-10) : s1=repr(x)+' '
                for var in bl[1]:
                    s1+=var+' '
                for op in bl[2]:
                    s1+=repr(op)
                if s1=='' : s1='1'
                if (signe==1) : s+='+ ' + s1 + ' '
                if (signe==-1) : s+='- ' + s1 + ' '
            else:
                s += '+ (' + repr(bl[0]) + ') '
                for op in bl[2]:
                    s+=repr(op)
                s += ' '

        if (s=='') : return('0')
        if (s[0]=="+") : return(s[1:])
        return(s)

    def __call__(self,state) :
        res = {}
        for coef,var,oplist in self.L :
            r = state
            # compute the action of a monomial
            for op in reversed(oplist) :
                r = op(r)
            # add r to res with coef
            for n,l in r.items() :
                if n in res : res[n] += coef*l
                else : res[n] = coef*l
        return res

    def make_coef_real_and_check(self) :
        """ Transforms the coefficients """
        def f(x) : 
            if type(x) != type(1j) : return x
	    assert abs(x.imag)<1.e-12
            return x.real
        self.L = [ (f(coef),var,oplist) for coef,var,oplist in self.L]

    def number_as_C(self) :
        """
        If the operator is alpha C , return the number of C
        """
        if len(self.L)!=1 or len(self.L[0][2])!=1 :
            raise "Internal error : len of C is not right"
        return  self.L[0][2][0].number
 
    def is_fundamental(self) :
        """
        If the operator is alpha C , return true
        """
        return len(self.L)==1 and len(self.L[0][2])==1

    def as_fundamental(self) :
        """
        If the operator is alpha C , return the C
        """
        if not self.is_fundamental():
            raise "Internal error : len of C is not right"
        return  self.L[0][2][0]

    def symmetry(self) :
        """
        If the operator is alpha C , return the symmetry of C
        """
        if len(self.L)!=1 or len(self.L[0][2])!=1 :
            raise "Internal error : len of C is not right"
        return  self.L[0][2][0].char

 
_Varlist={}
def var(x):
    global _Varlist
    _Varlist[x]=1
    return(Op(1,[x],()))


# list of all the C operators
_C_list ={}

def C_list():
    for cdag,c,n,b in _C_list.values() :
        yield n,cdag,c

def C_list_names():
    return _C_list.keys()

def number_of_C():
    return len(_C_list)

def operator_id():
    res = Operator()
    res.L.append([1,[],[] ])
    return res

def C(*args):
    """
    C operator.
    Usage : C(any argument) defines or retrieves a the operator::

      print C('down',1) # gives the C_{down,1} operator

    """
    global _C_list
    t = tuple(args)
    if t not in _C_list :
        _C_list[t] = apply(Operator,[True,len(_C_list)] + list(args)),apply(Operator,[False,len(_C_list)]+ list(args)),len(_C_list),2**len(_C_list)
    return _C_list [t][1]
        
def Cdag(*args):
    """ C^dagger operator. Usage : Cdag(any argument) defines or retrieves a the operator"""
    global _C_list
    t = tuple(args)
    if t not in _C_list :
        _C_list[t] = apply(Operator,[True,len(_C_list)]+ list(args)),apply(Operator,[False,len(_C_list)]+ list(args)),len(_C_list),2**len(_C_list)
    return _C_list [t][0]

def N(*args):
    """ Short cut for Cdag(args)*C(args)"""
    return(apply(Cdag,args)*apply(C,args))

def state(*args):
    """
Input : a list of indices for C operators. e.g. State( (1,up), (1,down) )
Returns : the corresponding indices, whose binary decomposition is given by 000100..1000
where the 1 are at the position described in input"""
    r = 0
    try : 
        for x in args :
            r +=  _C_list[tuple(x)][3]
    except :
        raise "One C operator unknown !"
    return r
    
def all_pure_states():
    """ Generates all the pure states"""
    for i in xrange(2**len(_C_list)):
        yield {i : 1}
        
def commutator(A,B) :
    C = A*B - B*A
    return C

def anti_commutator(A,B) :
    C = A*B + B*A
    return C

def extend_function_on_fundamentals(func) :
    """
    Given a function func that map any C operator to another one,
    it extends the function on the algebra of all operators.
    Returns : a function acting an operators.
    """
    def f(Op) : 
        RES = Operator()
        for l,X,M in Op.L : # term is of type (lambda, [...], [MOMEOME])
            r = [ func(c).as_fundamental() for c in M]
            RES.L.append ( [l, X, r] )
        RES._Operator__redu()
        return RES
    return f


def complete_op_list_with_fundamentals (op_dict) : 
    """
      Given a dict of operators, it  : 
       - finds all the fundamental operators contained in these operators
       - add them to the dictionnary with a unique name
       - renumbers them, without holes, with n for C, -n for the corresponding Cdagger
       - prepares a dict : name_of_the_symmetry -> X
          where X is a dict : FundamentalOperator.number_for_C -> Character (COMPLEX)
       - returns (OpDict_Complete, Nfermions,Nbosons, SymCharacters,NameOpFundamentalList)

    """      
    # first find all fundamental operators
    Opfund,NameOpFundamentalList = [],[] 
    for name, Op in op_dict.items() :
        for l,X,M in Op.L : # term is of type (lambda, [...], [MOMEOME])
            Opfund += M
    L = sorted( [ (Op.number, Op) for Op in set(Opfund) if not(Op.dag) ])
    NF = sum ( [ 1 if c.isFermion else 0 for n,c in L]) # number of fermions
    NB = sum ( [ 1 if not(c.isFermion) else 0 for n,c in L]) # number of bosons
    SymCharacters={}
    for (n,(i,OP)) in enumerate(L):
        OP.number_for_C = n+1
        OP.dagger().number_for_C =  - (n+1)
        for sym_name,char in OP.dagger().char.items() : 
            if sym_name in SymCharacters : 
                SymCharacters[sym_name][n] = char
            else :
                SymCharacters[sym_name] = {n : char}
        OP_C = C(*OP.name)
        if len([ k for (k,v) in op_dict.items() if (v-OP_C).is_zero()]) == 0: 
            k = 'C'    + string.join([ "%s"%x for x in OP.name],'')
            op_dict[k] = OP_C
            NameOpFundamentalList.append(k)
        OP_C = Cdag(*OP.name)
        if len([ k for (k,v) in op_dict.items() if (v-OP_C).is_zero()]) == 0: 
            k = 'Cdag' + string.join([ "%s"%x for x in OP.name],'')
            op_dict[k] = OP_C
            NameOpFundamentalList.append(k)

    return op_dict,NF,NB,SymCharacters,NameOpFundamentalList


def transcribe_op_list_for_C (op_dict) : 
    """
      Given a dict of operators, it  : 
       - transcribes all operators into a dict D : 
           name - >  [  [ COMPLEX, [ int] ]     ]
       - returns D

    """      
    D = {}
    for name, Op in op_dict.items() :
        opval = []
        for l,X,M in Op.L : # term is of type (lambda, [...], [MOMEOME])
            RES = [ c.number_for_C for c in M]
            RES.reverse()
            opval.append ( [l, RES] )
        D[name] = opval
    return D

