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
#include "project.h"
#include "glwindow.h"
#include "glview.h"

extern const gchar * dfi[2];
gboolean cmap_changed = FALSE;
GtkWidget * map_but[3];
GtkTreeStore * map_model;
colormap * the_map;
colormap * tmp_map;
GtkWidget * vcbox;
GtkWidget * vbbox;
GtkWidget * color_fix;
GtkWidget * color_bar;
float * tmp_data;

GtkWidget * create_css_color_bar (colormap * map)
{
  GtkWidget * colob;
  gchar * tmp;

  tmp = g_strdup_printf ("rgba(%d,%d,%d,%d) 0%%",
                           (int)(map -> values[0].red*255.0),
                           (int)(map -> values[0].green*255.0),
                           (int)(map -> values[0].blue*255.0),
                           (int)map -> values[0].alpha);
  int i, j, k;
  j = (map -> positions[0] < map -> cmax) ? 0 : 1;
  for (i=j; i<map -> points; i++)
  {
    k = (int)(100.0*(1.0 - (map -> positions[i] - map -> cmin)/(map -> cmax - map -> cmin)));
    k = (i == map -> points-1 && map -> positions[i] == map -> cmin) ? 100 : k;
    tmp = g_strdup_printf ("%s, rgba(%d,%d,%d,%d) %d%%", tmp,
                           (int)(map -> values[i].red*255.0),
                           (int)(map -> values[i].green*255.0),
                           (int)(map -> values[i].blue*255.0),
                           (int)map -> values[i].alpha,
                           k);
  }
  if (map -> positions[i-1] < map -> cmin)
  {
    tmp = g_strdup_printf ("%s, rgba(%d,%d,%d,%d) %d%%", tmp,
                           (int)(map -> values[i-1].red*255.0),
                           (int)(map -> values[i-1].green*255.0),
                           (int)(map -> values[i-1].blue*255.0),
                           (int)map -> values[i-1].alpha,
                           100);
  }
  gchar * gradient = g_strdup_printf ("image#gradient {\n"
                                      "  background-image: linear-gradient(to bottom, %s);\n"
                                      "  min-width: 50px;\n"
                                      "  min-height: 400px;\n"
                                      "}", tmp);
  g_free (tmp);
  provide_gtk_css (gradient);
  colob = gtk_image_new ();
  gtk_widget_set_name (colob, "gradient");
  gtk_widget_show (colob);
  //gtk_widget_set_size_request (colob, 100, 400);
  g_free (gradient);
  return colob;
}

void init_map_range (colormap * map, int pts)
{
  map -> points = pts;
  if (map -> positions != NULL)
  {
    g_free (map -> positions);
    map -> positions = NULL;
  }
  if (map -> values != NULL)
  {
    g_free (map -> values);
    map -> values = NULL;
  }
  map -> positions = allocfloat (pts);
  map -> values = g_malloc (pts*sizeof*map -> values);
  int i;
  for (i=0;  i<pts; i++)
  {
    map -> positions[i] = map -> cmax - i*(map -> cmax - map -> cmin)/(pts - 1.0);
    map -> values[i].red = 1.0 - i / (pts - 1.0);
    map -> values[i].green = 0.0;
    map -> values[i].blue = i / (pts - 1.0);
    map -> values[i].alpha = 1.0;
  }
}

colormap * allocate_color_map (int pts, struct project * this_proj)
{
  colormap * map = g_malloc0 (sizeof*map);
  map -> data = g_malloc (this_proj -> steps*sizeof*map -> data);
  map -> colors = g_malloc (this_proj -> steps*sizeof*map -> colors);
  int i;
  for (i=0; i<this_proj -> steps; i++)
  {
    map -> data[i] = g_malloc0 (this_proj -> natomes*sizeof*map -> data[i]);
    map -> colors[i] = g_malloc0 (this_proj -> natomes*sizeof*map -> colors[i]);
  }
  return map;
}

