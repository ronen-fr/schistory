cmake_minimum_required(VERSION 3.22)


set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_EXTENSIONS OFF)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


project(sc CXX)

#dd_compile_options(-std=c++20)



set(SOURCE a.cc)


add_executable(${PROJECT_NAME} ${SOURCE})
#set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

