cmake_minimum_required(VERSION 3.15)
project(NBody_Lab1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

add_executable(nbody_sequential main.cpp)

if(MSVC)
    # 1. Добавляем нужные оптимизации
    target_compile_options(nbody_sequential PRIVATE /Ox /arch:AVX2 /GS-)

    # 2. Удаляем конфликтующий флаг /RTC1 из стандартных настроек Debug
    string(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    # Устанавливаем обновленную строку флагов в текущем скоупе
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
else()
    target_compile_options(nbody_sequential PRIVATE -O3 -march=native -ffast-math)
endif()