gboolean setup_custom_color_map (float * data, struct project * this_proj, gboolean init)
{
  int i, j, k;
  gboolean action;
  float cmin, cmax;
  float delta;

  if (data != NULL)
  {
    k = 0;
    cmin = cmax = data[0];
    for (i=0; i<this_proj -> steps; i++)
    {
      for (j=0; j<this_proj -> natomes; j++)
      {
        cmin = min(cmin, data[k]);
        cmax = max(cmax, data[k]);
        k ++;
      }
    }
  }

  if (init)
  {
    gchar * str = g_strdup_printf ("Range: [min-max] = %f - %f", cmin, cmax);
    action = ask_yes_no ("Use this data set ?", str, GTK_MESSAGE_QUESTION, NULL);
  }
  else
  {
    action = TRUE;
  }
  if (action)
  {
    if (init)
    {
      this_proj -> modelgl -> custom_map = allocate_color_map (2, this_proj);
      this_proj -> modelgl -> custom_map -> points = 2;
    }
    the_map = this_proj -> modelgl -> custom_map;
    if (data != NULL)
    {
      the_map -> cmin = cmin;
      the_map -> cmax = cmax;
      init_map_range (the_map, the_map -> points);
      k = 0;
      for (i=0; i<this_proj -> steps; i++)
      {
        for (j=0; j<this_proj -> natomes; j++)
        {
          the_map -> data[i][j] = data[k];
          k ++;
        }
      }
    }
    gboolean done;
    for (i=0; i<this_proj -> steps; i++)
    {
      for (j=0; j<this_proj -> natomes; j++)
      {
        done = FALSE;
        for (k=0; k<the_map -> points-1; k++)
        {
          if (the_map -> data[i][j] <= the_map -> positions[k] && the_map -> data[i][j] > the_map -> positions[k+1])
          {
            delta = (the_map -> data[i][j]-the_map -> positions[k])/(the_map -> positions[k] - the_map -> positions[k+1]);
            the_map -> colors[i][j].red = the_map -> values[k].red + delta * (the_map -> values[k].red - the_map -> values[k+1].red);
            the_map -> colors[i][j].green = the_map -> values[k].green + delta * (the_map -> values[k].green - the_map -> values[k+1].green);
            the_map -> colors[i][j].blue = the_map -> values[k].blue + delta * (the_map -> values[k].blue - the_map -> values[k+1].blue);
            the_map -> colors[i][j].alpha = 1.0;
            done = TRUE;
          }
        }
        if (! done)
        {
          if (the_map -> data[i][j] > the_map -> positions[0])
          {
            the_map -> colors[i][j].red = the_map -> values[0].red;
            the_map -> colors[i][j].green = the_map -> values[0].green;
            the_map -> colors[i][j].blue = the_map -> values[0].blue;
            the_map -> colors[i][j].alpha = 1.0;
          }
          else if (the_map -> data[i][j] < the_map -> positions[the_map -> points-1])
          {
            the_map -> colors[i][j].red = the_map -> values[the_map -> points-1].red;
            the_map -> colors[i][j].green = the_map -> values[the_map -> points-1].green;
            the_map -> colors[i][j].blue = the_map -> values[the_map -> points-1].blue;
            the_map -> colors[i][j].alpha = 1.0;
          }
        }
      }
    }
  }
  return action;
}

G_MODULE_EXPORT void set_point_position (GtkSpinButton * res, gpointer data)
{
  int pts = GPOINTER_TO_INT (data);
  float pos = (float)  gtk_spin_button_get_value (res);
  gboolean update = FALSE;
  if (pos >= tmp_map -> cmin && pos <= tmp_map -> cmax)
  {
    if (pos != tmp_map -> positions[pts])
    {
      if (pts == 0)
      {
        update = (pos > tmp_map -> positions[1]) ? TRUE : FALSE;
      }
      else if (pts == tmp_map -> points-1)
      {
        update = (pos < tmp_map -> positions[pts-1]) ? TRUE : FALSE;
      }
      else
      {
        update = (pos < tmp_map -> positions[pts-1] && pos > tmp_map -> positions[pts+1]) ? TRUE : FALSE;
      }
      if (update) tmp_map -> positions[pts] = pos;
    }
  }
  gtk_spin_button_set_value (GTK_SPIN_BUTTON(res), tmp_map -> positions[pts]);
  destroy_this_widget (color_bar);
  color_bar = create_css_color_bar (tmp_map);
  gtk_fixed_put (GTK_FIXED(color_fix), color_bar, 50, 0);
}

G_MODULE_EXPORT void set_point_color (GtkColorChooser * colob, gpointer data)
{
  int pts = GPOINTER_TO_INT (data);
  tmp_map -> values[pts] =get_button_color (colob);
  destroy_this_widget (color_bar);
  color_bar = create_css_color_bar (tmp_map);
  gtk_fixed_put (GTK_FIXED(color_fix), color_bar, 50, 0);
}


GtkWidget * create_map_buttons ()
{
  GtkWidget * vbox = create_vbox(5);
  int i;
  GtkWidget * hbox;
  GtkWidget * fixed;
  for (i=0; i<tmp_map -> points; i++)
  {
    hbox = create_hbox (5);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
    fixed = gtk_fixed_new ();
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, fixed, FALSE, FALSE, 0);
    gtk_fixed_put (GTK_FIXED(fixed), color_button(tmp_map -> values[i], FALSE, 100, 25, G_CALLBACK(set_point_color), GINT_TO_POINTER(i)), 0, 0);
    fixed = gtk_fixed_new ();
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, fixed, FALSE, FALSE, 0);
    gtk_fixed_put (GTK_FIXED(fixed), spin_button (G_CALLBACK(set_point_position), tmp_map -> positions[i], tmp_map -> cmin, tmp_map -> cmax, 0.01, 3, 100, GINT_TO_POINTER(i)), 0, 0);
  }
  return vbox;
}

