cmake_minimum_required(VERSION 3.16)

project(UdeABook LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(UdeABook main.cpp
    sistema.h
    album.h
    usuario.h
    mensaje.h
    medidor.h)

include(GNUInstallDirs)
install(TARGETS UdeABook
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
