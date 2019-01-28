// FunctionInfo.h
#pragma once

#include <string>
#include "../config.h"

namespace sge
{
    struct TypeInfo;

    using ParamFlags_t = uint8;
    enum : ParamFlags_t
    {
        /**
         * \brief The value of this parameter is read by the called function.
         */
        PARAM_IN = 1 << 0,

        /**
         * \brief The value of this parameter is modified by the called function.
         */
        PARAM_OUT = 1 << 1,

        /**
         * \brief This parameter is not required by the function.
         */
        PARAM_OPT = 1 << 2,

        /**
         * \brief This parameter is expected to be a sequence.
         */
        PARAM_SEQ = 1 << 3
    };

    struct ParamInfo
    {
        std::string name;
        ParamFlags_t flags;
        const TypeInfo* type;
    };

    struct FunctionInfo
    {
        ParamInfo*
    };
}
