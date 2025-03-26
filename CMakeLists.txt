cmake_minimum_required(VERSION 3.28)
project(opendrive-gd LANGUAGES CXX)


add_compile_options(-fPIC)

set(GODOTCPP_DISABLE_EXCEPTIONS OFF)
add_subdirectory(third_party/godot-cpp EXCLUDE_FROM_ALL)
add_subdirectory(third_party/libOpenDRIVE EXCLUDE_FROM_ALL)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


add_library(
  opendrive-gd
  SHARED
    src/extension.cpp

    src/gdx.h
    src/utils.h

    src/open_drive_map.h
)

target_link_libraries(opendrive-gd PRIVATE godot::cpp OpenDrive)


install(TARGETS opendrive-gd DESTINATION .)
