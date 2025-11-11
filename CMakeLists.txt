cmake_minimum_required(VERSION 3.15)
project(WAMount VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Platform check
if(NOT WIN32)
    message(FATAL_ERROR "WAMount only supports Windows")
endif()

# Find WinFsp
# You may need to set WINFSP_PATH to your WinFsp installation directory
if(NOT DEFINED WINFSP_PATH)
    if(DEFINED ENV{WINFSP_PATH})
        set(WINFSP_PATH $ENV{WINFSP_PATH})
    else()
        set(WINFSP_PATH "C:/Program Files (x86)/WinFsp")
    endif()
endif()

if(NOT EXISTS "${WINFSP_PATH}")
    message(WARNING "WinFsp not found at ${WINFSP_PATH}")
    message(WARNING "Please install WinFsp from https://winfsp.dev/")
    message(WARNING "Or set WINFSP_PATH to your WinFsp installation directory")
endif()

# Include directories
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src
    "${WINFSP_PATH}/inc"
)

# Link directories
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    # 64-bit
    link_directories("${WINFSP_PATH}/lib")
else()
    # 32-bit
    link_directories("${WINFSP_PATH}/lib")
endif()

# Source files
set(SOURCES
    src/main.cpp
    src/apfs/container.cpp
    src/apfs/volume.cpp
    src/apfs/btree.cpp
    src/utils/disk_reader.cpp
    src/utils/logging.cpp
    src/winfsp/winfsp_ops.cpp
)

# Headers
set(HEADERS
    include/apfs_types.h
    include/apfs_structures.h
    src/apfs/container.h
    src/apfs/volume.h
    src/apfs/btree.h
    src/utils/disk_reader.h
    src/utils/logging.h
    src/winfsp/winfsp_ops.h
)

# Executable
add_executable(wamount ${SOURCES} ${HEADERS})

# Link WinFsp
target_link_libraries(wamount
    winfsp-x64.dll
)

# Compiler flags
if(MSVC)
    target_compile_options(wamount PRIVATE
        /W4          # Warning level 4
        /WX-         # Don't treat warnings as errors
        /permissive- # Standards conformance
        /utf-8       # UTF-8 source files
    )

    # Set subsystem to console
    set_target_properties(wamount PROPERTIES
        LINK_FLAGS "/SUBSYSTEM:CONSOLE"
    )
else()
    # MinGW or other compilers
    target_compile_options(wamount PRIVATE
        -Wall
        -Wextra
        -pedantic
    )
endif()

# Copy WinFsp DLL to output directory (optional, for convenience)
if(EXISTS "${WINFSP_PATH}/bin/winfsp-x64.dll")
    add_custom_command(TARGET wamount POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${WINFSP_PATH}/bin/winfsp-x64.dll"
        $<TARGET_FILE_DIR:wamount>
        COMMENT "Copying WinFsp DLL to output directory"
    )
endif()

# Installation
install(TARGETS wamount
    RUNTIME DESTINATION bin
)

install(FILES README.md
    DESTINATION .
)

# Print configuration summary
message(STATUS "WAMount Configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  WinFsp path: ${WINFSP_PATH}")
message(STATUS "  Install prefix: ${CMAKE_INSTALL_PREFIX}")
