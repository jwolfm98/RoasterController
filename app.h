#include <gtk/gtk.h>

void activate (GtkApplication *app, gpointer userData);

gchar*
vscale_format (GtkScale *scale, gdouble value, gpointer user_data);

void
vscale_moved (GtkRange *range,
              gpointer  user_data);
