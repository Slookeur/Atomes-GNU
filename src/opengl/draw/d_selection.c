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
#include "glview.h"
#include "dlp_field.h"

extern void create_atom_lists (gboolean to_pick);
extern int create_bond_lists (gboolean to_pick);
extern object_3d * draw_sphere (int quality);
extern object_3d * draw_cylinder (int quality, float ra, float rb);
extern object_3d * draw_cylinder_cap (int quality, float rad, gboolean picked);
extern void setup_line_vertice (float * vertices, vec3_t pos, ColRGBA col, float alpha);
extern void setup_sphere_vertice (float * vertices, vec3_t pos, ColRGBA col, float rad, float alpha);
extern void setup_cylinder_vertice (float * vertices, vec3_t pos_a, vec3_t pos_b, ColRGBA col, float rad, float alpha);
extern void setup_triangles (float * vertices, vec3_t sa, vec3_t sb, vec3_t sc);
extern float get_bond_radius (int sty, int ac, int at, int b, int sel);
extern void setup_this_atom (int style, gboolean to_pick, int picked, struct atom * at, int ac, float * vert, float al);
extern void prepare_clone (int style, gboolean to_pick, int picked, struct atom at, struct atom bt, float x, float y, float z, float * vertices);
extern void setup_this_bond (int sty, gboolean to_pick, gboolean picked, int cap, int bi, int pi, struct atom * at, struct atom * bt, float al, float * vertices);

void setup_selected_clone_vertices (int style, int at, int pi, float * vertices)
{
  int i, j;
  gboolean doit;
  struct distance d;
  for (i=0; i < proj_gl -> atoms[step][at].numv; i++)
  {
    j = proj_gl -> atoms[step][at].vois[i];
    d = distance_3d (cell_gl, (cell_gl -> npt) ? step : 0, & proj_gl -> atoms[step][at], & proj_gl -> atoms[step][j]);
    if (d.pbc)
    {
      if (in_movie_encoding && plot -> at_data != NULL)
      {
        doit = (plot -> at_data[at].pick[pi] && plot -> at_data[at].style == style) ? TRUE : FALSE;
      }
      else
      {
        doit = (proj_gl -> atoms[0][at].pick[pi] && proj_gl -> atoms[0][at].style == style) ? TRUE : FALSE;
      }
      if (doit)
      {
        prepare_clone (style, FALSE, pi+1,
                       proj_gl -> atoms[step][at],
                       proj_gl -> atoms[step][j],
                       d.x,
                       d.y,
                       d.z, vertices);
      }
    }
  }
}

int find_selected_clone_vertices (int style, int at)
{
  int i, j, k;
  gboolean doit = FALSE;
  struct distance d;
  if (in_movie_encoding && plot -> at_data != NULL)
  {
    if (plot -> at_data[at].show[1] && plot -> at_data[at].style == style) doit = TRUE;
  }
  else
  {
    if (proj_gl -> atoms[step][at].show[1] && proj_gl -> atoms[step][at].style == style)  doit = TRUE;
  }
  k = 0;
  if (doit)
  {
    for (i=0; i < proj_gl -> atoms[step][at].numv; i++)
    {
      j = proj_gl -> atoms[step][at].vois[i];
      d = distance_3d (cell_gl, (cell_gl -> npt) ? step : 0, & proj_gl -> atoms[step][at], & proj_gl -> atoms[step][j]);
      if (d.pbc)  k ++;
    }
  }
  return k;
}

