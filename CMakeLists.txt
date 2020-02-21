cmake_minimum_required(VERSION 3.15)
project(ust2lrc)

add_executable(ust2lrc src/ust2lrc.c)

install(TARGETS ust2lrc
        RUNTIME DESTINATION bin
)