void update_color_map ()
{
  destroy_this_widget (vbbox);
  vbbox = create_map_buttons ();
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vcbox, vbbox, FALSE, FALSE, 0);
  show_the_widgets (vbbox);
  destroy_this_widget (color_bar);
  color_bar = create_css_color_bar (tmp_map);
  gtk_fixed_put (GTK_FIXED(color_fix), color_bar, 50, 0);
}

G_MODULE_EXPORT void add_map_points (GtkSpinButton * res, gpointer data)
{
  //struct project * this_proj = get_project_by_id(GPOINTER_TO_INT(data));
  int id = gtk_spin_button_get_value_as_int (res);
  if (id >= 2 && id != tmp_map -> points)
  {
    init_map_range (tmp_map, id);
    update_color_map ();
  }
  gtk_spin_button_set_value (GTK_SPIN_BUTTON(res), tmp_map -> points);
}

G_MODULE_EXPORT void update_cmin_max (GtkEntry * res, gpointer data)
{
  int i;
  i = GPOINTER_TO_INT(data);
  const gchar * m = entry_get_text (res);
  double v = atof(m);
  gboolean update_cmap = FALSE;
  switch (i)
  {
    case 0:
      if (v > tmp_map -> cmin && v != tmp_map -> cmax)
      {
        tmp_map -> cmax = v;
        update_cmap = TRUE;
      }
      v = tmp_map -> cmax;
      break;
    case 1:
      if (v < tmp_map -> cmax && v != tmp_map -> cmin)
      {
        tmp_map -> cmin = v;
        update_cmap = TRUE;
      }
      v = tmp_map -> cmin;
      break;
  }
  update_entry_double(res, v);
  if (update_cmap)
  {
    //init_map_range (tmp_map, tmp_map -> points);
    update_color_map ();
  }
}

G_MODULE_EXPORT void run_custom_mize_map (GtkDialog * win, gint response_id, gpointer data)
{
  if (response_id == GTK_RESPONSE_APPLY)
  {
    struct project * this_proj = (struct project *)data;
    the_map -> cmax = tmp_map -> cmax;
    the_map -> cmin = tmp_map -> cmin;
    the_map -> points = tmp_map -> points;
    g_free (the_map -> positions);
    the_map -> positions = duplicate_float (the_map -> points, tmp_map -> positions);
    g_free (the_map -> values);
    the_map -> values = duplicate_color (the_map -> points, tmp_map -> values);
    setup_custom_color_map (NULL, this_proj, FALSE);
    int shaders[2] = {ATOMS, BONDS};
    re_create_md_shaders (2, shaders, this_proj);
    update (this_proj -> modelgl);
  }
  destroy_this_dialog (win);
}

G_MODULE_EXPORT void custom_mize_map (GtkWidget * but, gpointer data)
{
  struct project * this_proj = get_project_by_id(GPOINTER_TO_INT(data));
  GtkWidget * win = dialogmodal ("Edit color map", GTK_WINDOW(this_proj -> modelgl -> win));
  gtk_dialog_add_button (GTK_DIALOG(win), "Apply", GTK_RESPONSE_APPLY);
  gtk_widget_set_size_request (win, 300, -1);
  GtkWidget * vbox = dialog_get_content_area (win);
  tmp_map = g_malloc0 (sizeof*tmp_map);
  tmp_map -> cmin = the_map -> cmin;
  tmp_map -> cmax = the_map -> cmax;
  tmp_map -> points = the_map -> points;
  tmp_map -> positions = duplicate_float (the_map -> points, the_map -> positions);
  tmp_map -> values = duplicate_color (tmp_map -> points, the_map -> values);

  // Button for number of colors
  GtkWidget * hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 5);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("Number of color points for the gradient: ", 200, -1, 0.0, 0.5), FALSE, FALSE, 5);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox,
                      spin_button (G_CALLBACK(add_map_points), tmp_map -> points, 2, 100, 1, 0, 150, data),
                      FALSE, FALSE, 25);

  // Max
  hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 5);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("Initial value (overall max): ", 200, -1, 0.0, 0.5), FALSE, FALSE, 5);
  GtkWidget * entry = create_entry (G_CALLBACK(update_cmin_max), 100, 15, FALSE, GINT_TO_POINTER(0));
  update_entry_double (GTK_ENTRY(entry), (double)tmp_map -> cmax);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, entry, FALSE, FALSE, 25);

  // Min
  hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 5);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("Final value (overall min): ", 200, -1, 0.0, 0.5), FALSE, FALSE, 5);
  entry = create_entry (G_CALLBACK(update_cmin_max), 100, 15, FALSE, GINT_TO_POINTER(1));
  update_entry_double (GTK_ENTRY(entry), (double)tmp_map -> cmin);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, entry, FALSE, FALSE, 25);

  hbox = create_hbox(0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 5);
  vcbox = create_vbox (5);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, vcbox, FALSE, FALSE, 5);
  vbbox = create_map_buttons ();
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vcbox, vbbox, FALSE, FALSE, 0);

  color_fix = gtk_fixed_new ();
  color_bar = create_css_color_bar  (tmp_map);
  gtk_fixed_put (GTK_FIXED(color_fix), color_bar, 50, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, color_fix, FALSE, FALSE, 0);

  run_this_gtk_dialog (win, G_CALLBACK(run_custom_mize_map), this_proj);
}