int find_selected_bond_vertices (int sty, int at, int sp, int bi, int pi, int cap)
{
  int i, j, k, l, m, n;
  struct distance dist;
  gboolean show_a, show_b, show_c, show_d;
  l = 0;
  if (in_movie_encoding && plot -> at_data != NULL)
  {
    show_a = plot -> at_data[at].show[bi];
    show_c = plot -> at_data[at].pick[pi];
    m = plot -> at_data[at].style;
  }
  else
  {
    show_a = proj_gl -> atoms[step][at].show[bi];
    show_c = proj_gl -> atoms[step][at].pick[pi];
    m = proj_gl -> atoms[step][at].style;
  }
  if (show_a && show_c && m == sty)
  {
    for (i=0; i<proj_gl -> atoms[step][at].numv; i++)
    {
      j = proj_gl -> atoms[step][at].vois[i];
      if (in_movie_encoding && plot -> at_data != NULL)
      {
        show_b = plot -> at_data[j].show[bi];
        show_d = plot -> at_data[j].pick[pi];
        n = plot -> at_data[j].style;
        k = proj_gl -> atoms[0][j].sp;
      }
      else
      {
        show_b = proj_gl -> atoms[step][j].show[bi];
        show_d = proj_gl -> atoms[step][j].pick[pi];
        n = proj_gl -> atoms[step][j].style;
        k = proj_gl -> atoms[step][j].sp;
      }
      if (sp == -1 || k == sp)
      {
        dist = distance_3d (cell_gl, (cell_gl -> npt) ? step : 0, & proj_gl -> atoms[step][at], & proj_gl -> atoms[step][j]);
        if ((bi && dist.pbc) ||(! bi && ! dist.pbc))
        {
          if (cap)
          {
            if ((! show_b || ! show_d) || (m != n)) l += 1 + bi;
          }
          else
          {
            l += 1 + bi;
          }
        }
      }
    }
  }
  return 2*l;
}

int get_clone_id (int at, int bt)
{
  int i, j, k;
  for (i=0; i < wingl -> bonds[step][1]; i++)
  {
    j = wingl -> bondid[step][1][i][0];
    k = wingl -> bondid[step][1][i][1];
    if ((j == at && k == bt) || (j == bt && k == at)) return i;
  }
  return -1;
}

void prepare_selected_bond (int sty, gboolean to_pick, gboolean picked, int cap, int bi, int pi, struct atom * at, struct atom * bt, float * vertices)
{
  if (bi == 0)
  {
    setup_this_bond (sty, to_pick, picked, cap, bi, pi, at, bt, 1.0, vertices);
  }
  else
  {
    struct atom * tmp_a, * tmp_b;
    struct distance d = distance_3d (cell_gl, (cell_gl -> npt) ? step : 0, at, bt);

    tmp_a = duplicate_atom (at);
    tmp_b = duplicate_atom (at);
    tmp_a -> pick[pi] = bt -> pick[pi];
    tmp_a -> style = bt -> style;
    tmp_a -> sp = bt -> sp + proj_sp;
    tmp_b -> sp += proj_sp;
    tmp_a -> x -= d.x;
    tmp_a -> y -= d.y;
    tmp_a -> z -= d.z;
    setup_this_bond (sty, to_pick, picked, cap, bi, pi, tmp_b, tmp_a, 0.5, vertices);
    g_free (tmp_a);
    g_free (tmp_b);

    tmp_a = duplicate_atom (bt);
    tmp_b = duplicate_atom (bt);
    tmp_a -> pick[pi] = at -> pick[pi];
    tmp_a -> style = at -> style;
    tmp_a -> sp = at -> sp + proj_sp;
    tmp_b -> sp += proj_sp;
    tmp_a -> id = at -> id;
    tmp_a -> x += d.x;
    tmp_a -> y += d.y;
    tmp_a -> z += d.z;
    setup_this_bond (sty, to_pick, picked, cap, bi, pi, tmp_a, tmp_b, 0.5, vertices);
    g_free (tmp_a);
    g_free (tmp_b);
  }
}

