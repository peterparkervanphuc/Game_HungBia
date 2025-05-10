cmake_minimum_required(VERSION 3.10)
project(HungBia)

set(CMAKE_CXX_STANDARD 17)

# Add SDL2 libs path
set(SDL2_DIR "${CMAKE_SOURCE_DIR}/libs/SDL2/cmake")
set(SDL2_IMAGE_DIR "${CMAKE_SOURCE_DIR}/libs/SDL2_image/cmake")
set(SDL2_TTF_DIR "${CMAKE_SOURCE_DIR}/libs/SDL2_ttf/cmake")
set(SDL2_MIXER_DIR "${CMAKE_SOURCE_DIR}/libs/SDL2_mixer/cmake")

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${SDL2_IMAGE_INCLUDE_DIRS})
include_directories(${SDL2_TTF_INCLUDE_DIRS})
include_directories(${SDL2_MIXER_INCLUDE_DIRS})

add_executable(HungBia 
    main.cpp
    src/GameManager.cpp
)

target_link_libraries(HungBia
    SDL2::SDL2
    SDL2_image::SDL2_image
    SDL2_ttf::SDL2_ttf
    SDL2_mixer::SDL2_mixer
)
