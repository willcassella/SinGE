#include "lib/engine/components/gameplay/animation.h"
#include "lib/engine/scene.h"
#include "lib/engine/systems/animation_system.h"

namespace sge
{
    void AnimationSystem::register_pipeline(UpdatePipeline& pipeline)
    {
        pipeline.register_system_fn("animation_update", this, &AnimationSystem::animation_update);
        pipeline.register_system_fn("animation_apply", this, &AnimationSystem::animation_apply);
    }

    void AnimationSystem::animation_update(Scene& scene, SystemFrame& frame)
    {
        auto* const anim_comps = scene.get_component_container(CAnimation::type_info);
        const float delta = frame.time_delta();

        // Iterate over all animation components
        NodeId node_ids[32];
        size_t start_index = 0;
        size_t num_instances;
        while (anim_comps->get_instance_nodes(start_index, 32, &num_instances, node_ids))
        {
            start_index += 32;
            CAnimation* instances[32];
            anim_comps->get_instances(node_ids, num_instances, instances);

            for (size_t i = 0; i < num_instances; ++i)
            {
                instances[i]->index(instances[i]->index() + delta);
                if (instances[i]->index() > instances[i]->duration())
                {
                    instances[i]->index(0.f);
                    const auto temp_pos_target = instances[i]->target_position();
                    const auto temp_rot_target = instances[i]->target_rotation();
                    instances[i]->target_position(instances[i]->init_position());
                    instances[i]->init_position(temp_pos_target);
                    instances[i]->target_rotation(instances[i]->init_rotation());
                    instances[i]->init_rotation(temp_rot_target);
                }
            }
        }
    }

    void AnimationSystem::animation_apply(Scene& scene, SystemFrame& /*frame*/)
    {
        auto* const anim_comps = scene.get_component_container(CAnimation::type_info);

        // Iterate over all animation components
        NodeId node_ids[32];
        size_t start_index = 0;
        size_t num_instances;
        while (anim_comps->get_instance_nodes(start_index, 32, &num_instances, node_ids))
        {
            start_index += 32;
            CAnimation* instances[32];
            Node* nodes[32];
            anim_comps->get_instances(node_ids, num_instances, instances);
            scene.get_nodes(node_ids, num_instances, nodes);

            for (size_t i = 0; i < num_instances; ++i)
            {
                const auto v = instances[i]->index() / instances[i]->duration();
                const auto pos = instances[i]->init_position() + (instances[i]->target_position() - instances[i]->init_position()) * v;
                const auto rot = instances[i]->init_rotation() + (instances[i]->target_rotation() - instances[i]->init_rotation()) * v;

                if (instances[i]->animate_position())
                {
                    nodes[i]->set_local_position(pos);
                }
                if (instances[i]->animate_rotation())
                {
                    nodes[i]->set_local_rotation(rot);
                }
            }
        }
    }
}
