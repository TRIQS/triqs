#!/bin/sh

# The install directory
INSTALL_PREFIX=$HOME/triqs_install

# Uncomment if you want to use a specific compiler
#export CXX=clang++

# Number of cores for the compilation
NCORES=4

# Clone the sources
git clone https://github.com/TRIQS/cpp2py cpp2py.src
git clone https://github.com/TRIQS/triqs --branch unstable triqs.src

# Install cpp2py
mkdir -p cpp2py.build && cd cpp2py.build
cmake ../cpp2py.src -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX
make -j$NCORES && make install
cd ../
source $INSTALL_PREFIX/share/cpp2pyvars.sh

# Install triqs
mkdir -p triqs.build && cd triqs.build
cmake ../triqs.src -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX
make -j$NCORES && make test && make install
cd ../
source $INSTALL_PREFIX/share/triqsvars.sh

echo 
echo "Set up your environment variables by adding to .bashrc (or .zshrc):"
echo "source $INSTALL_PREFIX/share/cpp2pyvars.sh"
echo "source $INSTALL_PREFIX/share/triqsvars.sh"