void setup_all_selected_bond_vertices (int sty, int cap, int bi, int at, int sb, int pi, float * vertices)
{
  int i, j, k, l, m;
  struct distance dist;
  gboolean show_a, show_b, show_c, show_d;
  if (in_movie_encoding && plot -> at_data != NULL)
  {
    show_a = plot -> at_data[at].show[bi];
    show_c = plot -> at_data[at].pick[pi];
    l = plot -> at_data[at].style;
  }
  else
  {
    show_a = proj_gl -> atoms[step][at].show[bi];
    show_c = proj_gl -> atoms[step][at].pick[pi];
    l = proj_gl -> atoms[step][at].style;
  }
  if (show_a && show_c && l == sty)
  {
    for (i=0; i<proj_gl -> atoms[step][at].numv; i++)
    {
      j = proj_gl -> atoms[step][at].vois[i];
      if (in_movie_encoding && plot -> at_data != NULL)
      {
        show_b = plot -> at_data[j].show[bi];
        show_d = plot -> at_data[j].pick[pi];
        m = plot -> at_data[j].style;
        k = proj_gl -> atoms[0][j].sp;
      }
      else
      {
        show_b = proj_gl -> atoms[step][j].show[bi];
        show_d = proj_gl -> atoms[step][j].pick[pi];
        m = proj_gl -> atoms[step][j].style;
        k = proj_gl -> atoms[step][j].sp;
      }
      if (sb == -1 || k == sb)
      {
        dist = distance_3d (cell_gl, (cell_gl -> npt) ? step : 0, & proj_gl -> atoms[step][at], & proj_gl -> atoms[step][j]);
        if ((bi && dist.pbc) ||(! bi && ! dist.pbc))
        {
          if (cap)
          {
            if ((! show_b || ! show_d) || (l != m))
            {
              prepare_selected_bond (sty, FALSE, TRUE, cap, bi, pi, & proj_gl -> atoms[step][at], & proj_gl -> atoms[step][j], vertices);
            }
          }
          else
          {
            prepare_selected_bond (sty, FALSE, TRUE, cap, bi, pi, & proj_gl -> atoms[step][at], & proj_gl -> atoms[step][j], vertices);
          }
        }
      }
    }
  }
}

int atoms[NUM_STYLES][2];
int bonds[NUM_STYLES][2], caps[NUM_STYLES][2];
int npbds[NUM_STYLES][2], npcps[NUM_STYLES][2];
int *** nbonds[NUM_STYLES][2];

void prepare_selected (int style, gboolean cylinder, int clone, int type)
{
  int h, i, j;
  struct selatom * sel;
  npbds[style][type] = npcps[style][type] = 0;
  if (cylinder)
  {
    sel = plot -> selected[type] -> first;
    while (sel)
    {
      for (i=0; i<clone; i++)
      {
        npbds[style][type] += find_selected_bond_vertices (style-1, sel -> id, -1, i, type, 0);
        npcps[style][type] += find_selected_bond_vertices (style-1, sel -> id, -1, i, type, 1);
      }
      sel = sel -> next;
    }
    bonds[style][type] = (npbds[style][type]) ? 1 : 0;
    caps[style][type] = (npcps[style][type]) ? 1 : 0;
  }
  else
  {
    sel = plot -> selected[type] -> first;
    while (sel)
    {
      for (i=0; i<clone; i++)
      {
        for (j=0; j<proj_sp; j++)
        {
          nbonds[style][type][i][sel -> sp][j] += find_selected_bond_vertices (style-1, sel -> id, j, i, type, 0);
        }
      }
      sel = sel -> next;
    }
    for (h=0; h<clone; h++)
    {
      for (i=0; i<proj_sp; i++)
      {
        for (j=0; j<proj_sp; j++)
        {
          npbds[style][type] += nbonds[style][type][h][i][j];
          npcps[style][type] += (nbonds[style][type][h][i][j]) ? 1 : 0;
        }
      }
    }
    bonds[style][type] = npcps[style][type];
    caps[style][type] = 0;
  }
}

