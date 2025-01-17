#!/usr/bin/env python3

import argparse
import errno
import os
import shutil
import subprocess
import sys
import tarfile
from pathlib import Path


# prepare repository information (the user can add/remove repositories as needed)
#
# the following keys are supported:
# - priority: lower number means less dependencies on other repos
# - url: URL of the repository for cloning
# - clone_args: arguments passed to the git clone command, e.g. "-b unstable"
# - soft_links: dictionary of repositories that should be soft linked to the deps folder
# - cmake_args: additional arguments passed to the cmake command
# - no_tests: do not run tests after building
repos = {
    "gtest": {
        "priority": 0,
        "url": "https://github.com/google/googletest",
        "clone_args": "-b main --depth 1"
    },
    "fmt": {
        "priority": 0,
        "url": "https://github.com/fmtlib/fmt",
        "clone_args": "-b 10.2.1 --depth 1"
    },
    "cpp2py": {
        "priority": 0,
        "url": "https://github.com/TRIQS/cpp2py.git",
        "clone_args": "-b unstable --depth 1"
    },
    "itertools": {
        "priority": 1,
        "url": "https://github.com/TRIQS/itertools.git",
        "soft_links": {
            "gtest": "GTest"
        }
    },
    "h5": {
        "priority": 1,
        "url": "https://github.com/TRIQS/h5.git",
        "soft_links": {
            "gtest": "GTest",
            "cpp2py": "Cpp2Py"
        }
    },
    "mpi": {
        "priority": 2,
        "url": "https://github.com/TRIQS/mpi.git",
        "soft_links": {
            "gtest": "GTest",
            "itertools": "itertools"
        }
    },
    "nda": {
        "priority": 3,
        "url": "https://github.com/TRIQS/nda.git",
        "soft_links": {
            "gtest": "GTest",
            "cpp2py": "Cpp2Py",
            "itertools": "itertools",
            "mpi": "mpi",
            "h5": "h5"
        }
    },
    "cppdlr": {
        "priority": 4,
        "url": "https://github.com/flatironinstitute/cppdlr.git",
        "soft_links": {
            "gtest": "GTest",
            "nda": "nda",
            "fmt": "fmt"
        }
    },
    "triqs": {
        "priority": 5,
        "url": "https://github.com/TRIQS/triqs.git",
        "soft_links": {
            "gtest": "GTest",
            "cpp2py": "Cpp2Py",
            "itertools": "itertools",
            "mpi": "mpi",
            "h5": "h5",
            "nda": "nda",
            "fmt": "fmt",
            "cppdlr": "cppdlr"
        }
    },
    "cthyb": {
        "priority": 6,
        "url": "https://github.com/TRIQS/cthyb.git",
        "soft_links": {
            "gtest": "GTest",
            "cpp2py": "Cpp2Py"
        }
    },
    "dft_tools": {
        "priority": 6,
        "url": "https://github.com/TRIQS/dft_tools.git",
        "soft_links": {
            "gtest": "GTest",
            "cpp2py": "Cpp2Py"
        }
    },
    "tprf": {
        "priority": 6,
        "url": "https://github.com/TRIQS/tprf.git",
        "soft_links": {
            "gtest": "GTest",
            "cpp2py": "Cpp2Py"
        }
    },
    "maxent": {
        "priority": 6,
        "url": "https://github.com/TRIQS/maxent.git"
    },
    "hubbardI": {
        "priority": 6,
        "url": "https://github.com/TRIQS/hubbardI.git",
        "soft_links": {
            "gtest": "GTest",
            "cpp2py": "Cpp2Py"
        }
    },
    "hartree_fock": {
        "priority": 6,
        "url": "https://github.com/TRIQS/hartree_fock.git"
    },
    "solid_dmft": {
        "priority": 6,
        "url": "https://github.com/TRIQS/solid_dmft.git"
    }
}


