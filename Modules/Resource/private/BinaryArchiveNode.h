// BinaryArchiveNode.h
#pragma once

#include <Core/env.h>

namespace sge
{
    using BinaryArchiveSize_t = uint32;
    using BinaryArchiveBool_t = uint8;
    enum BinaryArchiveNode : byte
    {
        BAN_NULL,
        BAN_BOOLEAN,
        BAN_INT8,
        BAN_UINT8,
        BAN_INT16,
        BAN_UINT16,
        BAN_INT32,
        BAN_UINT32,
        BAN_INT64,
        BAN_UINT64,
        BAN_FLOAT,
        BAN_DOUBLE,
        BAN_STRING,
        BAN_ARRAY_BOOLEAN,
        BAN_ARRAY_INT8,
        BAN_ARRAY_UINT8,
        BAN_ARRAY_INT16,
        BAN_ARRAY_UINT16,
        BAN_ARRAY_INT32,
        BAN_ARRAY_UINT32,
        BAN_ARRAY_INT64,
        BAN_ARRAY_UINT64,
        BAN_ARRAY_FLOAT,
        BAN_ARRAY_DOUBLE,
        BAN_ARRAY_GENERIC,
        BAN_OBJECT,
    };
}
