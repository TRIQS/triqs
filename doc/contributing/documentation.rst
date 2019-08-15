.. highlight:: bash

How to contribute to documentation?
===================================

Who should write documentation?
-------------------------------

We gladly accept contributions of users, especially when it comes to providing simple examples of functions or
write small parts of tutorials. Indeed, if you are learning to work with Green's functions, it is very likely that you are writing some
lines of codes to play with this container that could be very useful to future users.

If you are willing to contribute to this project, we however ask you to take the following steps:

- Check the :ref:`Documentation <documentation>` and :ref:`User guide <userguide>` sections of the website
- Check our `Tutorials on github <https://github.com/TRIQS/tutorials>`_
- Respect our :ref:`documentation conventions <conventions>`

As for code contribution, you are going to submit your changes as a pull request to the unstable branch 
of the `triqs github <https://github.com/TRIQS/triqs>`_. If you are unsure on how to do it, you can follow instructions
:ref:`here <pull_request>`.

If you have any question, feel free to use our `Google group <https://triqs.github.io/announcements>`_.


Building the documentation
--------------------------

You first need to install sphinx::

        pip install sphinx

When building the TRIQS project from sources, you should add the following flags to the cmake command::

        -DBuild_Documentation=ON -DSphinx_Only=ON

Then move to the doc folder and build it::

        cd doc && make

Because of the -DSphinx_Only=ON flag, this only generates html pages from rst files. If you also want to generate rst files from C++ examples, run::

        make docs_example_output

and if you want the rst from headers, run::

        make docs_cpp2rst


Editing the documentation
-------------------------

You can now modify rst files in::

        triqs.src/doc

and see the output in::

        triqs.build/doc/html

In order not to rebuild the documentation by hand every time you make some changes, you can install sphinx-autobuild::

         pip install sphinx-autobuild

and run::

         sphinx-autobuild triqs.src/doc triqs.build/doc/html -c triqs.build/doc

..
     Do we also want to move some rst examples from :ref:`documentation conventions <conventions>` here?