def parse_args():
    """
    Parse command-line arguments for the TRIQS (offline) installation script.

    Returns
    -------
    argparse.Namespace
        The parsed arguments as a namespace object.
    """

    parser = argparse.ArgumentParser(description="Clone, archive and install TRIQS related repositories", allow_abbrev=False)
    parser.add_argument("-d", "--dir", default="triqs_repositories", help="directory containing (the cloned) repositories (default: ./triqs_repositories)")
    parser.add_argument("-a", "--archive", action="store_true", help="archive the repository directory DIR (default: False)")
    parser.add_argument("-c", "--clone", nargs="*", help="clone all or only specific repositories into DIR")
    parser.add_argument("--clone-args", default="--depth 1", help="default arguments to pass to the git clone command (default: '--depth 1')")
    parser.add_argument("-i", "--install", nargs="*", help="build and install all or only specific repositories")
    parser.add_argument("--install-prefix", help="CMake install prefix (default: DIR/install)")
    parser.add_argument("--cmake-args", default="", help="additional arguments to pass to the cmake command (default: '')")
    parser.add_argument("--no-tests", action="store_true", help="do not run tests after building (default: False)")
    parser.add_argument("-j", "--ncores", default=1, type=int, help="number of cores to use for building and installing (default: 1)")
    args = parser.parse_args()
    return args


def clone_repos(args, repos):
    """
    Clone the specified repositories into the specified directory.

    Parameters
    ----------
    args : argparse.Namespace
        The parsed command-line arguments.
    repos : dict
        A dictionary containing the repository information.

    Raises
    ------
    subprocess.CalledProcessError
        If the git clone command fails.
    ValueError
        If a specified repository is not found in the repository information.
    """

    print('=' * 25)

    # check repos to clone
    if not args.clone:
        # choose all repos if none are specified
        print(f"No repositories specified to clone --> cloning all repositories")
        args.clone = [*repos]
    else:
        print(f"User specified repositories to clone: {args.clone}")

        # remove duplicate repos
        args.clone = list(set(args.clone))

         # loop over remaining repos
        for name in args.clone:
            # check if the repo is in the repo information
            if name not in repos:
                raise ValueError(f"Repository {name} not found in the repository information.")

            # add missing dependencies
            for dep in repos[name].get("soft_links", {}).keys():
                if dep not in args.clone:
                    print(f"Adding missing {name} dependency: {dep}")
                    args.clone.append(dep)

    # sort repos by priority
    args.clone.sort(key=lambda x: repos[x]["priority"])

    # output repos to clone
    print(f"Cloning the following repositories: {args.clone}")
    print('=' * 25)

    # loop over repos to clone
    for name in args.clone:
        # get repo information
        repo_info = repos[name]
        url = repo_info["url"]
        clone_args = repo_info.get("clone_args", args.clone_args)
        repo_dir = args.dir + "/" + name + ".src"
        repo_path = Path(repo_dir)

        # clone the repo if hasn't been cloned yet
        if not repo_path.exists():
            subprocess.run(["git", "clone"] + clone_args.split() + [url, repo_dir], check=True)
        else:
            print(f"Repository {name} already exists --> skipping")

        # create soft links for dependencies
        soft_links = repo_info.get("soft_links", {})
        deps_path = repo_path / "deps"
        for dep_name, ln_name in soft_links.items():
            dst = deps_path / ln_name
            src = Path(args.dir + "/" + dep_name + ".src")
            if not dst.is_symlink() and src.exists():
                print(f"Creating soft link {ln_name} for {name}")
                os.symlink(os.path.relpath(src, deps_path), dst)
            else:
                print(f"Soft link {ln_name} for {name} already exists --> skipping")
        print('-' * 25)


def create_archive(args):
    """
    Archives the specified directory into a tar.gz file.

    Parameters
    ----------
    args : argparse.Namespace
        The parsed command-line arguments.

    Raises
    ------
    FileNotFoundError
        If the specified directory does not exist.
    """

    print('=' * 25)

    # check if the specified directory exists
    repo_dir = Path(args.dir)
    if not repo_dir.exists():
        raise FileNotFoundError(errno.ENOENT, os.strerror(errno.ENOENT), str(repo_dir))
    print(f"Archiving the directory: {str(repo_dir.absolute())}")

    # copy the script into the repo directory
    script_path = Path(__file__).resolve()
    shutil.copy(script_path, repo_dir)

    # create the tar.gz archive
    with tarfile.open(repo_dir.name + ".tar.gz", "w:gz") as tar:
        tar.add(repo_dir)


