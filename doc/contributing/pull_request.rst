.. _pull_request:

How to make a pull request?
===========================

You will need to submit your contributions in the form of pull requests to the unstable branch of the TRIQS repository,
and this page is here to guide you through this process.


Setting up your fork repository
-------------------------------

First, go to the `triqs github <https://github.com/TRIQS/triqs>`_, and click the `Fork` button at the top right of the screen.
This creates your own copy of the TRIQS repository. If you go to your account page, you see that you now have a new repository, forked from TRIQS/triqs.

Go to your local TRIQS source directory::

     cd triqs.src
and run::

     git remote -v
You should be seeing the following::

     origin	https://github.com/TRIQS/triqs.git (fetch)
     origin	https://github.com/TRIQS/triqs.git (push)
Now, add your version of TRIQS::

     git remote add my_triqs https://github.com/YOUR_USERNAME/triqs.git
Running::

     git remote -v
again returns::

     my_triqs	https://github.com/YOUR_USERNAME/triqs.git (fetch)
     my_triqs	https://github.com/YOUR_USERNAME/triqs.git (push)
     origin	https://github.com/TRIQS/triqs.git (fetch)
     origin	https://github.com/TRIQS/triqs.git (push)

Finally, run::

     git remote update
     git fetch --all

If you now type the following command::

     git branch --all
you see all the branches, both from the original repository and your own fork.

Congratulations, you're all set!
You can learn more about forks `here <https://help.github.com/en/articles/about-forks>`_ and `here <https://help.github.com/en/articles/fork-a-repo>`_.
`This page <https://help.github.com/en/articles/syncing-a-fork>`_ explains how to keep your fork synced to the original repository.


Making your pull request
------------------------

Start by moving to the unstable branch of the TRIQS repository and get the latest commits::

     git checkout unstable
     git pull
From here, create the branch where your pull request will live::

     git branch my_first_PR
     git checkout my_first_PR

You can now work on this branch as usual, modifying files and committing your changes. Keep in mind to push your branch to your fork::

     git push my_triqs my_first_PR
     
Please keep in your commit messages clear and
concise, using brackets to specify what you are modifying (for instance [doc] if you contribute to the documentation, [det_manip] if you fix a bug
in the det_manip class, etc.). Do not multiply your commits, or squash them before doing your pull request
(see for example `this page <https://davidwalsh.name/squash-commits-git>`_).

When you consider your pull request ready to be reviewed by TRIQS developers, go to the github page of your fork
and click `New pull request`. Be sure to select unstable as the target branch and my_first_PR as the source branch.
If you experience any issue, you can refer to the `github help <https://help.github.com/en/articles/creating-a-pull-request>`_.

Thank you for contributing to TRIQS!
