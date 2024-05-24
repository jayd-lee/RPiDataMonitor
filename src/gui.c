#include <gtk/gtk.h>
#include "gui.h"

/*
 * TODO: integrate GUI for better user expereince
 */


GtkWidget *main_window;
GtkWidget *statistics_window;
GtkWidget *time_entry;

void create_main_window(int argc, char **argv) {
    // Initialize GTK
    gtk_init(&argc, &argv);

    // Create the main window
    main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(main_window), "Sensor Data");
    gtk_container_set_border_width(GTK_CONTAINER(main_window), 10);
    gtk_window_set_default_size(GTK_WINDOW(main_window), 400, 200);

    // Create a vertical box to hold widgets
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(main_window), vbox);

    // Create a label to display temperature and humidity
    GtkWidget *label = gtk_label_new("Temperature: N/A\nHumidity: N/A");
    gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, TRUE, 0);

    // Create a "See Statistics" button
    GtkWidget *see_statistics_button = gtk_button_new_with_label("See Statistics");
    g_signal_connect(see_statistics_button, "clicked", G_CALLBACK(on_see_statistics_button_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), see_statistics_button, FALSE, FALSE, 0);

    // Create a "Quit" button
    GtkWidget *quit_button = gtk_button_new_with_label("Quit");
    g_signal_connect(quit_button, "clicked", G_CALLBACK(on_quit_button_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), quit_button, FALSE, FALSE, 0);

    // Show all widgets in the main window
    gtk_widget_show_all(main_window);

    // Run the GTK main loop
    gtk_main();
}

void on_quit_button_clicked(GtkWidget *widget, gpointer data) {
    gtk_main_quit();
}

void on_see_statistics_button_clicked(GtkWidget *widget, gpointer data) {
    (void)widget; // Suppress unused parameter warning
    (void)data; // Suppress unused parameter warning
    create_statistics_window();
    gtk_widget_hide(main_window);
}

void on_back_button_clicked(GtkWidget *widget, gpointer data) {
    (void)widget; // Suppress unused parameter warning
    (void)data; // Suppress unused parameter warning
    gtk_widget_hide(statistics_window);
    gtk_widget_show_all(main_window);
}

void create_statistics_window() {
    // Create the statistics window
    statistics_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(statistics_window), "Statistics");
    gtk_container_set_border_width(GTK_CONTAINER(statistics_window), 10);
    gtk_window_set_default_size(GTK_WINDOW(statistics_window), 400, 300);

    // Create a vertical box to hold widgets
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(statistics_window), vbox);

    // Create a label to display statistics
    GtkWidget *label = gtk_label_new("Statistics:\nMin: N/A\nMax: N/A\nAvg: N/A");
    gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, TRUE, 0);

    // Create an entry for time range input
    time_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(time_entry), "Enter time range");
    gtk_box_pack_start(GTK_BOX(vbox), time_entry, FALSE, FALSE, 0);

    // Create a "Back" button
    GtkWidget *back_button = gtk_button_new_with_label("Back");
    g_signal_connect(back_button, "clicked", G_CALLBACK(on_back_button_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), back_button, FALSE, FALSE, 0);

    // Show all widgets in the statistics window
    gtk_widget_show_all(statistics_window);
}

