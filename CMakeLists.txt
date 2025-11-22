cmake_minimum_required(VERSION 3.20)

project(RemoteDesktopLab
        VERSION 0.1.0
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_EXTENSIONS off)

option(RDL_BUILD_SERVER "Build Qt-based server" on)
option(RDL_BUILD_CLIENT "Build Qt-based client" on)

set(VCPKG_INSTALLED_DIR "${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}")
set(RDL_DEPLOY_DIR "${CMAKE_BINARY_DIR}/deploy/$<CONFIG>")

add_subdirectory(core)

set(QT_DEPLOYMENT_REQUIRED false)

if (RDL_BUILD_SERVER)
    set(QT_DEPLOYMENT_REQUIRED true)
    add_subdirectory(server-qt)    
endif()

if (RDL_BUILD_CLIENT)
    set(QT_DEPLOYMENT_REQUIRED true)
    add_subdirectory(client-qt)
endif()