/*void map_set_visible (GtkTreeViewColumn * col,
                      GtkCellRenderer   * renderer,
                      GtkTreeModel      * mod,
                      GtkTreeIter       * iter,
                      gpointer          data)
{
  int i, j;
  j = GPOINTER_TO_INT(data);
  gtk_tree_model_get (mod, iter, 0, & i, -1);
  if (i > 0)
  {
    if (j == 0)
    {
      gtk_cell_renderer_set_visible (renderer, 1);
    }
    else
    {
      gtk_cell_renderer_set_visible (renderer, 0);
    }
  }
  else
  {
    if (j == 0)
    {
      gtk_cell_renderer_set_visible (renderer, 0);
    }
    else
    {
      gtk_cell_renderer_set_visible (renderer, 1);
    }
  }
}*/

void fill_map_model (GtkTreeStore * store, struct project * this_proj)
{
  GtkTreeIter step_level, atom_level;
  int h, i, j;
  if (this_proj -> steps > 1)
  {
    for (h=0; h < this_proj -> steps; h++)
    {
      gtk_tree_store_append (store, & step_level, NULL);
      gtk_tree_store_set (store, & step_level, 0, h+1,
                                               1, 0,
                                               2, 0,
                                               3, 0, -1);
      for (i=0; i < this_proj -> natomes; i++)
      {
        gtk_tree_store_append (store, & atom_level, & step_level);
        j = this_proj -> atoms[h][i].sp;
        gtk_tree_store_set (store, & atom_level, 0, -h-1, 1, this_proj -> chemistry -> label[j] , 2, i+1, 3, the_map -> data[h][i], -1);
      }
    }
  }
  else
  {
    for (i=0; i < this_proj -> natomes; i++)
    {
      gtk_tree_store_append (store, & atom_level, NULL);
      j = this_proj -> atoms[0][i].sp;
      gtk_tree_store_set (store, & atom_level, 0, this_proj -> chemistry -> label[j] , 1, i+1, 2, the_map -> data[0][i], -1);
    }
  }
}

G_MODULE_EXPORT void edit_map_cell (GtkCellRendererText * cell,
                                    gchar * path_string,
                                    gchar * new_text,
                                    gpointer data)
{
  int i, j;
  struct project * this_proj = get_project_by_id(GPOINTER_TO_INT(data));
  GtkTreeIter iter;
  GtkTreePath * path = gtk_tree_path_new_from_string (path_string);
  gtk_tree_model_get_iter (GTK_TREE_MODEL(map_model), & iter, path);
  if (this_proj -> steps > 1)
  {
    gtk_tree_model_get (GTK_TREE_MODEL(map_model), & iter, 0, & i, -1);
    gtk_tree_model_get (GTK_TREE_MODEL(map_model), & iter, 2, & j, -1);
  }
  else
  {
    i = -1;
    gtk_tree_model_get (GTK_TREE_MODEL(map_model), & iter, 1, & j, -1);
  }
  tmp_data[(-i-1)*this_proj -> natomes + j - 1] = atof(new_text);
  if (this_proj -> steps > 1)
  {
    gtk_tree_store_set (map_model, & iter, 3, tmp_data[(-i-1)*this_proj -> natomes + j - 1], -1);
  }
  else
  {
    gtk_tree_store_set (map_model, & iter, 2, tmp_data[(-i-1)*this_proj -> natomes + j - 1], -1);
  }

}

GtkWidget * create_map_tree (struct project * this_proj)
{
  int i, j, k;
  GtkTreeViewColumn * map_col[4];
  GtkCellRenderer * map_cell[4];
  gchar * ctitle[4]={"MD. step", "Element", "Id.", "Value"};
  gchar * ctype[4]={"text", "text", "text", "text"};
  GType col_type[2][4]= {{G_TYPE_STRING, G_TYPE_INT, G_TYPE_FLOAT},
                        {G_TYPE_INT, G_TYPE_STRING, G_TYPE_INT, G_TYPE_FLOAT}};
  j = (this_proj -> steps > 1) ? 1: 0;
  k = (this_proj -> steps > 1) ? 0: 1;
  map_model = gtk_tree_store_newv (3+j, col_type[j]);
  fill_map_model (map_model, this_proj);
  GtkWidget * map_tree = gtk_tree_view_new_with_model (GTK_TREE_MODEL(map_model));
  for (i=0; i<3+j; i++)
  {
    map_cell[i] = gtk_cell_renderer_text_new ();
    map_col[i] = gtk_tree_view_column_new_with_attributes (ctitle[i+k], map_cell[i], ctype[i], i, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(map_tree), map_col[i]);
    gtk_tree_view_column_set_alignment (map_col[i], 0.5);
    //gtk_tree_view_column_set_cell_data_func (map_col[i], map_cell[i], map_set_visible, GINT_TO_POINTER(i), NULL);
    if (i == 2+j)
    {
      g_object_set (map_cell[i], "editable", TRUE, NULL);
      g_signal_connect (G_OBJECT(map_cell[i]), "edited", G_CALLBACK(edit_map_cell), GINT_TO_POINTER(this_proj -> id));
    }
  }
  return map_tree;
}

