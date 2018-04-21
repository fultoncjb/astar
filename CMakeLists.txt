
cmake_minimum_required(VERSION 3.9)

find_package(PkgConfig REQUIRED)

pkg_check_modules(GTKMM gtkmm-3.0)

add_executable(AstarGui astar.cpp
        jsoncpp.cpp
        main.cpp
        menuwindow.cpp)
target_include_directories(AstarGui PUBLIC json ${GTKMM_INCLUDE_DIRS})
target_link_libraries(AstarGui PUBLIC ${GTKMM_LIBRARIES})