void prepare_picked (int style, gboolean cylinder, int clone, int type)
{
  int h, i, j;
  npbds[style][type] = npcps[style][type] = 0;
  if (cylinder)
  {
    for (i=0; i<proj_at; i++)
    {
      if (proj_gl -> atoms[0][i].pick[type])
      {
        for (h=0; h<clone; h++)
        {
          npbds[style][type] += find_selected_bond_vertices (style-1, i, -1, h, type, 0);
          npcps[style][type] += find_selected_bond_vertices (style-1, i, -1, h, type, 1);
        }
      }
    }
    bonds[style][type] = (npbds[style][type]) ? 1 : 0;
    caps[style][type] = (npcps[style][type]) ? 1 : 0;
  }
  else
  {
    for (i=0; i<proj_at; i++)
    {
      if (proj_gl -> atoms[0][i].pick[type])
      {
        for (h=0; h<clone; h++)
        {
          for (j=0; j<proj_sp; j++)
          {
            nbonds[style][type][h][proj_gl -> atoms[0][i].sp][j] += find_selected_bond_vertices (style-1, i, j, h, type, 0);
          }
        }
      }
    }
    for (h=0; h<clone; h++)
    {
      for (i=0; i<proj_sp; i++)
      {
        for (j=0; j<proj_sp; j++)
        {
          npbds[style][type] += nbonds[style][type][h][i][j];
          npcps[style][type] += (nbonds[style][type][h][i][j]) ? 1 : 0;
        }
      }
    }
    bonds[style][type] = npcps[style][type];
    caps[style][type] = 0;
  }
}

int render_selected (int style, gboolean cylinder, int caps, int bonds, int ncaps, int type, int clone, int shader)
{
  int h, i, j, k, l;
  struct selatom * sel;
  object_3d * cyl, * cap;
  if (cylinder)
  {
    cyl = draw_cylinder (plot -> quality, 1.0, 1.0);
    cyl -> num_instances =  (bonds/2) * (plot -> extra_cell[0]+1)*(plot -> extra_cell[1]+1)*(plot -> extra_cell[2]+1);
    cyl -> inst_buffer_size = CYLI_BUFF_SIZE;
    cyl -> instances = allocfloat (CYLI_BUFF_SIZE*cyl -> num_instances);
    if (caps)
    {
      cap = draw_cylinder_cap (plot -> quality, 1.0, TRUE);
      cap -> num_instances =  (ncaps/2) * (plot -> extra_cell[0]+1)*(plot -> extra_cell[1]+1)*(plot -> extra_cell[2]+1);
      cap -> inst_buffer_size = CAPS_BUFF_SIZE;
      cap -> instances = allocfloat (CAPS_BUFF_SIZE*cap -> num_instances);
    }
    for (h=0; h<caps+1; h++)
    {
      nbs = 0;
      for (i=0; i<clone; i++)
      {
        sel = plot -> selected[type] -> first;
        for (j=0; j< plot -> selected[type] -> selected; j++)
        {
          for (k=0; k<proj_sp; k++)
          {
            setup_all_selected_bond_vertices (style-1, h, i, sel -> id, k, type, (h == 0) ? cyl -> instances: cap -> instances);
          }
          if (sel -> next != NULL) sel = sel -> next;
        }
      }
    }
    l = 1;
    wingl -> ogl_glsl[SELEC][step][shader] = init_shader_program (SELEC, GLSL_CYLINDERS, cylinder_vertex, NULL, full_color, GL_TRIANGLE_STRIP, 6, 1, cylinder, cyl);
    g_free (cyl);
    if (caps)
    {
      l ++;
      wingl -> ogl_glsl[SELEC][step][shader+1] = init_shader_program (SELEC, GLSL_CAPS, cap_vertex, NULL, full_color, GL_TRIANGLE_FAN, 5, 1, cylinder, cap);
      g_free (cap);
    }
  }
  else
  {
    l = 0;
    for (h=0; h<clone; h++)
    {
      for (i=0; i<proj_sp; i++)
      {
        for (j=0; j<proj_sp; j++)
        {
          if (nbonds[style][type][h][i][j])
          {
            cyl = g_malloc0 (sizeof*cyl);
            cyl -> vert_buffer_size = LINE_BUFF_SIZE;
            cyl -> num_vertices = nbonds[style][type][h][i][j] * (plot -> extra_cell[0]+1)*(plot -> extra_cell[1]+1)*(plot -> extra_cell[2]+1);
            cyl -> vertices = allocfloat (cyl -> vert_buffer_size*cyl -> num_vertices);
            nbs = 0;
            sel = plot -> selected[type] -> first;
            for (k=0; k< plot -> selected[type] -> selected; k++)
            {
              if (sel -> sp == i)
              {
                setup_all_selected_bond_vertices (style-1, 0, h, sel -> id, j, type, cyl -> vertices);
              }
              if (sel -> next != NULL) sel = sel -> next;
            }
            wingl -> ogl_glsl[SELEC][step][shader+l] = init_shader_program (SELEC, GLSL_LINES, line_vertex, NULL, line_color, GL_LINES, 2, 1, cylinder, cyl);
            wingl -> ogl_glsl[SELEC][step][shader+l] -> line_width = get_bond_radius (style, h, i+proj_sp*h, j+proj_sp*h, TRUE);
            g_free (cyl);
            l++;
          }
        }
      }
    }
  }
  return l;
}

