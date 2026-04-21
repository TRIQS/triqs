import warnings
warnings.warn(
    "triqs.random_generator has been moved to triqs.mc_tools. "
    "Please update your imports.",
    FutureWarning, stacklevel=2
)
from triqs.mc_tools import *
from triqs.mc_tools import __all__
