#!/usr/bin/env python3

import argparse
import errno
import os
import shutil
import subprocess
import sys
import tarfile
from pathlib import Path


# prepare repository information for the TRIQS core libraries (including triqs itself) and the applications based on
# triqs (the user can add/remove repositories as needed)
#
# the following keys are supported:
# - url: URL of the repository for cloning
# - default_tag: git tag or branch passed to the git clone command as "-b DEFAULT_TAG" in case the "--unstable" command
# line option is not used
# - unstable_tag: git tag or branch passed to the git clone command in case the "--unstable" command line option is used
# (the priority is unstable_tag > default_tag > "unstable")
# - deps: list of repositories that should be soft linked to the deps folder
# - cmake_args: additional arguments passed to the cmake command
# - no_tests: do not run tests after building
core_repos = {
    "GTest": {
        "url": "https://github.com/google/googletest",
        "default_tag": "main"
    },
    "fmt": {
        "url": "https://github.com/fmtlib/fmt",
        "default_tag": "10.2.1",
        "unstable_tag": "11.0.2"
    },
    "Cpp2Py": {
        "url": "https://github.com/TRIQS/cpp2py.git",
        "default_tag": "unstable",
        "unstable_tag": "main",
    },
    "itertools": {
        "url": "https://github.com/TRIQS/itertools.git",
        "deps": ["GTest"]
    },
    "h5": {
        "url": "https://github.com/TRIQS/h5.git",
        "deps": ["GTest", "Cpp2Py"]
    },
    "mpi": {
        "url": "https://github.com/TRIQS/mpi.git",
        "deps": ["GTest", "itertools"]
    },
    "nda": {
        "url": "https://github.com/TRIQS/nda.git",
        "deps": ["GTest", "Cpp2Py", "itertools", "mpi", "h5"]
    },
    "cppdlr": {
        "url": "https://github.com/flatironinstitute/cppdlr.git",
        "deps": ["GTest", "nda", "fmt"],
        "unstable_tag": "main"
    },
    "triqs": {
        "url": "https://github.com/TRIQS/triqs.git",
        "deps": ["GTest", "Cpp2Py", "itertools", "mpi", "h5", "nda", "fmt", "cppdlr"]
    }
}


app_repos = {
    "cthyb": {
        "url": "https://github.com/TRIQS/cthyb.git",
        "deps": ["GTest", "Cpp2Py"]
    },
    "dft_tools": {
        "url": "https://github.com/TRIQS/dft_tools.git",
        "deps": ["GTest", "Cpp2Py"]
    },
    "tprf": {
        "url": "https://github.com/TRIQS/tprf.git",
        "deps": ["GTest", "Cpp2Py"]
    },
    "maxent": {
        "url": "https://github.com/TRIQS/maxent.git"
    },
    "hubbardI": {
        "url": "https://github.com/TRIQS/hubbardI.git",
        "deps": ["GTest", "Cpp2Py"]
    },
    "hartree_fock": {
        "url": "https://github.com/TRIQS/hartree_fock.git"
    },
    "solid_dmft": {
        "url": "https://github.com/TRIQS/solid_dmft.git"
    }
}


# merge the core_repos and app_repos dictionaries
all_repos = {**core_repos, **app_repos}


def clone_repo(name, clone_dir, clone_unstable, clone_args="--depth 1"):
    """
    Clone the given repository and its dependencies recursively and create soft links for the dependencies.

    Parameters
    ----------
    name : str
        Name of the repository to clone.
    clone_dir : str
        Directory where the repository should be cloned.
    clone_unstable : bool
        Shoule the unstable branch (if available) be cloned?
    clone_args : str
        Additional arguments to pass to the git clone command.

    Raises
    ------
    subprocess.CalledProcessError
        If the git clone command fails.
    ValueError
        If a specified repository is not found in the repository dictionary.
    """

    print('-' * 25)
    print(f"Cloning the repository {name}")

    # check if the repo is in the repo dictionary
    if name not in all_repos:
        raise ValueError(f"Repository {name} not found in the repository dictionary.")

    # get repo information from the dictionary
    repo_info = all_repos[name]
    url = repo_info["url"]
    default_tag = repo_info.get("default_tag", "")
    unstable_tag = repo_info.get("unstable_tag", default_tag if default_tag else "unstable")
    repo_dir = clone_dir + "/" + name + ".src"
    repo_path = Path(repo_dir)

    # clone the repo if hasn't been cloned yet
    if not repo_path.exists():
        tag_args = f"-b {unstable_tag}" if clone_unstable else (f"-b {default_tag}" if default_tag else "")
        subprocess.run(["git", "clone"] + tag_args.split() + clone_args.split() + [url, repo_dir], check=True)
    else:
        print(f"Repository {name} already exists --> skipping")

    # clone its dependencies recursively and create soft links
    deps = repo_info.get("deps", [])
    if deps:
        print(f"Cloning the dependencies of {name}: {deps}")
        deps_path = repo_path / "deps"
        for dep_name in deps:
            # clone the dependency
            clone_repo(dep_name, clone_dir, clone_unstable, clone_args)

            # create a soft link in the deps folder
            dst = deps_path / dep_name
            src = Path(clone_dir + "/" + dep_name + ".src")
            if not dst.is_symlink() and src.exists():
                print(f"Creating soft link {dep_name} for {name}")
                os.symlink(os.path.relpath(src, deps_path), dst)
            else:
                print(f"Soft link {dep_name} for {name} already exists --> skipping")
    print('-' * 25)

    return

