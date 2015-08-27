=====================
Autorun Documentation
=====================

:author: Vadim Gubergrits <vadim.gubergrits@gmail.com>

Autorun is an extension for Sphinx that can execute the code from a
runblock directive and attach the output of the execution to the document. 

For example:

.. code-block:: rst

    .. runblock:: pycon
        
        >>> for i in range(5):
        ...    print i
        ...

Produces

.. runblock:: pycon
        
    >>> for i in range(5):
    ...    print i
    ...


Another example

.. code-block:: rst

    .. runblock:: console

        $ date

Produces

.. runblock:: console

    $ date 

Currently autorun supports ``pycon`` and ``console`` languages. It's also
possible to configure autorun (from :file:`conf.py`) to run other languages.


Installation
-----------------

Installing from sources

.. code-block:: console

    $ hg clone http://bitbucket.org/birkenfeld/sphinx-contrib/
    $ cd sphinx-contrib/autorun
    $ python setup.py install


Configuration
-----------------

To enable the autorun extension add 'sphinxcontrib.autorun' to the
``extensions`` list in :file:`conf.py`.

.. literalinclude:: conf.py
    :lines: 25-28

By default autorun supports ``pycon`` and ``console``.  It's possible to
configure autorun to run other languages. First you need to be able to pipe your
source to an executable. In many cases it's already done but you can build
your own program to do that.


``autorun_languages``:

    This is a dictionary in :file:`conf.py` that maps a language to an
    executable. For example:
            
    .. code-block:: python
        
        autorun_languages = {}
        autorun_languages['pycon']='python -'
    
    In order to pipe python code to python executable we must use the ``-``
    argument.
    
    It's also possible to specify the number of characters to remove from each
    line before sending the code. To do that map ``${language}_prefix_chars``
    to the number of characters to remove.

    .. code-block:: python

        autorun_languages = {}
        autorun_languages['pycon'] = 'python -'
        autorun_languages['pycon_prefix_chars'] = 4


Example of configuring autorun to run gnuplot scripts.

.. literalinclude:: conf.py
    :lines: 29-30



.. code-block:: rst

    .. runblock:: gnuplot
        
        set term png
        set out 'log.png'
        plot log(x)


This will not produce any output on stdout but it will write the
:download:`log.png` file that can be included with a standard image directive:

.. code-block:: rst

    .. image:: log.png

