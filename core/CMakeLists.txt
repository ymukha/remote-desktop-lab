add_library(rdl_core
    STATIC
        src/version.cpp
)

target_include_directories(rdl_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(rdl_core
    PUBLIC
        cxx_std_17
)