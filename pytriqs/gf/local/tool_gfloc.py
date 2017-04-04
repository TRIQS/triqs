from pytriqs.gf.singularities import TailGf_s, TailGf, TailGf_tv3, TailGf_tv4

def make_singularity_maker(n_order, tail, singularity):
    tail_type= [TailGf_s, None, TailGf, TailGf_tv3, TailGf_tv4] # None -> replace with vector valued ?
    return (lambda se : tail_type[len(se.target_shape)](target_shape = se.target_shape, n_order=n_order, order_min = -2)) if not (tail or singularity) else None


