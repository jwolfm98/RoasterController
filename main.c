#include <gtk/gtk.h>
#include "app.h"

/*TODO: Error handling and recovery from GTK crashing?
	Would require cacheing or storage of current program state, specifically:
			Roaster settings
			Profile data and data points

	Restarting the GTK application and simply regraphing and restoring communication with
	the device can prevent application crashing from ruining a roast. Restart counter to prevent
	application restart looping from allowing manual recovery.
*/

int main(int argc, char *argv[]) {
  GtkApplication *app;
  int status;

  app = gtk_application_new("org.wolf.roasterSoftware", G_APPLICATION_FLAGS_NONE);

  g_signal_connect(app, "activate", G_CALLBACK(on_application_activate), NULL);

  status = g_application_run(G_APPLICATION(app), argc, argv);

  g_object_unref(app);

  return status;
}


