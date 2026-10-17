#ifndef KN_ENGRAVING_DIAGNOSTICSERRORS_H
#define KN_ENGRAVING_DIAGNOSTICSERRORS_H

#include "global/types/ret.h"

namespace kn::engraving {
enum class Err {
    Undefined       = int(kfw::Ret::Code::Undefined),
    Ok              = int(kfw::Ret::Code::Ok),
    UnknownError    = int(kfw::Ret::Code::DiagnosticsFirst),

    // Draw Data
    DDiff   = 3101
};

inline kfw::Ret make_ret(Err e)
{
    int retCode = static_cast<int>(e);

    switch (e) {
    case Err::Undefined: return kfw::Ret(retCode);
    case Err::Ok: return kfw::Ret(retCode);
    case Err::UnknownError: return kfw::Ret(retCode);
    case Err::DDiff: return kfw::Ret(retCode, "drawdata is different");
    }

    return retCode;
}
}

#endif // KN_ENGRAVING_DIAGNOSTICSERRORS_H
