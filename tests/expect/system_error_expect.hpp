/*
 * This file has been auto-generated by CppUMockGen v0.6.
 *
 * Contents will NOT be preserved if it is regenerated!!!
 *
 * Generation options: -s c++20
 */

#include <CppUMockGen.hpp>

#include "../../src/system/system_error.hpp"

#include <CppUTestExt/MockSupport.h>

namespace expect { namespace Panic$ {
MockExpectedCall& powerUp();
MockExpectedCall& powerUp(unsigned int __numCalls__);
} }

namespace expect { namespace Panic$ {
MockExpectedCall& throwError(CppUMockGen::Parameter<const Panic::ErrorCode> code, bool __return__);
MockExpectedCall& throwError(unsigned int __numCalls__, CppUMockGen::Parameter<const Panic::ErrorCode> code, bool __return__);
} }

namespace expect { namespace Panic$ {
MockExpectedCall& assertion(CppUMockGen::Parameter<const bool> predicate, CppUMockGen::Parameter<const Panic::ErrorCode> code);
MockExpectedCall& assertion(unsigned int __numCalls__, CppUMockGen::Parameter<const bool> predicate, CppUMockGen::Parameter<const Panic::ErrorCode> code);
} }

namespace expect { namespace Panic$ {
MockExpectedCall& getLastError(Panic::ErrorCode __return__);
MockExpectedCall& getLastError(unsigned int __numCalls__, Panic::ErrorCode __return__);
} }

namespace expect { namespace Panic$ {
MockExpectedCall& resetError();
MockExpectedCall& resetError(unsigned int __numCalls__);
} }

namespace expect { namespace Panic$ {
MockExpectedCall& registerHandler(CppUMockGen::Parameter<const Panic::ErrorCode> code, CppUMockGen::Parameter<Panic::ErrorCallback&> handler);
MockExpectedCall& registerHandler(unsigned int __numCalls__, CppUMockGen::Parameter<const Panic::ErrorCode> code, CppUMockGen::Parameter<Panic::ErrorCallback&> handler);
} }
