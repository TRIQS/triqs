#!/bin/bash

# Set this variable to your desired install directory
INSTALL_PREFIX=$(pwd)/install

# Set the number of cores for the compilation
NCORES=4

# Clone the git repository of triqs
git clone https://github.com/TRIQS/triqs triqs.src

# Use cmake to configure the triqs build process
mkdir -p triqs.build && cd triqs.build
cmake ../triqs.src -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX

# Build, test and install triqs
make -j$NCORES && make test && make install
cd ../

# Load the triqs installation into your environment
source $INSTALL_PREFIX/share/triqs/triqsvars.sh

echo 
echo "If you want to automatically load triqs into your environment,"
echo "please add the following line to your ~/.bash_profile (or ~/.zprofile):"
echo "source $INSTALL_PREFIX/share/triqs/triqsvars.sh"
