
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_test_macros.hpp>
#include <catch2/internal/catch_compile_time_section.hpp>

#include <vector>

// Tests C++17 _________________________________________________________________

TEST_CASE( "COMPILE_TIME_SECTIONS C++17" ) {
    REQUIRE( true );
#if defined( CATCH_CONFIG_CPP17_OR_GREATER ) && \
    !defined( CATCH_CONFIg_CPP20_OR_GREATER )

    // COMPILE_TIME_SECTION works but does not run anything at runtime in C++17

    constexpr int captureMe = 78;
    COMPILE_TIME_SECTION( "C++17 Compile time section" ) {
        int c = 1;
        c *= 10;
        COMPILE_TIME_REQUIRE( c == 10 );
    };

    COMPILE_TIME_SECTION( "C++17 Compile time section 2" ) {
        COMPILE_TIME_REQUIRE(captureMe == 78);
    };

    COMPILE_TIME_SECTION( "C++17 Compile time section 3" ) {
        int c = 1;
        c *= 10;
        COMPILE_TIME_REQUIRE( c == 10 );
    };

#endif
}

// Tests C++20 _________________________________________________________________

// The tests below are not supported before C++20.
// Workaround to have the same test report is to undef compile time macros
// and redefine them again
#ifndef CATCH_CPP20_OR_GREATER
#    undef COMPILE_TIME_SECTION
#    define COMPILE_TIME_SECTION( ... )                                        \
        SECTION( __VA_ARGS__ " [Passed at compile time]" ) {                   \
            Catch::CompileTimeSection::addFakeAssertion( __FILE__, __LINE__ ); \
        }                                                                      \
        if ( false )
#    define FAIL_AT_RUNTIME_IF_NOT_CPP_20( macroName, condition ) \
        failBecauseEvaluatedAtRuntime( macroName##_catch_sr,      \
                                       CATCH_INTERNAL_LINEINFO,   \
                                       condition##_catch_sr )
#else
#    define FAIL_AT_RUNTIME_IF_NOT_CPP_20( ... )
#endif

TEST_CASE( "Simple compile time section", "[compilation]" ) {
    constexpr int captured = 38;
    COMPILE_TIME_SECTION( "Simple compile time section" ) {
        int x = 5;
        x *= 10;
        COMPILE_TIME_REQUIRE( x == 50 );
        COMPILE_TIME_REQUIRE( captured == 38 );
        COMPILE_TIME_REQUIRE_FALSE( x != 50 );
    };
}

TEST_CASE( "Nested compile time sections" ) {
    COMPILE_TIME_SECTION( "First" ) {
        int x = 19;
        COMPILE_TIME_REQUIRE( x == 19 );
        x *= 10;
        COMPILE_TIME_REQUIRE( x == 190 );

        COMPILE_TIME_SECTION( "Nested 1" ) {
            std::vector<int> v{ 1, 2, 3 };
            COMPILE_TIME_REQUIRE( v.size() == 3 );
            v.push_back( 4 );
            COMPILE_TIME_REQUIRE( v.size() == 4 );
            COMPILE_TIME_REQUIRE( v[3] == 4 );

            COMPILE_TIME_SECTION( "Nested 2" ){
                // Keeping empty here should be perfectly fine
            };
        };
    };
}

#ifndef CATCH_CPP20_OR_GREATER
#    undef INTERNAL_CATCH_AT_COMPILE_TIME
#    define INTERNAL_CATCH_AT_COMPILE_TIME( ... ) false
#endif

TEST_CASE( "COMPILE_TIME_REQUIRE should always fail at runtime" ) {
    COMPILE_TIME_REQUIRE( true );
    COMPILE_TIME_REQUIRE( false );
}

TEST_CASE( "COMPILE_TIME_REQUIRE_FALSE should always fail at runtime" ) {
    COMPILE_TIME_REQUIRE_FALSE( false );
    COMPILE_TIME_REQUIRE_FALSE( true );
}