G_MODULE_EXPORT void run_edit_data_map (GtkDialog * win, gint response_id, gpointer data)
{
  if (response_id == GTK_RESPONSE_APPLY)
  {
    struct project * this_proj = get_project_by_id(GPOINTER_TO_INT(data));
    setup_custom_color_map (tmp_data, this_proj, FALSE);
    int shaders[2] = {ATOMS, BONDS};
    re_create_md_shaders (2, shaders, this_proj);
    update (this_proj -> modelgl);
  }
  g_free (tmp_data);
  destroy_this_dialog (win);
}

G_MODULE_EXPORT void edit_data_map (GtkWidget * but, gpointer data)
{
  struct project * this_proj = get_project_by_id(GPOINTER_TO_INT(data));
  GtkWidget * win = dialogmodal ("Edit color map data", GTK_WINDOW(this_proj -> modelgl -> win));
  gtk_dialog_add_button (GTK_DIALOG(win), "Apply", GTK_RESPONSE_APPLY);
  gtk_widget_set_size_request (win, 250, 600);
  GtkWidget * vbox = dialog_get_content_area (win);
  GtkWidget * maps = create_scroll(NULL, -1, -1, GTK_SHADOW_NONE);
  tmp_data = allocfloat(this_proj -> natomes*this_proj -> steps);
  int i, j, k;
  k = 0;
  for (i=0; i<this_proj -> steps; i++)
  {
    for (j=0; j<this_proj -> natomes; j++)
    {
      tmp_data[k] = the_map -> data[i][j];
      k ++;
    }
  }
  add_container_child (CONTAINER_SCR, maps, create_map_tree (this_proj));
  gtk_widget_set_size_request (maps, -1, 550);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, maps, FALSE, FALSE, 0);
  run_this_gtk_dialog (win, G_CALLBACK(run_edit_data_map), data);
}

gboolean open_save_map (FILE * fp, int act, struct project * this_proj)
{
  int i, j;
  if (act == 0)
  {
    float * tmp_map = allocfloat(this_proj -> natomes*this_proj -> steps);
    rewind (fp);
    for (i=0; i<this_proj -> natomes*this_proj -> steps; i++)
    {
      if (fscanf (fp, "%f", & tmp_map[i]) != 1)
      {
        show_error ("Wrong file format !\n Expecting only a single column file\n"
                    " with Ns x Na lines, with:\n"
                    "  - Ns = number of MD steps\n"
                    "  - Na = number of atoms", 0, this_proj -> modelgl -> win);
        g_free (tmp_map);
        cmap_changed = FALSE;
        return FALSE;
      }
    }
    cmap_changed = setup_custom_color_map (tmp_map, this_proj, TRUE);
    g_free (tmp_map);
    widget_set_sensitive (map_but[1], cmap_changed);
    widget_set_sensitive (map_but[2], cmap_changed);
    return TRUE;
  }
  else
  {
    cmap_changed = FALSE;
    for (i=0; i<this_proj -> steps; i++)
    {
      for (j=0; j<this_proj -> natomes; j++)
      {
        if (fprintf (fp, "%f\n", this_proj -> modelgl -> custom_map -> data[i][j]) < 0)
        {
          show_error ("Error while saving custom color map", 0, this_proj -> modelgl -> win);
          return FALSE;
        }
      }
    }
    return TRUE;
  }
}

dint osmap;

