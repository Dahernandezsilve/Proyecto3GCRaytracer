cmake_minimum_required(VERSION 3.26)
project(Proyecto3GCRaytracer)

set(CMAKE_CXX_STANDARD 23)

add_executable(Proyecto3GCRaytracer scripts/main.cpp
        scripts/color.h
)

# --- SDL2 SETUP ---
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)
set(SDL2_PATH "SDL2/x86_64-w64-mingw32")
set(SDL2_IMAGE_PATH "SDL2/x86_64-w64-mingw32")

find_package(SDL2_image REQUIRED)
find_package(SDL2 REQUIRED)
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARY})