int render_picked (int style, gboolean cylinder, int caps, int bonds, int ncaps, int type, int clone, int shader)
{
  int h, i, j, k, l;
  object_3d * cyl, * cap;

  if (cylinder)
  {
    cyl = draw_cylinder (plot -> quality, 1.0, 1.0);
    cyl -> num_instances =  (bonds/2) * (plot -> extra_cell[0]+1)*(plot -> extra_cell[1]+1)*(plot -> extra_cell[2]+1);
    cyl -> inst_buffer_size = CYLI_BUFF_SIZE;
    cyl -> instances = allocfloat (CYLI_BUFF_SIZE*cyl -> num_instances);
    if (caps)
    {
      cap = draw_cylinder_cap (plot -> quality, 1.0, TRUE);
      cap -> num_instances =  (ncaps/2) * (plot -> extra_cell[0]+1)*(plot -> extra_cell[1]+1)*(plot -> extra_cell[2]+1);
      cap -> inst_buffer_size = CAPS_BUFF_SIZE;
      cap -> instances = allocfloat (CAPS_BUFF_SIZE*cap -> num_instances);
    }
    for (h=0; h<caps+1; h++)
    {
      nbs = 0;
      for (i=0; i<clone; i++)
      {
        for (j=0; j<proj_at; j++)
        {
          if (proj_gl -> atoms[0][j].pick[type])
          {
            for (k=0; k<proj_sp; k++)
            {
              setup_all_selected_bond_vertices (style-1, h, i, j, k, type, (h == 0) ? cyl -> instances: cap -> instances);
            }
          }
        }
      }
    }
    l = 1;
    wingl -> ogl_glsl[SELEC][step][shader] = init_shader_program (SELEC, GLSL_CYLINDERS, cylinder_vertex, NULL, full_color, GL_TRIANGLE_STRIP, 6, 1, cylinder, cyl);
    g_free (cyl);
    if (caps)
    {
      l ++;
      wingl -> ogl_glsl[SELEC][step][shader+1] = init_shader_program (SELEC, GLSL_CAPS, cap_vertex, NULL, full_color, GL_TRIANGLE_FAN, 5, 1, cylinder, cap);
      g_free (cap);
    }
  }
  else
  {
    l = 0;
    for (h=0; h<clone; h++)
    {
      for (i=0; i<proj_sp; i++)
      {
        for (j=0; j<proj_sp; j++)
        {
          if (nbonds[style][type][h][i][j])
          {
            cyl = g_malloc0 (sizeof*cyl);
            cyl -> vert_buffer_size = LINE_BUFF_SIZE;
            cyl -> num_vertices = nbonds[style][type][h][i][j] * (plot -> extra_cell[0]+1)*(plot -> extra_cell[1]+1)*(plot -> extra_cell[2]+1);
            cyl -> vertices = allocfloat (cyl -> vert_buffer_size*cyl -> num_vertices);
            nbs = 0;
            for (k=0; k<proj_at; k++)
            {
              if (proj_gl -> atoms[0][k].pick[type] && proj_gl -> atoms[0][k].sp == i)
              {
                setup_all_selected_bond_vertices (style-1, 0, h, k, j, type, cyl -> vertices);
              }
            }
            wingl -> ogl_glsl[SELEC][step][shader+l] = init_shader_program (SELEC, GLSL_LINES, line_vertex, NULL, line_color, GL_LINES, 2, 1, cylinder, cyl);
            wingl -> ogl_glsl[SELEC][step][shader+l] -> line_width = get_bond_radius (style, h, i+proj_sp*h, j+proj_sp*h, TRUE);
            g_free (cyl);
            l++;
          }
        }
      }
    }
  }
  return l;
}

