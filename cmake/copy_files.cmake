# cmake/copy_some_dlls.cmake

if(NOT DEFINED src_dir)
    message(FATAL_ERROR "copy_files.cmake: src_dir is not defined")
endif()

if(NOT DEFINED dst_dir)
    message(FATAL_ERROR "copy_files.cmake: dst_dir is not defined")
endif()

if(NOT DEFINED files)
    message(FATAL_ERROR "copy_files.cmake: files is not defined")
endif()

file(MAKE_DIRECTORY "${dst_dir}")

string(REPLACE " " ";" files "${files}")
foreach(name IN LISTS files)
    set(src "${src_dir}/${name}")
    if(EXISTS "${src}")
        # message(STATUS "copy_files: copying ${src} -> ${dst_dir}")
        file(COPY "${src}" DESTINATION "${dst_dir}")
    else()
        message(WARNING "copy_files: ${src} does not exist")
    endif()
endforeach()
