/* This file is part of Atomes.

Atomes is free software: you can redistribute it and/or modify it under the terms
of the GNU Affero General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

Atomes is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with Atomes.
If not, see <https://www.gnu.org/licenses/> */

#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>

#include "global.h"
#include "interface.h"
#include "curve.h"
#include "valid.h"

void save_to_file_ (int *, char *, int *, double *, double *, int *, int *, int *, double *, int *, int *, int *, int *, char *);
void prep_file_ (int *, char *, int *, int *, int *, double *, int *, int *);
void append_to_file_ (int *, double *, double *, double *, int *, int *, int *, int *, int *, int *, int *, char *);

GtkFileFilter * filter1, * filter2;

#ifdef GTK4
G_MODULE_EXPORT void run_write_curve (GtkNativeDialog * info, gint response_id, gpointer data)
{
  GtkFileChooser * chooser = GTK_FILE_CHOOSER((GtkFileChooserNative *)info);
#else
G_MODULE_EXPORT void run_write_curve (GtkDialog * info, gint response_id, gpointer data)
{
  GtkFileChooser * chooser = GTK_FILE_CHOOSER((GtkWidget *)info);
#endif
  int a, b, c, i, j, k, l, m, n, o, p, q;
  tint * cd = (tint *)data;
  a = cd -> a;
  b = cd -> b;
  c = cd -> c;
  if (response_id == GTK_RESPONSE_ACCEPT)
  {
    struct project * this_proj = get_project_by_id(a);
    this_proj -> curves[b][c] -> cfile = file_chooser_get_file_name (chooser);
    GtkFileFilter * tmp = gtk_file_chooser_get_filter (chooser);
    if (tmp == filter1)
    {
      l = 0;
    }
    else if (tmp == filter2)
    {
      l = 1;
    }
    k = strlen(this_proj -> curves[b][c] -> cfile);
    j = strlen (this_proj -> curves[b][c] -> title);
    if (this_proj -> curves[b][c] -> extrac -> extras == 0)
    {
      save_to_file_ (& k, this_proj -> curves[b][c] -> cfile,
                     & this_proj -> curves[b][c] -> ndata,
                     this_proj -> curves[b][c] -> data[0],
                     this_proj -> curves[b][c] -> data[1],
                     & this_proj -> curves[b][c] -> scale[0],
                     & this_proj -> curves[b][c] -> scale[1],
                     & this_proj -> curves[b][c] -> layout -> aspect,
                     & this_proj -> delta[b], & b, & c, & l,
                     & j, this_proj -> curves[b][c] -> title);

    }
    else
    {
      p = this_proj -> curves[b][c] -> extrac -> extras + 1;
      m = 0;
      prep_file_ (& k, this_proj -> curves[b][c] -> cfile, & l,
                  & this_proj -> curves[b][c] -> scale[0],
                  & this_proj -> curves[b][c] -> scale[1],
                  & this_proj -> delta[b], & b, & c);
      append_to_file_ (& this_proj -> curves[b][c] -> ndata,
                       this_proj -> curves[b][c] -> data[0],
                       this_proj -> curves[b][c] -> data[1],
                       & this_proj -> delta[b],
                       & this_proj -> curves[b][c] -> layout -> aspect,
                       & b, & c, & l, & m, & p,
                       & j, this_proj -> curves[b][c] -> title);
      j = this_proj -> curves[b][c] -> extrac -> extras;
      struct cextra * ctmp = this_proj -> curves[b][c] -> extrac -> first;
      for (i=0 ; i < j ; i++ )
      {
        m = m + 1;
        q = ctmp -> id.a;
        o = ctmp -> id.b;
        n = ctmp -> id.c;
        this_proj = get_project_by_id(q);
        j = strlen (this_proj -> curves[o][n] -> title);
        append_to_file_ (& this_proj -> curves[o][n] -> ndata,
                         this_proj -> curves[o][n] -> data[0],
                         this_proj -> curves[o][n] -> data[1],
                         & this_proj -> delta[o],
                         & ctmp -> layout -> aspect,
                         & o, & n, & l, & m, & p,
                         & j, this_proj -> curves[o][n] -> title);
        if (ctmp -> next) ctmp = ctmp -> next;
      }
    }
  }
#ifdef GTK4
  destroy_this_native_dialog (info);
#else
  destroy_this_dialog (info);
#endif
}

G_MODULE_EXPORT void write_curve (GtkWidget * butcurve, gpointer idata)
{
  if (! saving_option ()) goto end;

#ifdef GTK4
  GtkFileChooserNative * info;
#else
  GtkWidget * info;
#endif
  tint * cd = (tint *)idata;
  int a, b, c;
  a = cd -> a;
  b = cd -> b;
  c = cd -> c;
  struct project * this_proj = get_project_by_id(a);
  info = create_file_chooser ("Save Data",
                              GTK_WINDOW(this_proj -> curves[b][c] -> window),
                              GTK_FILE_CHOOSER_ACTION_SAVE,
                              "Save");
  GtkFileChooser * chooser = GTK_FILE_CHOOSER(info);
#ifdef GTK3
  gtk_file_chooser_set_do_overwrite_confirmation (chooser, TRUE);
#endif
  filter1 = gtk_file_filter_new();
  gtk_file_filter_set_name (GTK_FILE_FILTER(filter1), "Xmgrace file (*.agr)");
  gtk_file_filter_add_pattern (GTK_FILE_FILTER(filter1), "*.agr");
  gtk_file_chooser_add_filter (chooser, filter1);
  filter2 = gtk_file_filter_new();
  gtk_file_filter_set_name (GTK_FILE_FILTER(filter2), "x/y ASCII file (*.dat)");
  gtk_file_filter_add_pattern (GTK_FILE_FILTER(filter2), "*.dat");
  gtk_file_chooser_add_filter (chooser, filter2);
  if (this_proj -> curves[b][c] -> cfile != NULL)
  {
    if (! file_chooser_set_file_name (chooser, this_proj -> curves[b][c] -> cfile)) goto end;
  }
  else
  {
    file_chooser_set_current_folder (chooser);
    gtk_file_chooser_set_current_name (chooser, this_proj -> curves[b][c] -> name);
  }
#ifdef GTK4
  run_this_gtk_native_dialog ((GtkNativeDialog *)info, G_CALLBACK(run_write_curve), idata);
#else
  run_this_gtk_dialog (info, G_CALLBACK(run_write_curve), idata);
#endif
  end:;
}
