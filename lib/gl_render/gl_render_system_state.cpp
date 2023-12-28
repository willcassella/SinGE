#include <stdint.h>

#include "lib/engine/util/debug_draw.h"
#include "lib/gl_render/gl_render_system_state.h"

namespace sge
{
    namespace gl_render
    {
        void GLRenderSystem::State::gather_debug_lines(EventChannel& debug_line_channel, EventChannel::SubscriberId subscriber_id)
        {
            // Clear current lines
            frame_debug_lines.clear();

            DebugLine lines[64];
            int32_t num_lines;
            DebugLineVert verts[128];
            while (debug_line_channel.consume(subscriber_id, lines, &num_lines))
            {
                // Fill in lines
                for (int32_t i = 0; i < num_lines; ++i)
                {
                    verts[i * 2].color_rgb = lines[i].color;
                    verts[i * 2].world_position = lines[i].world_start;
                    verts[i * 2 + 1].color_rgb = lines[i].color;
                    verts[i * 2 + 1].world_position = lines[i].world_end;
                }

                // Add them to the buffer
                frame_debug_lines.insert(frame_debug_lines.end(), &verts[0], &verts[0] + num_lines * 2);
            }
        }
    }
}
