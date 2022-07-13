project("hashlookup-yara" C)
cmake_minimum_required(VERSION 3.16)

set(TARGET_GROUP production CACHE STRING "Group to build")

add_subdirectory(fnv)

if(TARGET_GROUP STREQUAL production)
    add_subdirectory(main)
elseif(TARGET_GROUP STREQUAL test)
    include(CTest)

    add_subdirectory(external)
    add_subdirectory(test)
else()
    message(FATAL_ERROR "Given TARGET_GROUP unknown")
endif()