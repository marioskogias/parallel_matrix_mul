Steps
-----

1. Download and unpack library sources 
$ mkdir ~/tbb
$ cd ~/tbb
$ cp /home/parallel/pps/2013-2014/a2/tbbs_resources/tbb41_20121003oss_src.tgz .
$ tar zxvf tbb41_20121003oss_src.tgz


2. Build library 

(
    To build the library for clones, firts edit lines 45-46 of tbb41_20121003oss_src/build/linux.gcc.inc
    to the following:
    CPLUS = /various/common_tools/in_path/g++-4.7.2
    CONLY = /various/common_tools/in_path/gcc-4.7.2
)

$ cd ~/tbb/tbb41_20121003oss_src
$ make

The following steps are recommended in order to have
version-independent Makefiles in the user programs. 
(for provided Makefile.include file to work "as is", the
 specific file namings should be kept)

2.1 Create symlink for latest version dir 
$ ln -s ~/tbb/tbb41_20121003oss_src ~/tbb/tbb_latest

2.2 Locate actual build dir containing library binaries
$ ls ~/tbb/tbb_latest/build/

The build dir is system specific, and usually is suffixed
with "_release" 
e.g., "linux_intel64_gcc_cc4.4.5_libc2.11.3_kernel2.6.38_release"

Create a txt file in ~/tbb with the name of this build dir
e.g.,
$ cat TBB_LATEST_LIBRARY_VER.txt 
linux_intel64_gcc_cc4.4.5_libc2.11.3_kernel2.6.38_release

2.3 Specify full paths in Makefile.include as appropriate:
i.e.,
$ cat Makefile.include
TBB_DIR = /home/users/anastop/tbb/tbb_latest/
LIBRARY_VER = $(shell cat /home/users/anastop/tbb/TBB_LATEST_LIBRARY_VER.txt)

3. Build user program
$ cd parallel_for
$ make

NOTE: gcc-4.5 or newer required for Lambda-based versions!
NOTE: The makefiles provided are configured for use in clones

4. Run example
$ export LD_LIBRARY_PATH=/home/users/anastop/tbb/tbb41_20121003oss/build/linux_intel64_gcc_cc4.4.5_libc2.11.3_kernel2.6.38_release/
$ ./parallel_for 4 10000000

(apparently, the LD_LIBRARY_PATH needs to be initialized
 only once for a given shell session)