int prepare_selection_shaders (int style, int shaders, int clone, int type, gboolean do_bonds)
{
  int j;
  int nshaders = 0;
  struct selatom * sel;
  gboolean doit;
  gboolean sphere = TRUE;
  gboolean cylinder = FALSE;
  object_3d * atos;

  // Bonds
  if (do_bonds)
  {
    if (bonds[style][type])
    {
      if ((style-1 == NONE && (plot -> style == BALL_AND_STICK || plot -> style == CYLINDERS)) || style-1 == BALL_AND_STICK || style-1 == CYLINDERS) cylinder = TRUE;
      if (plot -> selected[type] -> selected > 0)
      {
        nshaders += render_selected (style, cylinder, caps[style][type], npbds[style][type], npcps[style][type], type, clone, shaders);
      }
      else
      {
        nshaders += render_picked (style, cylinder, caps[style][type], npbds[style][type], npcps[style][type], type, clone, shaders);
      }
      g_free (nbonds[style][type]);
    }
  }
  // Atoms
  if ((style-1 == NONE && (plot -> style == WIREFRAME || plot -> style == PUNT)) || style-1 == WIREFRAME || style-1 == PUNT) sphere = FALSE;

  if (sphere)
  {
    atos = draw_sphere (plot -> quality);
  }
  else
  {
    atos = g_malloc0 (sizeof*atos);
    atos -> vert_buffer_size = 3;
    atos -> num_vertices = 1;
    atos -> vertices = allocfloat (3);
    atos -> vertices[0] = atos -> vertices[1] = atos -> vertices[2] = 0.0;
  }

  atos -> num_instances = atoms[style][type] * (plot -> extra_cell[0]+1)*(plot -> extra_cell[1]+1)*(plot -> extra_cell[2]+1);
  atos -> inst_buffer_size = ATOM_BUFF_SIZE;
  atos -> instances = allocfloat (atos -> num_instances*ATOM_BUFF_SIZE);

  nbl = 0;
  if (plot -> selected[type] -> selected)
  {
    sel = plot -> selected[type] -> first;
    for (j=0; j< plot -> selected[type] -> selected; j++)
    {
      if (in_movie_encoding && plot -> at_data != NULL)
      {
        doit = (plot -> at_data[sel -> id].show[0] && plot -> at_data[sel -> id].style == style-1) ? TRUE : FALSE;
      }
      else
      {
        doit = (proj_gl -> atoms[step][sel -> id].show[0] && proj_gl -> atoms[step][sel -> id].style == style-1) ? TRUE : FALSE;
      }
      if (doit)
      {
        setup_this_atom (style-1, FALSE, type+1, & proj_gl -> atoms[step][sel -> id], 0, atos -> instances, 0.75);
      }
      if (sel -> next != NULL) sel = sel -> next;
    }

    if (plot -> draw_clones)
    {
      // Clones instances
      sel = plot -> selected[type] -> first;
      for (j=0; j< plot -> selected[type] -> selected; j++)
      {
        if (proj_gl -> atoms[step][sel -> id].cloned)
        {
          setup_selected_clone_vertices (style-1, sel -> id, type,  atos -> instances);
        }
        if (sel -> next != NULL) sel = sel -> next;
      }
    }
  }
  else
  {
    for (j=0; j<proj_at; j++)
    {
      if (in_movie_encoding && plot -> at_data != NULL)
      {
        doit = (plot -> at_data[j].pick[type] && plot -> at_data[j].show[0] && plot -> at_data[j].style == style-1) ? TRUE : FALSE;
      }
      else
      {
        doit = (proj_gl -> atoms[step][j].pick[type] && proj_gl -> atoms[step][j].show[0] && proj_gl -> atoms[step][j].style == style-1) ? TRUE : FALSE;
      }
      if (doit)
      {
        setup_this_atom (style-1, FALSE, type+1, & proj_gl -> atoms[step][j], 0, atos -> instances, 0.75);
      }
    }
    if (plot -> draw_clones)
    {
      // Clones instances
      for (j=0; j<proj_at; j++)
      {
        if (proj_gl -> atoms[step][j].cloned) setup_selected_clone_vertices (style-1, j, type, atos -> instances);
      }
    }
  }

  if (sphere)
  {
    wingl -> ogl_glsl[SELEC][step][nshaders+shaders] = init_shader_program (SELEC, GLSL_SPHERES, sphere_vertex, NULL, full_color, GL_TRIANGLE_STRIP, 4, 1, TRUE, atos);
  }
  else
  {
    wingl -> ogl_glsl[SELEC][step][nshaders+shaders] = init_shader_program (SELEC, GLSL_POINTS, point_vertex, NULL, point_color, GL_POINTS, 4, 1, FALSE, atos);
  }
  nshaders ++;
  g_free (atos);
  return nshaders;
}

