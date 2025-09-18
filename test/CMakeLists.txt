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

add_subdirectory(x3)
