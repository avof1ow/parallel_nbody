cmake_minimum_required(VERSION 3.18)

set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -allow-unsupported-compiler")

project(NBody_CUDA LANGUAGES CXX CUDA)

set(CMAKE_CUDA_STANDARD 17)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)

add_executable(nbody_cuda kernel.cu
        kernel.cu)

set_target_properties(nbody_cuda PROPERTIES CUDA_ARCHITECTURES "native")