int check_selection (int style, int type)
{
  struct selatom * sel;
  int j, k;
  k = 0;
  if (plot -> selected[type] -> selected)
  {
    sel = plot -> selected[type] -> first;
    for (j=0; j< plot -> selected[type] -> selected; j++)
    {
      if (in_movie_encoding && plot -> at_data != NULL)
      {
        if (plot -> at_data[sel -> id].show[0] && plot -> at_data[sel -> id].style == style) k ++;
      }
      else
      {
        if (proj_gl -> atoms[step][sel -> id].show[0] && proj_gl -> atoms[step][sel -> id].style == style) k ++;
      }
      if (sel -> next != NULL) sel = sel -> next;
    }

    if (plot -> draw_clones)
    {
      // Clones instances
      sel = plot -> selected[type] -> first;
      for (j=0; j< plot -> selected[type] -> selected; j++)
      {
        k += find_selected_clone_vertices (style, sel -> id);
        if (sel -> next != NULL) sel = sel -> next;
      }
    }
  }
  else
  {
    for (j=0; j<proj_at; j++)
    {
      if (proj_gl -> atoms[0][j].pick[type] && proj_gl -> atoms[0][j].show[0] && proj_gl -> atoms[0][j].style == style) k ++;
    }
    if (plot -> draw_clones)
    {
      // Clones instances
      for (j=0; j<proj_at; j++)
      {
        if (proj_gl -> atoms[0][j].pick[type]) k += find_selected_clone_vertices (style, j);
      }
    }
  }
  return k;
}