def main():

    # epilog for the help message
    epilog = f"""
    repositories:
        core: {', '.join(core_repos.keys())}
        apps: {', '.join(app_repos.keys())}

    examples:
        - clone and archive all repositories (core + apps):
            python {os.path.relpath(__file__)} -c -a

        - build + install all repositories (building in parallel with 12 cores):
            python {os.path.relpath(__file__)} -i -j 12

        - clone and install only triqs and cthyb (building in parallel with 12 cores):
            python {os.path.relpath(__file__)} -c triqs cthyb -i triqs cthyb -j 12
    """

    # parse command line arguments
    parser = argparse.ArgumentParser(description="Clone, archive and install TRIQS related repositories", epilog=epilog,
                                     formatter_class=argparse.RawDescriptionHelpFormatter, allow_abbrev=False)
    parser.add_argument("-d", "--dir", default="triqs_repositories", help="directory containing (the cloned) repositories (default: ./triqs_repositories)")
    parser.add_argument("-a", "--archive", action="store_true", help="archive the repository directory DIR (default: False)")
    parser.add_argument("-c", "--clone", nargs="*", help="clone all or only specific repositories into DIR")
    parser.add_argument("-i", "--install", nargs="*", help="build and install all or only specific repositories")
    parser.add_argument("--unstable", action="store_true", help="clone unstable branches when possible (default: False)")
    parser.add_argument("--install-prefix", help="CMake install prefix (default: DIR/install)")
    parser.add_argument("--cmake-args", default="", help="additional arguments to pass to the cmake command (default: '')")
    parser.add_argument("--no-tests", action="store_true", help="do not run tests after building (default: False)")
    parser.add_argument("-j", "--ncores", default=1, type=int, help="number of cores to use for building and installing (default: 1)")
    args = parser.parse_args()

    # print help if no arguments are provided
    if len(sys.argv) == 1:
        parser.print_help(sys.stderr)
        sys.exit(1)

    # clone repositories
    if args.clone is not None:
        print('=' * 25)

        # check repos to clone
        if not args.clone:
            # choose all repos if none are specified
            print("No repositories specified to clone --> cloning all repositories")
            args.clone = [*core_repos, *app_repos]

        # output repos to clone
        print(f"Cloning the following repositories: {args.clone}")
        print('=' * 25)

        # loop over repos to clone and clone its dependencies recursively
        for name in args.clone:
            clone_repo(name, clone_unstable=args.unstable, clone_dir=args.dir)

    # archive the repository directory
    if args.archive:
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

    # build and install repositories
    if args.install is not None:
        print('=' * 25)

        # check repos to install
        if not args.install:
            # if no repos are specified, install triqs and all apps (core libs are installed by triqs automatically)
            print("No repositories specified to install --> installing TRIQS and all apps (core libraries are installed by TRIQS automatically)")
            args.install = ['triqs', *app_repos]
        else:
            print(f"User specified repositories to install: {args.install}")

            # remove duplicate repos
            args.install = list(set(args.install))

            # loop over remaining repos
            for name in args.install:
                # check if the repo is in the repo information
                if name not in all_repos:
                    raise ValueError(f"Repository {name} not found in the repository information.")

                # filter out dependencies that will be installed by another repo with higher priority
                for dep_name in all_repos[name].get("deps", []):
                    if dep_name in args.install:
                        print(f"Repository {dep_name} will be installed by {name} automatically --> removing")
                        args.install = [x for x in args.install if x != dep_name]

            # sort repos according to the order in the all_repos dictionary
            args.install.sort(key=lambda x: list(all_repos.keys()).index(x))

        # check if the specified repos exists
        directories = [d for d in os.listdir(args.dir+'/') if os.path.isdir(args.dir+'/'+d)]
        for name in reversed(args.install):
            if name+'.src' not in directories:
                print(f"Repository {name} not found in the directory {args.dir} --> skipping")
                args.install.remove(name)

        if not args.install:
            raise ValueError("No repositories to install found in the directory {args.dir}")

        # output repositories to install
        print(f"Installing the following repositories: {args.install}")
        print('=' * 25)

        # loop over repos to install
        for name in args.install:
            print(f"Building and installing {name}")

            # get repo information
            repo_info = all_repos[name]
            repo_path = Path(args.dir + "/" + name + ".src")

            # set up build and install directory
            build_dir = Path(args.dir + "/" + name + ".build")
            install_prefix = Path(args.dir + "/install" if args.install_prefix is None else args.install_prefix)
            cmake_install_prefix = "-DCMAKE_INSTALL_PREFIX=" + str(install_prefix.absolute())

            # source triqs shell variables for apps if not already in the environment
            if name in app_repos and "TRIQS_ROOT" not in os.environ:
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

    return


if __name__ == "__main__":
    main()
