cmake_minimum_required(VERSION 3.12)
project(E2)

set(CMAKE_CXX_STANDARD 14)

add_executable(E2 main.cpp BigInteger.h BigInteger.cpp E2.cpp E2.h HashFunction.cpp HashFunction.h DigitalSignature.cpp DigitalSignature.h)