int create_selection_lists ()
{
  int h, i, j, k, l;
#ifdef DEBUG
  g_debug ("Selected LIST");
#endif
  gboolean do_bonds;
  gboolean cylinder = FALSE;

  cleaning_shaders (wingl, SELEC);
  wingl -> create_shaders[SELEC] = FALSE;
  i = (plot -> draw_clones) ? 2 : 1;
  j = 2;
  int nshaders = 0;
  for (k=0; k<j; k++)
  {
    if (plot -> selected[k] -> selected > 0 || (! k && wingl -> picked > 0))
    {
      for (h=0; h<NUM_STYLES; h++)
      {
        do_bonds = TRUE;
        cylinder = FALSE;
        if ((! h && (plot -> style == PUNT || plot -> style == SPHERES)) || h-1 == PUNT || h-1 == SPHERES) do_bonds = FALSE;
        atoms[h][k] = check_selection (h-1, k);
        nshaders += (atoms[h][k]) ? 1 : 0;
        if (do_bonds)
        {
          nbonds[h][k] = alloctint (i, proj_sp, proj_sp);
          if ((! h && (plot -> style == BALL_AND_STICK || plot -> style == CYLINDERS)) || h-1 == BALL_AND_STICK || h-1 == CYLINDERS) cylinder = TRUE;
          if (plot -> selected[k] -> selected > 0)
          {
            prepare_selected (h, cylinder, i, k);
          }
          else
          {
            prepare_picked (h, cylinder, i, k);
          }
          nshaders += bonds[h][k] + caps[h][k];
        }
      }
    }
  }
  if (! nshaders) return 0;
  wingl -> ogl_glsl[SELEC][step] = g_malloc0 (nshaders*sizeof*wingl -> ogl_glsl[SELEC][step]);
  h = 0;
  for (k=0; k<j; k++)
  {
    for (l=0; l<NUM_STYLES; l++)
    {
      if (atoms[l][k])
      {
        do_bonds = TRUE;
        if ((! l && (plot -> style == PUNT || plot -> style == SPHERES)) || l-1 == PUNT || l-1 == SPHERES) do_bonds = FALSE;
        if (plot -> selected[k] -> selected > 0 || (! k && wingl -> picked > 0))
        {
          h += prepare_selection_shaders (l, h, i, k, do_bonds);
        }
      }
    }
  }
  return nshaders;
}

int create_pick_lists ()
{
  int i, j, k, l;
#ifdef DEBUG
  g_debug ("Pick LIST");
#endif
  cleaning_shaders (wingl, PICKS);
  wingl -> create_shaders[PICKS] = FALSE;

  i = (plot -> draw_clones) ? 2:1;
  j = 0;
  for (k=0; k<i; k++)
  {
    for (l=0; l<proj_sp; l++)
    {
      j += plot -> show_atom[k][l];
    }
  }
  if (j == 0) return 0;
  k = 0;
  for (i=0; i<proj_at; i++)
  {
    if (in_movie_encoding && plot -> at_data != NULL)
    {
      k += plot -> at_data[i].show[0];
      k += plot -> at_data[i].show[1];
    }
    else
    {
      k += proj_gl -> atoms[step][i].show[0];
      k += proj_gl -> atoms[step][i].show[1];
    }
  }
  if (k == 0) return 0;

  int nshaders = 1;
  gboolean bonds = FALSE;

  i = proj_at;
  if (plot -> draw_clones) i += 2 * wingl -> bonds[step][1];
  j = wingl -> bonds[step][0] + wingl -> bonds[step][1];
  if (plot -> style != SPHERES && plot -> style != PUNT && j > 0)
  {
    bonds = TRUE;
    nshaders ++;
    i += 2 * wingl -> bonds[step][0];
    if (plot -> draw_clones) i += 4 * wingl -> bonds[step][1];
  }
  int tmp_style = plot -> style;
  plot -> style = BALL_AND_STICK;

  if (wingl -> color_to_pick != NULL)
  {
    g_free (wingl -> color_to_pick);
    wingl -> color_to_pick = NULL;
  }
  wingl -> to_be_picked = 0;
  wingl -> color_to_pick = allocint(i);

  wingl -> n_shaders[PICKS][0] = nshaders;
  wingl -> ogl_glsl[PICKS][0] = g_malloc0 (nshaders*sizeof*wingl -> ogl_glsl[PICKS][0]);

  gColorID[0] = gColorID[1] = gColorID[2] = 0;
  create_atom_lists (TRUE);

  wingl -> bonds_to_be_picked = 0;
  if (bonds) create_bond_lists (TRUE);

  plot -> style = tmp_style;

  return nshaders;
}
