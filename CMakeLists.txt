cmake_minimum_required(VERSION 3.15)

project(shimmer_wallet DESCRIPTION "IOTA Shimmer Wallet")

enable_language(C)

option(WALLET_TESTS "Enable Wallet library test cases" OFF)

# fetch external libs
include(ExternalProject)

if(WALLET_TESTS)
  enable_testing()
  include(cmake/unity.cmake)
endif()

include(cmake/uthash.cmake)
include(cmake/sodium.cmake)
include(cmake/base58.cmake)
include(cmake/cjson.cmake)

if(ENABLED_JEMALLOC)
  include(cmake/jemalloc.cmake)
endif()

if(IOTA_ASAN_ENABLED)
  if(UNIX AND (CMAKE_BUILD_TYPE STREQUAL "Debug" OR NOT CMAKE_BUILD_TYPE))
    include(CheckCCompilerFlag)
    set(CMAKE_REQUIRED_LIBRARIES "asan")
    if(CMAKE_C_COMPILER_ID MATCHES "Clang" OR CMAKE_C_COMPILER_ID MATCHES "GNU")
      check_c_compiler_flag("-fsanitize=address" HAS_ASAN)
      if(HAS_ASAN)
        add_compile_options("-fsanitize=address" "-fno-omit-frame-pointer")
        set(HAS_ASAN_ENABLED
            ON
            CACHE BOOL "Asan has enabled on target" FORCE)
        message("AddressSanitizer is enabled")
      else()
        message(WARNING "ASan is not supported")
      endif()
    endif()
  else()
    message(WARNING "ASan is not supported on Windows or Release build")
  endif()
endif()

find_package(CURL REQUIRED)

# links libraries in the sandbox
link_directories("${CMAKE_INSTALL_PREFIX}/lib")

add_subdirectory(src)

if(WALLET_TESTS)
  add_subdirectory(tests)
endif()
