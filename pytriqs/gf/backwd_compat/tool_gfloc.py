from pytriqs.gf.singularities import TailGf_s, TailGf, TailGfTv3, TailGfTv4

def make_singularity_maker(tail, singularity):
    tail_type= [TailGf_s, None, TailGf, TailGfTv3, TailGfTv4] # None -> replace with vector valued ?
    def f(se):
      if not (tail or singularity):
        t = tail_type[len(se.target_shape)](target_shape = se.target_shape)
      else:
        t = None
      return t
    return f

