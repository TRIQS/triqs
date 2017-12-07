#!/bin/sh

# Set this variable to your desired install directory
INSTALL_PREFIX=$(pwd)/install

# Set the number of cores for the compilation
NCORES=4

# Clone the git repositories for cpp2py and triqs
git clone https://github.com/TRIQS/cpp2py cpp2py.src
git clone https://github.com/TRIQS/triqs --branch unstable triqs.src

# Configure the cpp2py build process
mkdir -p cpp2py.build && cd cpp2py.build
cmake ../cpp2py.src -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX

# Build and install cpp2py
make -j$NCORES && make install
cd ../

# Load the cpp2py installation into your environment
source $INSTALL_PREFIX/share/cpp2pyvars.sh

# Use cmake to configure the triqs build process
mkdir -p triqs.build && cd triqs.build
cmake ../triqs.src -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX

# Build, test and install triqs
make -j$NCORES && make test && make install
cd ../

# Load the triqs installation into your environment
source $INSTALL_PREFIX/share/triqsvars.sh

echo 
echo "If you want to automatically load triqs and cpp2py into your environment,"
echo "please add the following two lines to your ~/.bash_profile (or ~/.zprofile):"
echo "source $INSTALL_PREFIX/share/cpp2pyvars.sh"
echo "source $INSTALL_PREFIX/share/triqsvars.sh"
