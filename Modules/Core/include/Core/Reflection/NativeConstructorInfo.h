// NativeConstructorInfo.h
#pragma once

#include <functional>
#include "ConstructorInfo.h"

namespace sge
{
    struct SGE_CORE_API NativeConstructorInfo final : ConstructorInfo
    {
        struct Data
        {
            ////////////////////////
            ///   Constructors   ///
        public:

            Data()
                : constructor(nullptr)
            {
            }

            //////////////////
            ///   Fields   ///
        public:

            std::function<void(void* addr, const ArgAny* args)> constructor;
        };

        ////////////////////////
        ///   Constructors   ///
    public:

        NativeConstructorInfo(ConstructorInfo::Data baseData, Data data)
            : ConstructorInfo(std::move(baseData)), _data(std::move(data))
        {
        }

        ///////////////////
        ///   Methods   ///
    public:

        void invoke(void* addr, const ArgAny* args) const override
        {
            _data.constructor(addr, args);
        }

        //////////////////
        ///   Fields   ///
    public:

        Data _data;
    };
}
