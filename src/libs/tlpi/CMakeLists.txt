cmake_minimum_required(VERSION 3.16)
project(cmg)

include_directories(.)

file(GLOB SOURCE_FILES *.c)

add_library(tlpi STATIC ${SOURCE_FILES})
