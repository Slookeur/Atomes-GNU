/* This file is part of Atomes.

Atomes is free software: you can redistribute it and/or modify it under the terms
of the GNU Affero General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

Atomes is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with Atomes.
If not, see <https://www.gnu.org/licenses/> */

#ifndef INTERFACE_H_
#define INTERFACE_H_
G_MODULE_EXPORT void create_about_dialog (GtkWidget * widg, gpointer data);

void show_info (char * information, int val, GtkWidget * win);
void show_info_ (double * valdij);
void show_warning (char * warning, GtkWidget * win);
void show_warning_ (char * warning, char * sub, char * tab);
void show_error (char * error, int val, GtkWidget * win);
void show_error_ (char * error, char * sub, char * tab);
gboolean ask_yes_no (gchar * title, gchar * text, int type, GtkWidget * widg);
gchar * exact_name (gchar * name);
GtkWidget * show_pop (char * pop, GtkWidget * pwin);
#ifdef GTK4
G_MODULE_EXPORT gboolean leaving_question (GtkWindow * widget, gpointer data);
#else
G_MODULE_EXPORT gboolean leaving_question (GtkWidget * widget, GdkEvent * event, gpointer data);
#endif
int dummy_ask_ (char * question);
int iask (char * question, char * lab, int id, GtkWidget * win);
gchar * cask (char * question,  char * lab, int id, char * old, GtkWidget * win);

void print_info  (gchar * str, gchar * stag, GtkTextBuffer * buffer);
gchar * textcolor (int i);

void set_progress_ (gdouble * prog);

gchar * env_name (struct project * this_proj, int g, int s, int f, GtkTextBuffer * buffer);
void init_data_ (int * nats, int * nspc, int * stps, int * cid);
void update_after_calc (int calc);

// In init.c:

void prepostcalc (GtkWidget * widg, gboolean status, int run, int adv, double opc);
void prep_calc_actions ();
void initcwidgets ();
void initbonds ();
void initcnames (int w, int s);
#endif
