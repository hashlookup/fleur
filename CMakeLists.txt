project("hashlookup-yara" C)
cmake_minimum_required(VERSION 3.16)

set(TARGET_GROUP production CACHE STRING "Group to build")
set(CMAKE_BUILD_TYPE RelWithDebInfo)

add_subdirectory(fnv)
add_subdirectory(fleur)
add_subdirectory(myutils)

if(TARGET_GROUP STREQUAL production)
    add_subdirectory(fleurcli)
    set_target_properties(fleurcli PROPERTIES C11_STANDARD_REQUIRED ON)
elseif(TARGET_GROUP STREQUAL test)
    include(CTest)

    add_subdirectory(external)
    add_subdirectory(test)
else()
    message(FATAL_ERROR "Given TARGET_GROUP unknown")
endif()