#ifdef GTK4
G_MODULE_EXPORT void run_open_save_data_map (GtkNativeDialog * info, gint response_id, gpointer data)
{
  GtkFileChooser * chooser = GTK_FILE_CHOOSER((GtkFileChooserNative *)info);
#else
G_MODULE_EXPORT void run_open_save_data_map (GtkDialog * info, gint response_id, gpointer data)
{
  GtkFileChooser * chooser = GTK_FILE_CHOOSER((GtkWidget *)info);
#endif
  gboolean status = FALSE;
  gchar * datafile;
  FILE * fp;
  switch (response_id)
  {
    case GTK_RESPONSE_ACCEPT:
      datafile = file_chooser_get_file_name (chooser);
      fp = fopen (datafile, dfi[osmap.b]);
      if (fp != NULL)
      {
        status = open_save_map (fp, osmap.b, get_project_by_id(osmap.a));
      }
      else
      {
        status = FALSE;
      }
      fclose (fp);
      break;
    default:
      status = TRUE;
      break;
  }
  if (status)
  {
#ifdef GTK4
    destroy_this_native_dialog (info);
#else
    destroy_this_dialog (info);
#endif
  }
}

G_MODULE_EXPORT void open_save_data_map (GtkWidget * but, gpointer data)
{
  GtkFileFilter * filter1, * filter2;
  const gchar * res[2] = {"Open", "Save"};
  GtkFileChooserAction act[2] = {GTK_FILE_CHOOSER_ACTION_OPEN, GTK_FILE_CHOOSER_ACTION_SAVE};
  gchar * title[2] = {"Opening custom map for ", "Saving custom map for "};
  osmap.a = GPOINTER_TO_INT (data);
  struct project * this_proj = get_project_by_id(osmap.a);
  gchar * str;
  gboolean open_save = FALSE;
  if (the_map != NULL)
  {
    str = g_strdup_printf ("Do you want to save the color map data");
    open_save = ask_yes_no ("Save color map data to file ?", str, GTK_MESSAGE_QUESTION, this_proj -> modelgl -> win);
    g_free (str);
    if (open_save)
    {
      osmap.b = 1;
    }
    else
    {
      str = g_strdup_printf ("Do you want to read new color map data");
      open_save = ask_yes_no ("Read color map data from file ?", str, GTK_MESSAGE_QUESTION, this_proj -> modelgl -> win);
      g_free (str);
      osmap.b = 0;
    }
  }
  else
  {
    osmap.b = 0;
    open_save = TRUE;
  }
  if (open_save)
  {
#ifdef GTK4
    GtkFileChooserNative * info;
#else
    GtkWidget * info;
#endif
    str = g_strdup_printf ("%s%s", title[osmap.b], prepare_for_title(this_proj -> name));
    info = create_file_chooser (str,
                                GTK_WINDOW(MainWindow),
                                act[osmap.b],
                                res[osmap.b]);
    GtkFileChooser * chooser = GTK_FILE_CHOOSER(info);
    g_free (str);
#ifdef GTK3
    if (osmap.b == 1) gtk_file_chooser_set_do_overwrite_confirmation (chooser, TRUE);
#endif
    file_chooser_set_current_folder (chooser);
    filter1 = gtk_file_filter_new();
    gtk_file_filter_set_name (GTK_FILE_FILTER(filter1), "Raw data (*.dat)");
    gtk_file_filter_add_pattern (GTK_FILE_FILTER(filter1), "*.dat");
    gtk_file_chooser_add_filter (chooser, filter1);
    filter2 = gtk_file_filter_new();
    gtk_file_filter_set_name (GTK_FILE_FILTER(filter2), "All files (*)");
    gtk_file_filter_add_pattern (GTK_FILE_FILTER(filter2), "*");
    gtk_file_chooser_add_filter (chooser, filter2);
#ifdef GTK4
    run_this_gtk_native_dialog ((GtkNativeDialog *)info, G_CALLBACK(run_open_save_data_map), NULL);
#else
    run_this_gtk_dialog (info, G_CALLBACK(run_open_save_data_map), NULL);
#endif
  }
}

gboolean res_use_map;

G_MODULE_EXPORT void run_use_color_map (GtkDialog * win, gint response_id, gpointer data)
{
  if (response_id == GTK_RESPONSE_APPLY)
  {
    res_use_map = (cmap_changed || the_map != NULL) ? TRUE : FALSE;
  }
  else
  {
    res_use_map = the_map != NULL ? TRUE : FALSE;
  }
  destroy_this_dialog (win);
}

gboolean use_custom_color_map (int p)
{
  struct project * this_proj = get_project_by_id(p);
  GtkWidget * win = dialogmodal ("Custom color map settings", GTK_WINDOW(this_proj -> modelgl -> win));
  gtk_dialog_add_button (GTK_DIALOG(win), "Apply", GTK_RESPONSE_APPLY);
  GtkWidget * vbox = dialog_get_content_area (win);
  gchar * btitle[3] = {"Import / Save data", "Edit data", "Customize color map"};
  gchar * bimage[3] = {FOPEN, EDITA, EDITA};
  GCallback handlers[3] = {G_CALLBACK(open_save_data_map), G_CALLBACK(edit_data_map), G_CALLBACK(custom_mize_map)};
  int i;
  for (i=0; i<3; i++)
  {
    map_but[i] = create_button (btitle[i], IMG_STOCK, bimage[i], 150, 50, GTK_RELIEF_NORMAL, handlers[i], GINT_TO_POINTER(p));
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, map_but[i], FALSE, FALSE, 10);
    if (i > 0  && the_map == NULL)
    {
      widget_set_sensitive (map_but[i], 0);
    }
  }
  run_this_gtk_dialog (win, G_CALLBACK(run_use_color_map), NULL);
  return res_use_map;
}

