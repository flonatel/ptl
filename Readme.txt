
* Preconditions

** Install development environment
   In addition to the used compiler, the following packages
   are needed (package names are the ones from Debian 6 - 
   depending on the distribution this might differ):
   lcov autoconf-archive libtool automake autoconf git make

* Basic usage

** Check out everything from git
   $ git clone https://github.com/flonatel/ptl

** Init the autotools
   $ cd ptl
   $ bash autotools_init.sh
   $ cd ..

** Create a build directory
   For each 'version' build a seperate build directory:
   This might be compiler dependend, debug enabled, ...

   $ mkdir BUILD_GCC_DEBUG
   $ cd BUILD_GCC_DEBUG
   
** Call configure
   $ ${PWD}/../ptl/configure

   To see all available options, call configure with --help.
   The most interesting options are currently:
   --enable-debug
   --enable-profiling
   --enable-coverage
   --enable-valgrind

** Build
   $ make -j4 

** Run test cases
   $ make -j4 check


Local Variables:
mode: outline
End:
