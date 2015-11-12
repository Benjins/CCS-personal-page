#!/bin/bash

# g++4.8.1
if [ "$1" == "g++" ]; then sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test; fi

# clang 3.4
if [ "$1" == "clang++" ]; then sudo add-apt-repository -y ppa:h-rayflood/llvm; fi
sudo apt-get install freeglut3-dev glew1.5-dev libfreetype6-dev libopenal-dev sshpass
#only install doxygen if we're on g++, save time on clang
if [ "$1" == "g++" ]; then sudo apt-get install doxygen; fi
sudo apt-get update -qq

#g++4.8.1
if [ "$1" == "g++" ]; then sudo apt-get install -qq gcc-4.8 g++-4.8; fi
if [ "$1" == "g++" ]; then sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.8 90; fi

# clang 3.4
if [ "$1" == "clang++" ]; then sudo apt-get install --allow-unauthenticated -qq clang-3.4; fi
if [ "$1" == "clang++" ]; then export CXX="clang++-3.4"; fi