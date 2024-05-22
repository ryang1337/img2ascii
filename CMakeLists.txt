cmake_minimum_required(VERSION 3.26)
project(img2ascii)

include_directories(/usr/local/include)
link_directories(/usr/local/lib)

add_executable(img2ascii img2ascii.cc)
target_link_libraries(img2ascii png)
