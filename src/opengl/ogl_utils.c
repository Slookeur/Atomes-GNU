/* This file is part of Atomes.

Atomes is free software: you can redistribute it and/or modify it under the terms
of the GNU Affero General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

Atomes is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with Atomes.
If not, see <https://www.gnu.org/licenses/> */

#include "global.h"
#include "interface.h"
#include "glview.h"
#include "dlp_field.h"

extern gboolean field_color;
extern ColRGBA init_color (int id, int numid);

struct distance distance_2d (struct atom * at, struct atom * bt)
{
  struct distance dist;
  dist.pbc = FALSE;
  dist.x = at -> x - bt -> x;
  dist.y = at -> y - bt -> y;
  dist.z = 0.0;
  dist.length = sqrt(dist.x*dist.x + dist.y*dist.y);
  return dist;
}

struct distance distance_3d (cell_info * cell, int mdstep, struct atom * at, struct atom * bt)
{
  struct distance dist;
  double tmp;
  vec3_t dij;
  dist.pbc = FALSE;
  dist.x = at -> x - bt -> x;
  dist.y = at -> y - bt -> y;
  dist.z = at -> z - bt -> z;
  dist.length = sqrt(dist.x*dist.x + dist.y*dist.y + dist.z*dist.z);
  if (cell -> pbc)
  {
    if (cell -> box[mdstep].param[1][0] == 90.0 && cell -> box[mdstep].param[1][1] == 90.0 && cell -> box[mdstep].param[1][2] == 90.0)
    {
      dij.x = dist.x - round((at -> x-bt -> x)/cell -> box[mdstep].param[0][0]) * cell -> box[mdstep].param[0][0];
      dij.y = dist.y - round((at -> y-bt -> y)/cell -> box[mdstep].param[0][1]) * cell -> box[mdstep].param[0][1];
      dij.z = dist.z - round((at -> z-bt -> z)/cell -> box[mdstep].param[0][2]) * cell -> box[mdstep].param[0][2];
    }
    else
    {
      vec3_t a = vec3(at -> x, at -> y, at -> z);
      vec3_t b = vec3(bt -> x, bt -> y, bt -> z);
      vec3_t af = m4_mul_coord (cell -> box[mdstep].cart_to_frac, a);
      vec3_t bf = m4_mul_coord (cell -> box[mdstep].cart_to_frac, b);
      vec3_t nij = v3_sub(af, bf);
      nij.x -= round(nij.x);
      nij.y -= round(nij.y);
      nij.z -= round(nij.z);
      dij = m4_mul_coord (cell -> box[mdstep].frac_to_cart, nij);
    }
    tmp = v3_length(dij);
    if (dist.length - tmp > 0.001)
    {
      dist.pbc = TRUE;
      dist.x = dij.x;
      dist.y = dij.y;
      dist.z = dij.z;
      dist.length = tmp;
    }
  }
  return dist;
}

double arc_cos (double val)
{
  if (val < -1.0)
  {
    return acos(-2.0 - val) * 180.0 / pi;
  }
  else if (val > 1.0)
  {
    return acos(2.0 - val) * 180.0 / pi;
  }
  else
  {
    return acos(val) * 180.0 / pi;
  }
}

struct angle angle_2d (struct atom * at, struct atom * bt, struct atom * ct)
{
  struct angle theta;
  struct distance dist_a = distance_2d (bt, at);
  struct distance dist_b = distance_2d (bt, ct);
  theta.pbc = FALSE;
  double v = 0.0;
  v = dist_a.x*dist_b.x + dist_a.y*dist_b.y;
  theta.angle = arc_cos(v/(dist_a.length*dist_b.length));
  return theta;
}

struct angle angle_3d (cell_info * cell, int mdstep, struct atom * at, struct atom * bt, struct atom * ct)
{
  struct angle theta;
  struct distance dist_a = distance_3d (cell, mdstep, bt, at);
  struct distance dist_b = distance_3d (cell, mdstep, bt, ct);
  theta.pbc = FALSE;
  if (dist_a.pbc || dist_b.pbc) theta.pbc = TRUE;
  double v = 0.0;
  v = dist_a.x*dist_b.x + dist_a.y*dist_b.y + dist_a.z*dist_b.z;
  theta.angle = arc_cos(v/(dist_a.length*dist_b.length));
  return theta;
}

struct angle dihedral_3d (cell_info * cell, int mdstep, struct atom * at, struct atom * bt, struct atom * ct, struct atom * dt)
{
  struct angle phi;
  struct distance dist_a = distance_3d (cell, mdstep, at, bt);
  struct distance dist_b = distance_3d (cell, mdstep, bt, ct);
  struct distance dist_c = distance_3d (cell, mdstep, ct, dt);
  vec3_t u, v;

  if (dist_a.pbc || dist_b.pbc || dist_c.pbc) phi.pbc = TRUE;

  u = vec3(dist_a.y*dist_b.z  - dist_a.z*dist_b.y, dist_a.z*dist_b.x  - dist_a.x*dist_b.z, dist_a.x*dist_b.y  - dist_a.y*dist_b.x);
  v = vec3(dist_b.y*dist_c.z  - dist_b.z*dist_c.y, dist_b.z*dist_c.x  - dist_b.x*dist_c.z, dist_b.x*dist_c.y  - dist_b.y*dist_c.x);

  if (v3_length(u) == 0.0 || v3_length(v) == 0.0)
  {
    phi.angle = 0.0;
  }
  else
  {
    phi.angle = arc_cos(v3_dot(u, v)/(v3_length(u)*v3_length(v)));
  }
  return phi;
}

struct angle inversion_3d (cell_info * cell, int mdstep, struct atom * at, struct atom * bt, struct atom * ct, struct atom * dt)
{
  struct angle inv;
  struct distance dist_a = distance_3d (cell, mdstep, bt, at);
  struct distance dist_b = distance_3d (cell, mdstep, ct, at);
  struct distance dist_c = distance_3d (cell, mdstep, at, dt);
  vec3_t u, v, w, x;

  if (dist_a.pbc || dist_b.pbc || dist_c.pbc) inv.pbc = TRUE;

  u = vec3(dist_b.x, dist_b.y, dist_b.z);
  v = vec3(dist_c.x, dist_c.y, dist_c.z);
  w = v3_cross (u, v);
  x = vec3(dist_a.x, dist_a.y, dist_a.z);
  if (v3_length(w) == 0.0 || v3_length(x) == 0.0)
  {
    inv.angle = 0.0;
  }
  else
  {
    inv.angle = fabs(90.0 - arc_cos(v3_dot(w, x)/(v3_length(w)*v3_length(x))));
  }
  return inv;
}
