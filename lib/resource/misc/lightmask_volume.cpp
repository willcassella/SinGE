#include <stdint.h>

#include "lib/base/env.h"
#include "lib/resource/misc/lightmask_volume.h"

namespace sge {
void create_lightmask_volume_frustum_elems(uint32_t* out_elems) {
  // Near face
  out_elems[0] = 2;
  out_elems[1] = 8;
  out_elems[2] = 5;
  out_elems[3] = 2;
  out_elems[4] = 11;
  out_elems[5] = 8;

  // Left face
  out_elems[6] = 12;
  out_elems[7] = 3;
  out_elems[8] = 21;
  out_elems[9] = 12;
  out_elems[10] = 0;
  out_elems[11] = 3;

  // Bottom face
  out_elems[12] = 13;
  out_elems[13] = 10;
  out_elems[14] = 1;
  out_elems[15] = 13;
  out_elems[16] = 16;
  out_elems[17] = 10;

  // Right face
  out_elems[18] = 9;
  out_elems[19] = 18;
  out_elems[20] = 6;
  out_elems[21] = 9;
  out_elems[22] = 15;
  out_elems[23] = 18;

  // Top face
  out_elems[24] = 4;
  out_elems[25] = 19;
  out_elems[26] = 22;
  out_elems[27] = 4;
  out_elems[28] = 7;
  out_elems[29] = 19;

  // Far face
  out_elems[30] = 17;
  out_elems[31] = 23;
  out_elems[32] = 20;
  out_elems[33] = 17;
  out_elems[34] = 14;
  out_elems[35] = 23;

  static_assert(NUM_FRUSTUM_ELEMS == 36);
}

void create_lightmask_volume_frustum_normals(
    float horiz_angle_radians,
    float vert_angle_radians,
    Vec3* out_normals
) {
  const float cos_half_horiz_angle = cosf(horiz_angle_radians * 0.5f);
  const float cos_half_vert_angle = cosf(vert_angle_radians * 0.5f);
  const float sin_half_horiz_angle = sinf(horiz_angle_radians * 0.5f);
  const float sin_half_vert_angle = sinf(vert_angle_radians * 0.5f);

  // Near bottom left vertex (occupies indices 0, 1, 2)
  out_normals[0].x(-cos_half_horiz_angle);
  out_normals[0].y(0);
  out_normals[0].z(sin_half_horiz_angle);
  out_normals[1].x(0);
  out_normals[1].y(-cos_half_vert_angle);
  out_normals[1].z(sin_half_vert_angle);
  out_normals[2].x(0);
  out_normals[2].y(0);
  out_normals[2].z(1);

  // Near top left vertex (occupies indices 3, 4, 5)
  out_normals[3].x(-cos_half_horiz_angle);
  out_normals[3].y(0);
  out_normals[3].z(sin_half_horiz_angle);
  out_normals[4].x(0);
  out_normals[4].y(cos_half_vert_angle);
  out_normals[4].z(sin_half_vert_angle);
  out_normals[5].x(0);
  out_normals[5].y(0);
  out_normals[5].z(1);

  // Near top right vetex (occupies indices 6, 7, 8)
  out_normals[6].x(cos_half_horiz_angle);
  out_normals[6].y(0);
  out_normals[6].z(sin_half_horiz_angle);
  out_normals[7].x(0);
  out_normals[7].y(cos_half_vert_angle);
  out_normals[7].z(sin_half_vert_angle);
  out_normals[8].x(0);
  out_normals[8].y(0);
  out_normals[8].z(1);

  // Near bottom right vertex (occupies indices 9, 10, 11)
  out_normals[9].x(cos_half_horiz_angle);
  out_normals[9].y(0);
  out_normals[9].z(sin_half_horiz_angle);
  out_normals[10].x(0);
  out_normals[10].y(-cos_half_vert_angle);
  out_normals[10].z(sin_half_vert_angle);
  out_normals[11].x(0);
  out_normals[11].y(0);
  out_normals[11].z(1);

  // Far bottom left vertex (occupies indices 12, 13, 14)
  out_normals[12].x(-cos_half_horiz_angle);
  out_normals[12].y(0);
  out_normals[12].z(sin_half_horiz_angle);
  out_normals[13].x(0);
  out_normals[13].y(-cos_half_horiz_angle);
  out_normals[13].z(sin_half_vert_angle);
  out_normals[14].x(0);
  out_normals[14].y(0);
  out_normals[14].z(-1);

  // Far bottom right vertex (occupies indices 15, 16, 17)
  out_normals[15].x(cos_half_horiz_angle);
  out_normals[15].y(0);
  out_normals[15].z(sin_half_horiz_angle);
  out_normals[16].x(0);
  out_normals[16].y(-cos_half_vert_angle);
  out_normals[16].z(sin_half_vert_angle);
  out_normals[17].x(0);
  out_normals[17].y(0);
  out_normals[17].z(-1);

  // Far top right vertex (occupies indices 18, 19, 20)
  out_normals[18].x(cos_half_horiz_angle);
  out_normals[18].y(0);
  out_normals[18].z(sin_half_horiz_angle);
  out_normals[19].x(0);
  out_normals[19].y(cos_half_vert_angle);
  out_normals[19].z(sin_half_vert_angle);
  out_normals[20].x(0);
  out_normals[20].y(0);
  out_normals[20].z(-1);

  // Far top left vertex (occupies indices 21, 22, 23)
  out_normals[21].x(-cos_half_horiz_angle);
  out_normals[21].y(0);
  out_normals[21].z(sin_half_horiz_angle);
  out_normals[22].x(0);
  out_normals[22].y(cos_half_vert_angle);
  out_normals[22].z(sin_half_vert_angle);
  out_normals[23].x(0);
  out_normals[23].y(0);
  out_normals[23].z(-1);
}

void create_lightmask_volume_frustum_texcoords(
    float near_plane,
    float far_plane,
    float horiz_angle_radians,
    float vert_angle_radians,
    Vec2* out_texcoords
) {
  const float sin_half_horiz_angle = sinf(horiz_angle_radians * 0.5f);
  const float sin_half_vert_angle = sinf(vert_angle_radians * 0.5f);

  // Near bottom left vertex (occupied indices 0, 1, 2)
  out_texcoords[0].x(near_plane);
  out_texcoords[0].y(near_plane * -sin_half_vert_angle);
  out_texcoords[1].x(near_plane);
  out_texcoords[1].y(near_plane * -sin_half_horiz_angle);
  out_texcoords[2].x(near_plane * -sin_half_horiz_angle);
  out_texcoords[2].y(near_plane * -sin_half_vert_angle);

  // Near top left vertex (occupies indices 3, 4, 5)
  out_texcoords[3].x(near_plane);
  out_texcoords[3].y(near_plane * sin_half_vert_angle);
  out_texcoords[4].x(near_plane);
  out_texcoords[4].y(near_plane * -sin_half_horiz_angle);
  out_texcoords[5].x(near_plane * -sin_half_horiz_angle);
  out_texcoords[5].y(near_plane * sin_half_vert_angle);

  // Near top right vertex (occupies indices 6, 7, 8)
  out_texcoords[6].x(near_plane);
  out_texcoords[6].y(near_plane * sin_half_vert_angle);
  out_texcoords[7].x(near_plane);
  out_texcoords[7].y(near_plane * sin_half_horiz_angle);
  out_texcoords[8].x(near_plane * sin_half_horiz_angle);
  out_texcoords[8].y(near_plane * sin_half_vert_angle);

  // Near bottom right vertex (occupies indices 9, 10, 11)
  out_texcoords[9].x(near_plane);
  out_texcoords[9].y(near_plane * -sin_half_vert_angle);
  out_texcoords[10].x(near_plane);
  out_texcoords[10].y(near_plane * -sin_half_horiz_angle);
  out_texcoords[11].x(near_plane * sin_half_horiz_angle);
  out_texcoords[11].y(near_plane * -sin_half_vert_angle);

  // Far bottom left vertex (occupies indices 12, 13, 14)
  out_texcoords[12].x(far_plane);
  out_texcoords[12].y(far_plane * -sin_half_vert_angle);
  out_texcoords[13].x(far_plane);
  out_texcoords[13].y(far_plane * -sin_half_horiz_angle);
  out_texcoords[14].x(near_plane * sin_half_horiz_angle);
  out_texcoords[14].y(near_plane * -sin_half_vert_angle);

  // Far bottom right vertex (occupies indices 15, 16, 17)
  out_texcoords[15].x(far_plane);
  out_texcoords[15].y(far_plane * -sin_half_vert_angle);
  out_texcoords[16].x(far_plane);
  out_texcoords[16].y(far_plane * -sin_half_horiz_angle);
  out_texcoords[17].x(near_plane * sin_half_horiz_angle);
  out_texcoords[17].y(near_plane * -sin_half_vert_angle);

  // Far top right vertex (occupies indices 18, 19, 20)
  out_texcoords[18].x(far_plane);
  out_texcoords[18].y(far_plane * -sin_half_vert_angle);
  out_texcoords[19].x(far_plane);
  out_texcoords[19].y(far_plane * -sin_half_horiz_angle);
  out_texcoords[20].x(near_plane * sin_half_horiz_angle);
  out_texcoords[20].y(near_plane * -sin_half_vert_angle);

  // Far top left vertex (occupies indices 21, 22, 23)
  out_texcoords[21].x(far_plane);
  out_texcoords[21].y(far_plane * -sin_half_vert_angle);
  out_texcoords[22].x(far_plane);
  out_texcoords[22].y(far_plane * -sin_half_horiz_angle);
  out_texcoords[23].x(near_plane * sin_half_horiz_angle);
  out_texcoords[23].y(near_plane * -sin_half_vert_angle);
}

void create_lightmask_volume_frustum_positions(
    float near_plane,
    float far_plane,
    float horiz_angle_radians,
    float vert_angle_radians,
    Vec3* out_positions
) {
  const float sin_half_horiz_angle = sinf(horiz_angle_radians * 0.5f);
  const float sin_half_vert_angle = sinf(vert_angle_radians * 0.5f);

  // Near bottom left vertex (occupies indices 0, 1, 2)
  out_positions[0].x(near_plane * -sin_half_horiz_angle);
  out_positions[0].y(near_plane * -sin_half_vert_angle);
  out_positions[0].z(-near_plane);
  out_positions[1].x(near_plane * -sin_half_horiz_angle);
  out_positions[1].y(near_plane * -sin_half_vert_angle);
  out_positions[1].z(-near_plane);
  out_positions[2].x(near_plane * -sin_half_horiz_angle);
  out_positions[2].y(near_plane * -sin_half_vert_angle);
  out_positions[2].z(-near_plane);

  // Near top left vertex (occupies indices 3, 4, 5)
  out_positions[3].x(near_plane * -sin_half_horiz_angle);
  out_positions[3].y(near_plane * sin_half_vert_angle);
  out_positions[3].z(-near_plane);
  out_positions[4].x(near_plane * -sin_half_horiz_angle);
  out_positions[4].y(near_plane * sin_half_vert_angle);
  out_positions[4].z(-near_plane);
  out_positions[5].x(near_plane * -sin_half_horiz_angle);
  out_positions[5].y(near_plane * sin_half_vert_angle);
  out_positions[5].z(-near_plane);

  // Near top right vertex (occupies indices 6, 7, 8)
  out_positions[6].x(near_plane * sin_half_horiz_angle);
  out_positions[6].y(near_plane * sin_half_vert_angle);
  out_positions[6].z(-near_plane);
  out_positions[7].x(near_plane * sin_half_horiz_angle);
  out_positions[7].y(near_plane * sin_half_vert_angle);
  out_positions[7].z(-near_plane);
  out_positions[8].x(near_plane * sin_half_horiz_angle);
  out_positions[8].y(near_plane * sin_half_vert_angle);
  out_positions[8].z(-near_plane);

  // Near bottom right vertex (occupies indices 9, 10, 11)
  out_positions[9].x(near_plane * sin_half_horiz_angle);
  out_positions[9].y(near_plane * -sin_half_vert_angle);
  out_positions[9].z(-near_plane);
  out_positions[10].x(near_plane * sin_half_horiz_angle);
  out_positions[10].y(near_plane * -sin_half_vert_angle);
  out_positions[10].z(-near_plane);
  out_positions[11].x(near_plane * sin_half_horiz_angle);
  out_positions[11].y(near_plane * -sin_half_vert_angle);
  out_positions[11].z(-near_plane);

  // Far bottom left vertex (occupies indices 12, 13, 14)
  out_positions[12].x(far_plane * -sin_half_horiz_angle);
  out_positions[12].y(far_plane * -sin_half_vert_angle);
  out_positions[12].z(-far_plane);
  out_positions[13].x(far_plane * -sin_half_horiz_angle);
  out_positions[13].y(far_plane * -sin_half_vert_angle);
  out_positions[13].z(-far_plane);
  out_positions[14].x(far_plane * -sin_half_horiz_angle);
  out_positions[14].y(far_plane * -sin_half_vert_angle);
  out_positions[14].z(-far_plane);

  // Far bottom right vertex (occupies indices 15, 16, 17)
  out_positions[15].x(far_plane * sin_half_horiz_angle);
  out_positions[15].y(far_plane * -sin_half_vert_angle);
  out_positions[15].z(-far_plane);
  out_positions[16].x(far_plane * sin_half_horiz_angle);
  out_positions[16].y(far_plane * -sin_half_vert_angle);
  out_positions[16].z(-far_plane);
  out_positions[17].x(far_plane * sin_half_horiz_angle);
  out_positions[17].y(far_plane * -sin_half_vert_angle);
  out_positions[17].z(-far_plane);

  // Far top right vertex (occupies indices 18, 19, 20)
  out_positions[18].x(far_plane * sin_half_horiz_angle);
  out_positions[18].y(far_plane * sin_half_vert_angle);
  out_positions[18].z(-far_plane);
  out_positions[19].x(far_plane * sin_half_horiz_angle);
  out_positions[19].y(far_plane * sin_half_vert_angle);
  out_positions[19].z(-far_plane);
  out_positions[20].x(far_plane * sin_half_horiz_angle);
  out_positions[20].y(far_plane * sin_half_vert_angle);
  out_positions[20].z(-far_plane);

  // Far top left vertex (occupies indices 21, 22, 23)
  out_positions[21].x(far_plane * -sin_half_horiz_angle);
  out_positions[21].y(far_plane * sin_half_vert_angle);
  out_positions[21].z(-far_plane);
  out_positions[22].x(far_plane * -sin_half_horiz_angle);
  out_positions[22].y(far_plane * sin_half_vert_angle);
  out_positions[22].z(-far_plane);
  out_positions[23].x(far_plane * -sin_half_horiz_angle);
  out_positions[23].y(far_plane * sin_half_vert_angle);
  out_positions[23].z(-far_plane);
}
}  // namespace sge
