# Copyright 2025 Nana Sakisaka
#
# Distributed under the Boost Software License, Version 1.0.
# https://www.boost.org/LICENSE_1_0.txt

# For Spirit-specific tests.
add_library(boost_spirit_x4_cxx_test INTERFACE)
set_target_properties(boost_spirit_x4_cxx_test PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(boost_spirit_x4_cxx_test INTERFACE _boost_spirit_x4_cxx_common)

# For external libraries. Excludes strict warning, etc.
add_library(boost_spirit_x4_cxx_external INTERFACE)
set_target_properties(boost_spirit_x4_cxx_external PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(boost_spirit_x4_cxx_external INTERFACE _boost_spirit_x4_cxx_common)

if(MSVC)
    target_compile_options(
        boost_spirit_x4_cxx_test
        INTERFACE /W4 /analyze /analyze:external-
    )
    target_compile_options(
        boost_spirit_x4_cxx_external
        INTERFACE /analyze-
    )

else() # non-MSVC
    target_compile_options(
        boost_spirit_x4_cxx_test
        INTERFACE
            -Wall -Wextra -pedantic
    )
endif()

# --------------------------------------------

# https://github.com/catchorg/Catch2/blob/devel/docs/configuration.md
set(CATCH_CONFIG_FAST_COMPILE ON)
set(CATCH_CONFIG_NO_USE_BUILTIN_CONSTANT_P ON)

include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2
    GIT_TAG v3.10.0
    GIT_SHALLOW ON
)
FetchContent_MakeAvailable(Catch2)

set_target_properties(Catch2 PROPERTIES CXX_EXTENSIONS OFF)
target_compile_definitions(Catch2 PUBLIC DO_NOT_USE_WMAIN)
target_link_libraries(Catch2 PRIVATE boost_spirit_x4_cxx_external)
target_link_libraries(Catch2WithMain PRIVATE boost_spirit_x4_cxx_external)

target_link_libraries(boost_spirit_x4_cxx_test INTERFACE Catch2::Catch2WithMain)

# --------------------------------------------

add_subdirectory(x4)

