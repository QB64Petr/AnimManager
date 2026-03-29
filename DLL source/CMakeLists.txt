cmake_minimum_required(VERSION 3.20)

cmake_policy(SET CMP0091 NEW)

project(webpanim C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN OFF)

option(WEBPANIM_USE_BUNDLED_LIBWEBP "Build against third_party/libwebp" OFF)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

add_library(webpanim SHARED
    src/webpanim.c
)

if(WIN32 AND CMAKE_SIZEOF_VOID_P EQUAL 4)
    target_sources(webpanim PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/webpanim.def)
endif()

target_compile_definitions(webpanim PRIVATE WEBPANIM_BUILD=1)
target_include_directories(webpanim PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(WEBPANIM_USE_BUNDLED_LIBWEBP)
    add_subdirectory(third_party/libwebp EXCLUDE_FROM_ALL)
    target_include_directories(webpanim PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/third_party/libwebp/src)

    if(TARGET webpdemux AND TARGET webpdecoder AND TARGET webp)
        target_link_libraries(webpanim PRIVATE webpdemux webpdecoder webp)
    elseif(TARGET WebP::webpdemux AND TARGET WebP::webpdecoder AND TARGET WebP::webp)
        target_link_libraries(webpanim PRIVATE WebP::webpdemux WebP::webpdecoder WebP::webp)
    else()
        message(FATAL_ERROR "Bundled libwebp targets were found, but target names were not recognized")
    endif()
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LIBWEBP REQUIRED IMPORTED_TARGET libwebpdemux libwebp)
    target_link_libraries(webpanim PRIVATE PkgConfig::LIBWEBP)
endif()

if(WIN32)
    set_target_properties(webpanim PROPERTIES
        OUTPUT_NAME "webpanim"
        PREFIX ""
    )
else()
    set_target_properties(webpanim PROPERTIES
        OUTPUT_NAME "webpanim"
        PREFIX "lib"
    )
endif()