
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0
#ifndef CATCH_COMPILE_TIME_SECTION_HPP_INCLUDED
#define CATCH_COMPILE_TIME_SECTION_HPP_INCLUDED

#include <catch2/internal/catch_compiler_capabilities.hpp>
#include <catch2/internal/catch_section.hpp>
#include <catch2/internal/catch_unique_name.hpp>

// Check if we have all the required features required to insert Sections at
// runtime automatically
#if __cpp_generic_lambdas >= 201707L && defined( CATCH_CPP20_OR_GREATER ) && \
    defined( __cpp_lib_is_constant_evaluated ) &&                            \
    defined( CATCH_CONFIG_CPP17_OPTIONAL )
#    define CATCH_ADD_SECTION_AT_RUNTIME
#    include <array>
#    include <optional>
#    include <string>

#    define INTERNAL_CATCH_PROVIDE_SECTION_INFO_AS_TEMPLATE_ARGUMENTS( name, \
                                                                       ... ) \
<std::to_array( __FILE__ ), static_cast<size_t>( __LINE__ ),         \
            std::to_array( name )>
#else
#    define INTERNAL_CATCH_PROVIDE_SECTION_INFO_AS_TEMPLATE_ARGUMENTS( ... )
#endif

namespace Catch {

#ifdef CATCH_ADD_SECTION_AT_RUNTIME
    template <auto filename, auto line, auto name>
#endif
    struct CompileTimeSection {
        CompileTimeSection() = delete;
        CompileTimeSection( CompileTimeSection const& ) = delete;
        CompileTimeSection( CompileTimeSection&& ) noexcept = delete;
        CompileTimeSection& operator=( CompileTimeSection const& ) = delete;
        CompileTimeSection& operator=( CompileTimeSection&& ) noexcept = delete;

        template <typename Callable>
        constexpr CompileTimeSection( Callable&& callable ) {
            // clang-format off
            static_assert(
                [&] // error: COMPILE_TIME_SECTION failure. Check the compiler output to find the cause.
                    // You can make the section a SECTION to investigate at runtime.
                { callable(); return true; }(),
                "compile time section error"
            );
            // clang-format on
            if ( !INTERNAL_CATCH_AT_COMPILE_TIME() ) { addRuntimeSection(); }
        }

        // Add a fake assertion to the section to avoid failures because the
        // section is empty
        static void addFakeAssertion( const char* file, size_t lineNumber ) {
            AssertionHandler catchAssertionHandler(
                "COMPILE_TIME_SECTION",
                SourceLineInfo( file, lineNumber ),
                "[Passed at compile time]",
                ResultDisposition::Normal );
            catchAssertionHandler.handleExpr( Decomposer() <= true );
            catchAssertionHandler.complete();
        }

#ifdef CATCH_ADD_SECTION_AT_RUNTIME
        void addRuntimeSection() {
            using namespace std::string_literals;
            fullName.emplace( name.begin(), name.end() - 1 );
            fullName->append( " [Passed at compile time]" );
            fileName.emplace( filename.begin(), filename.end() - 1 );
            const auto sourceInfo = SourceLineInfo( fileName->c_str(), line );
            Section runtimeSection( sourceInfo, fullName->c_str() );
            addFakeAssertion( fileName->c_str(), line );
        }

        std::optional<std::string> fullName;
        std::optional<std::string> fileName;
#else
        constexpr void addRuntimeSection() {}
#endif
    };

} // namespace Catch

#ifdef CATCH_CPP17_OR_GREATER

#    define INTERNAL_CATCH_COMPILE_TIME_SECTION( ... )             \
        [[maybe_unused]] const ::Catch::CompileTimeSection         \
        INTERNAL_CATCH_PROVIDE_SECTION_INFO_AS_TEMPLATE_ARGUMENTS( \
            __VA_ARGS__, _ )                                       \
            INTERNAL_CATCH_UNIQUE_NAME(                            \
                catch_internal_CompileTimeSection ) = [&]()

#else
#    define INTERNAL_CATCH_COMPILE_TIME_SECTION( ... )                        \
        static_assert( false, "C++17 is required for COMPILE_TIME_SECTION" ); \
        if ( false ) {                                                        \
        } else
#endif

#undef CATCH_ADD_SECTION_AT_RUNTIME

#endif // CATCH_COMPILE_TIME_SECTION_HPP_INCLUDED
