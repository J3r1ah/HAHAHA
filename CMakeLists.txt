```cmake
cmake_minimum_required(VERSION 3.10)
project(GifPlayer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)
find_package(CURL REQUIRED)
find_package(Threads REQUIRED)

add_executable(gif_player gif_player.cpp)
target_link_libraries(gif_player ${OpenCV_LIBS} ${CURL_LIBRARIES} Threads::Threads)
