
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_COMPILE_TIME_ASSERTS_HPP_INCLUDED
#define CATCH_COMPILE_TIME_ASSERTS_HPP_INCLUDED
#include <catch2/internal/catch_compiler_capabilities.hpp>
#include <catch2/internal/catch_test_macro_impl.hpp>

#include <stdexcept>

#ifdef __cpp_lib_is_constant_evaluated
#    include <type_traits>
#    define INTERNAL_CATCH_AT_COMPILE_TIME() std::is_constant_evaluated()
#    define INTERNAL_CATCH_AT_RUNTIME() !std::is_constant_evaluated()
#else
#    define INTERNAL_CATCH_AT_COMPILE_TIME() true
#    define INTERNAL_CATCH_AT_RUNTIME() true
#endif

namespace Catch {

    using CompileTimeError = std::runtime_error;

    void failBecauseEvaluatedAtRuntime( StringRef macroName,
                                        SourceLineInfo lineInfo,
                                        StringRef condition );

} // namespace Catch

#define INTERNAL_CATCH_ENSURE_COMPILE_TIME_ASSERT_NOT_RUNTIME( macroName,  \
                                                               condition ) \
    if ( INTERNAL_CATCH_AT_COMPILE_TIME() ) {                              \
    } else {                                                               \
        failBecauseEvaluatedAtRuntime( macroName##_catch_sr,               \
                                       CATCH_INTERNAL_LINEINFO,            \
                                       condition##_catch_sr );             \
    }

#define INTERNAL_CATCH_COMPILE_TIME_ASSERT(                                    \
    macroName, resultDisposition, condition )                                  \
    INTERNAL_CATCH_ENSURE_COMPILE_TIME_ASSERT_NOT_RUNTIME( macroName,          \
                                                           #condition )        \
    if ( ( condition ) != Catch::isFalseTest( resultDisposition ) ) {          \
    } else {                                                                   \
        if ( INTERNAL_CATCH_AT_RUNTIME() ) {                                   \
            INTERNAL_CATCH_TEST( macroName, resultDisposition, condition );    \
        }                                                                      \
        if ( INTERNAL_CATCH_AT_COMPILE_TIME() ) {                              \
            throw Catch::CompileTimeError( "Compile time assertion failed." ); \
        }                                                                      \
    }                                                                          \
    static_assert( true, "Force the addition of ';'" )

#endif // CATCH_COMPILE_TIME_ASSERTS_HPP_INCLUDED
