import unittest
from triqs.atom_diag import AtomDiag
from triqs.operators import n

# possible constructors are
# many_body_operator h, fundamental_operator_set fops
# many_body_operator h, fundamental_operator_set fops, int n_min, int n_max
# many_body_operator h, many_body_operator hyb, fundamental_operator_set fops
# many_body_operator h, fundamental_operator_set fops, std::vector<triqs::atom_diag::atom_diag<false>::many_body_op_t> qn_vector

# list of tuple(args, kwargs) for constructor
def get_param_list(h, fops, qn_vector, hyb):
    return [( [h, fops], {} ),
            ( [h, fops, qn_vector], {} ),
            ( [h], {'fops':fops, 'qn_vector':qn_vector} ),
            ( [], {'h':h, 'fops':fops, 'qn_vector':qn_vector} ),
            ( [h, fops], {'n_min':1, 'n_max':1} ),
            ( [h, hyb, fops], {} ),
    ]

class test_atom_diag_constructors(unittest.TestCase):

    def setUp(self):
        self.h_real = n('up',0) * n('dn',0)
        self.h_real_valued = (1 + 0*1j) * n('up',0) * n('dn',0)
        self.h_complex = (1 + 1j) * n('up',0) * n('dn',0)
        self.hyb_real = n('up',0) + n('dn',0)
        self.hyb_complex = (1 + 1j) * n('up',0) + n('dn',0)
        self.fops = [('up',0), ('dn',0)]
        self.qn_vector = [n('up',0), n('dn',0)]

    def test_real(self):
        for args, kwargs in get_param_list(self.h_real, self.fops, self.qn_vector, self.hyb_real):
            with self.subTest(p1=args, p2=kwargs):
                try:
                    AtomDiag(*args, **kwargs)
                except Exception as e:
                    self.fail(f"args {args} and kwargs {kwargs} raised {e} !")

    def test_real_valued(self):
        for args, kwargs in get_param_list(self.h_real_valued, self.fops, self.qn_vector, self.hyb_real):
            with self.subTest(p1=args, p2=kwargs):
                try:
                    AtomDiag(*args, **kwargs)
                except Exception as e:
                    self.fail(f"args {args} and kwargs {kwargs} raised {e} !")

    def test_complex(self):
        for args, kwargs in get_param_list(self.h_complex, self.fops, self.qn_vector, self.hyb_complex):
            with self.subTest(p1=args, p2=kwargs):
                try:
                    AtomDiag(*args, **kwargs)
                except Exception as e:
                    self.fail(f"args {args} and kwargs {kwargs} raised {e} !")

    def test_h_real_hyb_complex(self):
        for args, kwargs in get_param_list(self.h_real, self.fops, self.qn_vector, self.hyb_complex):
            with self.subTest(p1=args, p2=kwargs):
                try:
                    AtomDiag(*args, **kwargs)
                except Exception as e:
                    self.fail(f"args {args} and kwargs {kwargs} raised {e} !")

    def test_h_complex_hyb_real(self):
        for args, kwargs in get_param_list(self.h_complex, self.fops, self.qn_vector, self.hyb_real):
            with self.subTest(p1=args, p2=kwargs):
                try:
                    AtomDiag(*args, **kwargs)
                except Exception as e:
                    self.fail(f"args {args} and kwargs {kwargs} raised {e} !")

if __name__ == '__main__':
    unittest.main()
