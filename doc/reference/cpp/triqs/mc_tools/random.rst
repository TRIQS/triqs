.. highlight:: c

.. _random:

Random number generator
-----------------------

Generic use
***********

TRIQS comes with a set of random number generators implemented in the class
``random_generator``. Here's an example illustrating the
use of the generator::

  #include <iostream>
  #include <triqs/mc_tools/random_generator.hpp>

  int main() {

    // Construct a random number generator
    triqs::mc_tools::random_generator RNG("mt19937", 23432);

    // Generate double numbers in [0,1[
    for (int i = 0; i < 100; i++) std::cout << RNG() << " ";
    std::cout << std::endl << std::endl;

    // Generate double numbers in [0,5[
    for (int i = 0; i < 100; i++) std::cout << RNG(5.0) << " ";
    std::cout << std::endl << std::endl;

    // Generate int numbers in [0,5[
    for (int i = 0; i < 100; i++) std::cout << RNG(5) << " ";
    std::cout << std::endl << std::endl;
  }

After having included the header
:file:`<triqs/mc_tools/random_generator.hpp>` one constructs a
random generator with::

    triqs::mc_tools::random_generator RNG("mt19937", 23432);

The first argument is a string which allows you to select the generator. Here
mt19937 stands for a version of a Mersenne Twister. The second argument is
the random seed.

Now that you have an instance of a generator ``RNG``, you can generate
either ``double`` numbers or ``int`` numbers. If ``RNG()`` is just called
with parenthesis, it produces numbers uniformly distributed on the
interval :math:`[0,1[`. If there is a ``double`` argument ``D``, then
numbers are produced uniformly on :math:`[0,D]`. Finally, if there is
a ``int`` argument ``I``, integer numbers are generated on :math:`[0,I[`.


Getting a list of random number generators
******************************************

In order to have a list of all the strings describing the available random
generators, you can call the static method ``random_generator_names``. For
example::

  #include <iostream>
  #include <triqs/mc_tools/random_generator.hpp>

  int main() {
    std::cout << "A list of all random generators" << std::endl << std::endl;
    std::cout << triqs::mc_tools::random_generator::random_generator_names() << std::endl;
  }


Accessing the random number generator from the Monte Carlo class
****************************************************************

When you construct an instance of a Monte Carlo class ``mc_generic``, this
instance automatically has an access to a random number generator.
Imagine you constructed an instance::

  triqs::mc_tools::mc_generic<double> SpinMC(n_cycles, length_cycle, n_warmup_cycles,
                                             random_name, random_seed, verbosity);

Now, you can use ``SpinMC`` to have a random number generator::

  triqs::mc_tools::random_generator RNG = SpinMC.get_rng();

Typically, this generator will be passed as a argument to your
implementation of a move::

  SpinMC.add_move(mymove(config, SpinMC.rng(), ...), "my move")

