import unittest
from triqs.atom_diag import AtomDiag
from triqs.operators import n

# possible constructors are
# many_body_operator h, fundamental_operator_set fops
# many_body_operator h, fundamental_operator_set fops, int n_min, int n_max
# many_body_operator h, many_body_operator hyb, fundamental_operator_set fops
# many_body_operator h, fundamental_operator_set fops, std::vector<triqs::atom_diag::atom_diag<false>::many_body_op_t> qn_vector

# list of tuple(args, kwargs) for constructor
def get_param_list(h, fops, qn_vector, h_hyb):
    return [( [h, fops], {} ),
            ( [h, fops, qn_vector], {} ),
            ( [h], {'fops':fops, 'qn_vector':qn_vector} ),
            ( [], {'h':h, 'fops':fops, 'qn_vector':qn_vector} ),
            ( [h, fops], {'n_min':1, 'n_max':1} ),
            ( [h, h_hyb, fops], {} ),
    ]

class test_atom_diag_constructors(unittest.TestCase):

    def setUp(self):
        self.h_real = (1 + 0*1j) * n('up',0) * n('dn',0)
        self.h_complex = (1 + 1j) * n('up',0) * n('dn',0)
        self.h_hyb_real = n('up',0) + n('dn',0)
        self.h_hyb_complex = (1 + 1j) * n('up',0) + n('dn',0)
        self.fops = [('up',0), ('dn',0)]
        self.qn_vector = [n('up',0), n('dn',0)]

    def test_real(self):
        for args, kwargs in get_param_list(self.h_real, self.fops, self.qn_vector, self.h_hyb_real):
            with self.subTest(p1=args, p2=kwargs):
                try:
                    AtomDiag(*args, **kwargs)
                except Exception as e:
                    self.fail(f"args {args} and kwargs {kwargs} raised {e} !")

    def test_complex(self):
        for args, kwargs in get_param_list(self.h_complex, self.fops, self.qn_vector, self.h_hyb_complex):
            with self.subTest(p1=args, p2=kwargs):
                try:
                    AtomDiag(*args, **kwargs)
                except Exception as e:
                    self.fail(f"args {args} and kwargs {kwargs} raised {e} !")
    
    # Probably doesn't have to be tested
    def test_h_real_hyb_complex(self):
        with self.assertRaises(Exception) as context:
            AtomDiag(self.h_real, self.h_hyb_complex, self.fops)
        self.assertTrue('the number is not real, it is complex' in str(context.exception))

    def test_h_complex_hyb_real(self):
        try:
            AtomDiag(self.h_complex, self.h_hyb_real, self.fops)
        except Exception as e:
            self.fail(f"h_complex and hybr_real raised {e} !")

if __name__ == '__main__':
    unittest.main()