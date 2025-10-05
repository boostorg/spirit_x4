# Copyright 2025 Nana Sakisaka
#
# Distributed under the Boost Software License, Version 1.0.
# https://www.boost.org/LICENSE_1_0.txt

function(x4_define_test_headers test_name)
    target_sources(
        x4_${test_name}_test
        PRIVATE FILE_SET HEADERS FILES
            ${ARGN}
    )
endfunction()

function(x4_define_test test_name)
    # message("test_name: ${test_name}, sources: ${ARGN}")
    add_executable(x4_${test_name}_test ${ARGN})
    x4_define_test_headers(${test_name} test.hpp)

    set_target_properties(
        x4_${test_name}_test
        PROPERTIES CXX_EXTENSIONS OFF
    )

    if(MSVC)
        # Prevent "Warning: Conflicting <Type> entries detected" error
        # set_source_files_properties(
        #     "${PROJECT_SOURCE_DIR}/boost_spirit_x4.natvis"
        #     TARGET_DIRECTORY x4_${test_name}_test
        #     PROPERTIES VS_SETTINGS "ExcludedFromBuild=true"
        # )

        # target_sources(x4_${test_name}_test PRIVATE cpp.hint)
    endif()

    target_link_libraries(
        x4_${test_name}_test
        PRIVATE Boost::spirit_x4 boost_spirit_x4_cxx_test
    )
    add_test(
        NAME x4_${test_name}_test
        COMMAND x4_${test_name}_test --colour-mode=ansi
    )
endfunction()

function(x4_define_tests)
    foreach(test_name IN LISTS ARGV)
        x4_define_test(${test_name} ${test_name}.cpp)
    endforeach()
endfunction()

x4_define_tests(
    actions
    alternative
    and_predicate
    as
    attr
    attribute
    attribute_type_check
    binary
    bool
    char
    char_class
    container_support
    context
    debug
    difference
    eoi
    eol
    eps
    error_handler
    expect
    extract_int
    int
    iterator
    kleene
    lexeme
    list
    lit
    matches
    not_predicate
    no_case
    no_skip
    omit
    optional
    parser
    plus
    raw
    real1
    real2
    real3
    repeat
    rule1
    rule2
    rule3
    rule4
    seek
    sequence
    skip
    symbols1
    symbols2
    symbols3
    to_utf8
    tst
    uint
    uint_radix
    unused
    with
    x3_rule_problem
)

x4_define_test(rule_separate_tu rule_separate_tu.cpp rule_separate_tu_grammar.cpp)
x4_define_test_headers(rule_separate_tu rule_separate_tu_grammar.hpp)

x4_define_test(grammar grammar.cpp grammar_linker.cpp)
x4_define_test_headers(grammar grammar.hpp)

x4_define_test_headers(real1 real.hpp)
x4_define_test_headers(real2 real.hpp)
x4_define_test_headers(real3 real.hpp)