def build_and_install(args, repos):
    """
    Build and install the specified repositories.

    Parameters
    ----------
    args : argparse.Namespace
        The parsed command-line arguments.
    repos : dict
        A dictionary containing the repository information.

    Raises
    ------
    subprocess.CalledProcessError
        If the cmake or make command fails.
    FileNotFoundError
        If the src directory does not exist.
    ValueError
        If a specified repository is not found in the repository information.
    """

    print('=' * 25)

    # check repos to install
    if not args.install:
        # if no repos are specified, install triqs and all apps (core libs are installed by triqs automatically)
        print(f"No repositories specified to install --> installing TRIQS and all apps (core libraries are installed by TRIQS automatically)")
        args.install = [x[0] for x in repos.items() if x[1]["priority"] >= 5]
    else:
        print(f"User specified repositories to install: {args.install}")

        # remove duplicate repos
        args.install = list(set(args.install))

        # loop over remaining repos
        for name in args.install:
            # check if the repo is in the repo information
            if name not in repos:
                raise ValueError(f"Repository {name} not found in the repository information.")

            # filter out dependencies that will be installed by another repo with higher priority
            for dep in repos[name].get("soft_links", {}).keys():
                if dep in args.install:
                    print(f"Repository {dep} will be installed by {name} automatically --> removing")
                    args.install = [x for x in args.install if x != dep]

    # sort repos by priority
    args.install.sort(key=lambda x: repos[x]["priority"])

    # output repositories to install
    print(f"Installing the following repositories: {args.install}")
    print('=' * 25)

    # loop over repos to install
    for name in args.install:
        print(f"Building and installing {name}")

        # get repo information
        repo_info = repos[name]

        # check if the src directory exists
        repo_path = Path(args.dir + "/" + name + ".src")
        if not repo_path.exists():
            raise FileNotFoundError(errno.ENOENT, os.strerror(errno.ENOENT), repo_path.name)

        # build and install directory
        build_dir = Path(args.dir + "/" + name + ".build")
        install_prefix = Path(args.dir + "/install" if args.install_prefix is None else args.install_prefix)
        cmake_install_prefix = "-DCMAKE_INSTALL_PREFIX=" + str(install_prefix.absolute())

        # source triqs shell variables for apps
        if repo_info["priority"] >= 6 and "TRIQS_ROOT" not in os.environ:
            py_version = str(sys.version_info.major) + "." + str(sys.version_info.minor)
            py_path = str((install_prefix / ("lib/python" + py_version + "/site-packages")).absolute())
            os.environ["TRIQS_ROOT"] = str(install_prefix.absolute())
            os.environ["PYTHONPATH"] = py_path + ":" + os.getenv("PYTHONPATH", "")
            print(f"Setting environment variable TRIQS_ROOT to {os.environ['TRIQS_ROOT']}")
            print(f"Setting environment variable PYTHONPATH to {os.environ['PYTHONPATH']}")

        # build, test and install
        cmake_args = repo_info.get("cmake_args", args.cmake_args)
        subprocess.run(["cmake", "-B", str(build_dir), str(repo_path), cmake_install_prefix] + cmake_args.split(), check=True)
        subprocess.run(["cmake", "--build", str(build_dir), "--", "-j", str(args.ncores)], check=True)
        if not repo_info.get("no_tests", args.no_tests):
            subprocess.run(["ctest", "--test-dir", str(build_dir), "-j", str(args.ncores)], check=True)
        subprocess.run(["cmake", "--install", str(build_dir)], check=True)

        print('-' * 25)


# command line arguments
args = parse_args()

# clone repositories
if args.clone is not None:
    clone_repos(args, repos)

# archive the repository directory
if args.archive:
    create_archive(args)

# build and install repositories
if args.install is not None:
    build_and_install(args, repos)
