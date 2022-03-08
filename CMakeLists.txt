cmake_minimum_required(VERSION 3.21)
include(FindMatlab)
project(readimg C)

set(CMAKE_C_STANDARD 99)

set(Matlab_ROOT_DIR "/Applications/MATLAB_R2021b.app")
find_package(Matlab)
matlab_add_mex(NAME mex SRC readimg imgdecoder.c imgdecoder.h imgutil.h imgutil.c)
