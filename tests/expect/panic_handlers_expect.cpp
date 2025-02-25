/*
 * This file has been auto-generated by CppUMockGen v0.6.
 *
 * Contents will NOT be preserved if it is regenerated!!!
 *
 * Generation options: -s c++20
 */

#include "panic_handlers_expect.hpp"

namespace expect { namespace Panic$ { namespace Handlers$ {
MockExpectedCall& DefaultHandler(CppUMockGen::Parameter<const Panic::ErrorCode &> code, bool __return__)
{
    return DefaultHandler(1, code, __return__);
}
MockExpectedCall& DefaultHandler(unsigned int __numCalls__, CppUMockGen::Parameter<const Panic::ErrorCode &> code, bool __return__)
{
    bool __ignoreOtherParams__ = false;
    MockExpectedCall& __expectedCall__ = mock().expectNCalls(__numCalls__, "Panic::Handlers::DefaultHandler");
    if(code.isIgnored()) { __ignoreOtherParams__ = true; } else { __expectedCall__.withConstPointerParameter("code", &code.getValue()); }
    __expectedCall__.andReturnValue(__return__);
    if(__ignoreOtherParams__) { __expectedCall__.ignoreOtherParameters(); }
    return __expectedCall__;
}
} } }

namespace expect { namespace Panic$ { namespace Handlers$ {
MockExpectedCall& NoopHandler(CppUMockGen::Parameter<const Panic::ErrorCode &> code, bool __return__)
{
    return NoopHandler(1, code, __return__);
}
MockExpectedCall& NoopHandler(unsigned int __numCalls__, CppUMockGen::Parameter<const Panic::ErrorCode &> code, bool __return__)
{
    bool __ignoreOtherParams__ = false;
    MockExpectedCall& __expectedCall__ = mock().expectNCalls(__numCalls__, "Panic::Handlers::NoopHandler");
    if(code.isIgnored()) { __ignoreOtherParams__ = true; } else { __expectedCall__.withConstPointerParameter("code", &code.getValue()); }
    __expectedCall__.andReturnValue(__return__);
    if(__ignoreOtherParams__) { __expectedCall__.ignoreOtherParameters(); }
    return __expectedCall__;
}
} } }

namespace expect { namespace Panic$ { namespace Handlers$ {
MockExpectedCall& FailToReadBoardVersion(CppUMockGen::Parameter<const Panic::ErrorCode &> code, bool __return__)
{
    return FailToReadBoardVersion(1, code, __return__);
}
MockExpectedCall& FailToReadBoardVersion(unsigned int __numCalls__, CppUMockGen::Parameter<const Panic::ErrorCode &> code, bool __return__)
{
    bool __ignoreOtherParams__ = false;
    MockExpectedCall& __expectedCall__ = mock().expectNCalls(__numCalls__, "Panic::Handlers::FailToReadBoardVersion");
    if(code.isIgnored()) { __ignoreOtherParams__ = true; } else { __expectedCall__.withConstPointerParameter("code", &code.getValue()); }
    __expectedCall__.andReturnValue(__return__);
    if(__ignoreOtherParams__) { __expectedCall__.ignoreOtherParameters(); }
    return __expectedCall__;
}
} } }

