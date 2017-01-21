// BasicComponentContainer.h
#pragma once

#include "../Component.h"

namespace sge
{
    template <class ComponentT, typename ComponentDataT>
    class BasicComponentContainer final : public ComponentContainer
    {
        ///////////////////
        ///   Methods   ///
    public:

        void reset() override
        {
            instances = {};
        }

        void to_archive(ArchiveWriter& writer, const std::set<EntityId>& /*est_intsances*/) const override
        {
            for (const auto& instance : instances)
            {
                writer.object_member(sge::to_string(instance.first).c_str(), instance.second);
            }
        }

        void from_archive(ArchiveReader& reader, std::set<EntityId>& est_instances) override
        {
            reader.enumerate_object_members([this, &reader, &est_instances](const char* entityIdStr)
            {
                EntityId entity = std::strtoull(entityIdStr, nullptr, 10);

                // See if this instance already exists
                auto iter = this->instances.find(entity);
                if (iter == this->instances.end())
                {
                    // If not, create one
                    iter = instances.insert(std::make_pair(entity, ComponentDataT{})).first;
                    est_instances.insert(entity);
                }

                sge::from_archive(iter->second, reader);
            });
        }

        void create_component(EntityId entity) override
        {
            instances.insert(std::make_pair(entity, ComponentDataT{}));
        }

        void remove_component(EntityId entity) override
        {
            auto iter = instances.find(entity);
            if (iter != instances.end())
            {
                instances.erase(iter);
            }
        }

        bool create_interface(ProcessingFrame& pframe, EntityId entity, void* addr) override
        {
            auto data = instances.find(entity);
            if (data == instances.end())
            {
                return false;
            }

            new (addr) ComponentT{ pframe, entity, data->second };
            return true;
        }

        //////////////////
        ///   Fields   ///
    public:

        std::unordered_map<EntityId, ComponentDataT> instances;
    };
}