G_MODULE_EXPORT void set_color_map (GtkWidget * widg, gpointer data)
{
  tint * the_data = (tint *) data;
  struct project * this_proj = get_project_by_id(the_data -> a);
  the_map = this_proj -> modelgl -> custom_map;
  int i, j, k, l;
  j = the_data -> b;
  k = j / ATOM_MAPS;
  i = this_proj -> modelgl -> anim -> last -> img -> color_map[k];
  l = j - k*ATOM_MAPS;
#ifdef GTK3
  if (i != l && check_menu_item_get_active ((gpointer)widg))
#else
  if (i != l)
#endif
  {
    gboolean change_map = TRUE;
    gboolean didit = FALSE;
    if (j == 6 && ! the_map && ! k)
    {
      change_map = use_custom_color_map (the_data -> a);
      didit = TRUE;
    }
    else if (j == 6 && k && ! the_map)
    {
      change_map = FALSE;
    }
    if (change_map)
    {
      this_proj -> modelgl -> anim -> last -> img -> color_map[k] = NONE;
#ifdef GTK3
      check_menu_item_set_active ((gpointer)this_proj -> modelgl -> color_styles[k*ATOM_MAPS+i], FALSE);
      if (j == 6) widget_set_sensitive (this_proj -> modelgl -> color_styles[ATOM_MAPS+POLY_MAPS-1], TRUE);
      if (widg != this_proj -> modelgl -> color_styles[j])
      {
        check_menu_item_set_active ((gpointer)this_proj -> modelgl -> color_styles[j], TRUE);
      }
#endif
      this_proj -> modelgl -> anim -> last -> img -> color_map[k] = l;

      if (k == 0)
      {
        int shaders[2] = {ATOMS, BONDS};
        re_create_md_shaders (2, shaders, this_proj);
        this_proj -> modelgl -> create_shaders[LABEL] = TRUE;
      }
      else
      {
        int shaders[1] = {POLYS};
        re_create_md_shaders (1, shaders, this_proj);
      }
      update (this_proj -> modelgl);
      if (j == 6 && ! didit && ! reading_input && ! k) use_custom_color_map (the_data -> a);
    }
  }
#ifdef GTK3
  else if (k*ATOM_MAPS+i == j && ! check_menu_item_get_active ((gpointer)widg))
  {
    check_menu_item_set_active ((gpointer)this_proj -> modelgl -> color_styles[j], TRUE);
  }
#endif
}

#ifdef GTK3
GtkWidget * mapw (glwin * view, int m, int mi, int mid, int mp, gchar * str)
{
  int v = mi*ATOM_MAPS + mid;
  if (m == 0)
  {
    view -> color_styles[v] = gtk3_menu_item (NULL, str, IMG_NONE, NULL, G_CALLBACK(set_color_map), & view -> colorp[v][0], FALSE, 0, 0, TRUE, TRUE, (mp == mid) ? TRUE : FALSE);
    if ((mi && (! view -> bonding || ! view -> custom_map))
     || (mid == 3 && ! view -> adv_bonding[0])
     || (mid == 4 && ! view -> adv_bonding[1])
     || (mid == 5 && ! get_project_by_id(view -> proj) -> force_field[0]))
    {
      widget_set_sensitive (view -> color_styles[v], 0);
    }
    return view -> color_styles[v];
  }
  else
  {
    GtkWidget * ats = gtk3_menu_item (NULL, str, IMG_NONE, NULL, G_CALLBACK(set_color_map), & view -> colorp[v][0], FALSE, 0, 0, TRUE, TRUE, (mp == mid) ? TRUE : FALSE);
    if ((mi && (! view -> bonding || ! view -> custom_map))
     || (mid == 3 && ! view -> adv_bonding[0])
     || (mid == 4 && ! view -> adv_bonding[1])
     || (mid == 5 && ! get_project_by_id(view -> proj) -> force_field[0]))
    {
      widget_set_sensitive (ats, 0);
    }
    return ats;
  }
}

GtkWidget * menump (glwin * view, gchar * ncm, int mid, int id, int cid)
{
  GtkWidget * map = create_menu_item (TRUE, ncm);
  GtkWidget * mapm = gtk_menu_new ();
  menu_item_set_submenu (map, mapm);
  add_menu_child (mapm, mapw(view, mid, id, 0, cid, "Atomic Species"));
  GtkWidget * cp = create_menu_item (FALSE, "Atomic Coordination");
  add_menu_child (mapm, cp);
  GtkWidget * mapn = gtk_menu_new ();
  menu_item_set_submenu (cp, mapn);
  add_menu_child (mapn, mapw(view, mid, id, 1, cid, "Total"));
  add_menu_child (mapn, mapw(view, mid, id, 2, cid, "Partial"));
  add_menu_child (mapm, mapw(view, mid, id, 3, cid, "Fragment(s)"));
  add_menu_child (mapm, mapw(view, mid, id, 4, cid, "Molecule(s)"));
  add_menu_child (mapm, mapw(view, mid, id, 5, cid, "Force Field (DL_POLY)"));
  if (id == 0)
  {
    add_menu_child (mapm, mapw(view, mid, id, 6, cid, "Custom"));
  }
  else
  {
    add_menu_child (mapm, mapw(view, mid, id, 6, cid, "Use Atom(s) Custom Map"));
  }
  return map;
}

