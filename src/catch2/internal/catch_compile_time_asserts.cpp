
//              Copyright Catch2 Authors
// Distributed under the Boost Software License, Version 1.0.
//   (See accompanying file LICENSE.txt or copy at
//        https://www.boost.org/LICENSE_1_0.txt)

// SPDX-License-Identifier: BSL-1.0

#include <catch2/catch_message.hpp>
#include <catch2/internal/catch_compile_time_asserts.hpp>

namespace Catch {

    void failBecauseEvaluatedAtRuntime( const StringRef macroName,
                                        const SourceLineInfo lineInfo,
                                        const StringRef condition ) {
        AssertionHandler catchAssertionHandler(
            macroName,
            lineInfo,
            StringRef(),
            ResultDisposition::ContinueOnFailure );

        auto message =
            macroName + "(" + condition +
            ") was evaluated at runtime, it should be evaluated at compile "
            "time in a COMPILE_TIME_SECTION.";

        catchAssertionHandler.handleMessage( ResultWas::ExplicitFailure,
                                             std::move( message ) );
        catchAssertionHandler.complete();
    }
} // namespace Catch