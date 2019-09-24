/*
TODO:
	This file should probably handle basic application setup, widgets, and the user interface.

	A "main" loop for communications to the hardware and the data tracking might need to be
	a separate thread.

*/


#include <gtk/gtk.h>

#include "roasterSettings.h"
#include "app.h"

RoasterGlobalSettings *rs;
double burnCommand = 0.0;

void
vscale_moved (GtkRange *range,
              gpointer  user_data)
{
  GtkWidget *label = user_data;
   
  gdouble pos = gtk_range_get_value (range);
  /* %.1f - stands for a double that will have 1 decimal place */
  gchar *str = g_strdup_printf ("Vertical scale is %.1f", pos);
  gtk_label_set_text (GTK_LABEL (label), str);

  g_free (str);
}

gchar*
vscale_format (GtkScale *scale, gdouble value, gpointer user_data) {
  /* Format value to specified range */
  double actualPressureCommanded = (double)(value / 100.0) * getBurnerRange(rs);
  burnCommand = actualPressureCommanded;
  return g_strdup_printf("%.1f", actualPressureCommanded);  
}

static void _activate (GtkApplication *app, gpointer userData) {
  GtkWidget *window;
  GtkWidget *v_scale;
  GtkWidget *vlabel;
  GtkWidget *grid;
  
  GtkAdjustment *vadjustment;

  /* initialize roaster settings */
  rsNew(rs); 
  setBurnerMinMaxValues(rs, 0.0, 9.0);

  window = gtk_application_window_new(app);
  
  gtk_window_set_title(GTK_WINDOW(window), "Roaster Software");
  gtk_window_set_default_size (GTK_WINDOW (window), 400, 300);
  gtk_container_set_border_width (GTK_CONTAINER (window), 5);
  
  vlabel = gtk_label_new ("Move the scale handle...");

  vadjustment = gtk_adjustment_new (50, 0, 100, 5, 10, 0);
  
  /* Create the Vertical scale. This time, we will see what happens 
   * when the digits aren't initially set.
   */
  v_scale = gtk_scale_new (GTK_ORIENTATION_VERTICAL, vadjustment);
  gtk_widget_set_vexpand (v_scale, TRUE);
  gtk_scale_set_digits(GTK_SCALE(v_scale), 1);

  /* Connecting the "value-changed" signal for the vertical scale to 
   * the appropriate callback function.
   */
  g_signal_connect (v_scale, "value-changed", G_CALLBACK (vscale_moved), vlabel);
  g_signal_connect (v_scale, "format-value", G_CALLBACK (vscale_format), vlabel);

  /* Create a grid and arrange everything accordingly */
  grid = gtk_grid_new ();
  gtk_grid_set_column_spacing (GTK_GRID (grid), 10);
  gtk_grid_set_column_homogeneous (GTK_GRID (grid), TRUE);
  gtk_grid_attach (GTK_GRID (grid), v_scale, 1, 0, 1, 1);
  gtk_grid_attach (GTK_GRID (grid), vlabel, 1, 1, 1, 1);

  gtk_container_add (GTK_CONTAINER (window), grid);

  gtk_widget_show_all(window);
}

/* TODO: Play with placement of this as well as it's requirement for existing */
void activate(GtkApplication *app, gpointer userData) { 
	_activate(app, userData); 
}

