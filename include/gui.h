#ifndef GUI_H
#define GUI_H

#include <gtk/gtk.h>

extern GtkWidget *main_window;
extern GtkWidget *statistics_window;
extern GtkWidget *time_entry;

void create_main_window(int argc, char **argv);
void create_statistics_window();
void on_quit_button_clicked(GtkWidget *widget, gpointer data);
void on_see_statistics_button_clicked(GtkWidget *widget, gpointer data);
void on_back_button_clicked(GtkWidget *widget, gpointer data);

#endif // GUI_H

