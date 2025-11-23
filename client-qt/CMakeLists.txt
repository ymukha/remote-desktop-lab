find_package(Qt6 REQUIRED COMPONENTS Widgets Gui Core Network)

add_executable(rdl_client_qt
    src/main.cpp
    src/network_client.cpp
)

target_include_directories(rdl_client_qt
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(rdl_client_qt
    PRIVATE
        rdl_core
        Qt6::Widgets
        Qt6::Gui
        Qt6::Core
        Qt6::Network
)

target_compile_features(rdl_client_qt PRIVATE cxx_std_17)

set_target_properties(rdl_client_qt
    PROPERTIES
        FOLDER "App"
        WIN32_EXECUTABLE TRUE
        AUTOMOC ON
)

set(QT_DLLS
    "$<IF:$<CONFIG:Debug>,Qt6Cored.dll,Qt6Core.dll>"
    "$<IF:$<CONFIG:Debug>,Qt6Guid.dll,Qt6Gui.dll>"
    "$<IF:$<CONFIG:Debug>,Qt6Widgetsd.dll,Qt6Widgets.dll>"
    "$<IF:$<CONFIG:Debug>,Qt6Networkd.dll,Qt6Network.dll>"
    "$<IF:$<CONFIG:Debug>,zlibd1.dll,zlib1.dll>"
    "$<IF:$<CONFIG:Debug>,pcre2-16d.dll,pcre2-16.dll>"
    double-conversion.dll
)

set(QT_PLATFORM_DLLS
    "$<IF:$<CONFIG:Debug>,qwindowsd.dll,qwindows.dll>"
    "$<IF:$<CONFIG:Debug>,qminimald.dll,qminimal.dll>"
    "$<IF:$<CONFIG:Debug>,qdirect2dd.dll,qdirect2d.dll>"
)

add_custom_command(TARGET rdl_client_qt POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${RDL_DEPLOY_DIR}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${RDL_DEPLOY_DIR}/platforms"

    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:rdl_client_qt>"
        "${RDL_DEPLOY_DIR}/"

    COMMAND ${CMAKE_COMMAND}
        -Dsrc_dir="$<IF:$<CONFIG:Debug>,${VCPKG_INSTALLED_DIR}/debug/bin,${VCPKG_INSTALLED_DIR}/bin>"
        -Ddst_dir="${RDL_DEPLOY_DIR}"
        -Dfiles="${QT_DLLS}"
        -P "${CMAKE_SOURCE_DIR}/cmake/copy_files.cmake"

    COMMAND ${CMAKE_COMMAND}
        -Dsrc_dir="$<IF:$<CONFIG:Debug>,${VCPKG_INSTALLED_DIR}/debug/Qt6/plugins/platforms,${VCPKG_INSTALLED_DIR}/Qt6/plugins/platforms>"
        -Ddst_dir="${RDL_DEPLOY_DIR}/platforms"
        -Dfiles="${QT_PLATFORM_DLLS}"
        -P "${CMAKE_SOURCE_DIR}/cmake/copy_files.cmake"
)