GtkWidget * menu_map (glwin * view,  int id)
{
  GtkWidget * menum = gtk_menu_new ();
  add_menu_child (menum, menump(view, "Atoms & bonds", id, 0, view -> anim -> last -> img -> color_map[0]));
  add_menu_child (menum, menump(view, "Polyhedra", id, 1, view -> anim -> last -> img -> color_map[1]));
  widget_set_sensitive (menum, get_project_by_id(view -> proj) -> nspec);
  return menum;
}
#else
G_MODULE_EXPORT void change_color_radio (GSimpleAction * action, GVariant * parameter, gpointer data)
{
  glwin * view = (glwin *)data;
  const gchar * color = g_variant_get_string (parameter, NULL);
  gchar * color_name = NULL;
  int i;
  for (i=0; i<ATOM_MAPS; i++)
  {
    color_name = g_strdup_printf ("set-amap.%d", i);
    if (g_strcmp0(color, (const gchar *)color_name) == 0)
    {
      set_color_map (NULL, & view -> colorp[i][0]);
      g_free (color_name);
      color_name = NULL;
      break;
    }
    g_free (color_name);
    color_name = NULL;
    color_name = g_strdup_printf ("set-pmap.%d", i);
    if (g_strcmp0(color, (const gchar *)color_name) == 0)
    {
      set_color_map (NULL, & view -> colorp[ATOM_MAPS+i][0]);
      g_free (color_name);
      color_name = NULL;
      break;
    }
    g_free (color_name);
    color_name = NULL;
  }
  g_action_change_state (G_ACTION (action), parameter);
}

GMenu * menump (glwin * view, int mid, int cid)
{
  gchar * mapname[2] = {"amap", "pmap"};
  gboolean sensitive;
  GMenu * menu = g_menu_new ();
  append_opengl_item (view, menu, "Atomic Species", mapname[mid], mid*ATOM_MAPS, NULL, IMG_NONE, NULL,
                      FALSE, G_CALLBACK(change_color_radio), (gpointer)view, FALSE, (cid == 0) ? TRUE : FALSE, TRUE, TRUE);
  GMenu * menuf = g_menu_new ();
  append_opengl_item (view, menuf, "Total(s)", mapname[mid], mid*ATOM_MAPS+1, NULL, IMG_NONE, NULL,
                      FALSE, G_CALLBACK(change_color_radio), (gpointer)view, FALSE, (cid == 1) ? TRUE : FALSE, TRUE, TRUE);
  append_opengl_item (view, menuf, "Partial(s)", mapname[mid], mid*ATOM_MAPS+2, NULL, IMG_NONE, NULL,
                      FALSE, G_CALLBACK(change_color_radio), (gpointer)view, FALSE, (cid == 2) ? TRUE : FALSE, TRUE, TRUE);
  g_menu_append_submenu (menu, "Atomic Coordinations", (GMenuModel*)menuf);
  g_object_unref (menuf);

  sensitive = view -> adv_bonding[0];
  append_opengl_item (view, menu, "Fragment(s)", mapname[mid], mid*ATOM_MAPS+3, NULL, IMG_NONE, NULL,
                      FALSE, G_CALLBACK(change_color_radio), (gpointer)view, FALSE, (cid == 3) ? TRUE : FALSE, TRUE, sensitive);
  sensitive = view -> adv_bonding[1];
  append_opengl_item (view, menu, "Molecule(s)", mapname[mid], mid*ATOM_MAPS+4, NULL, IMG_NONE, NULL,
                      FALSE, G_CALLBACK(change_color_radio), (gpointer)view, FALSE, (cid == 4) ? TRUE : FALSE, TRUE, sensitive);
  sensitive = (get_project_by_id(view -> proj) -> force_field[0]) ? TRUE : FALSE;
  append_opengl_item (view, menu, "Force Field (DL_POLY)", mapname[mid], mid*ATOM_MAPS+5, NULL, IMG_NONE, NULL,
                      FALSE, G_CALLBACK(change_color_radio), (gpointer)view, FALSE, (cid == 5) ? TRUE : FALSE, TRUE, sensitive);
  sensitive = (! mid) ? TRUE : (view -> custom_map) ? TRUE : FALSE;
  append_opengl_item (view, menu, (! mid) ? "Custom" : "Use Atom(s) Custom Map", mapname[mid], mid*ATOM_MAPS+6, NULL, IMG_NONE, NULL,
                      FALSE, G_CALLBACK(change_color_radio), (gpointer)view, FALSE, (cid == 6) ? TRUE : FALSE, TRUE, sensitive);
  return menu;
}

GMenu * menu_map (glwin * view)
{
  GMenu * menu = g_menu_new ();
  g_menu_append_submenu (menu, "Atoms & Bonds", (GMenuModel*)menump(view, 0, view -> anim -> last -> img -> color_map[0]));
  g_menu_append_submenu (menu, "Polyhedra", (GMenuModel*)menump(view, 1, view -> anim -> last -> img -> color_map[1]));
  return menu;
}
#endif
