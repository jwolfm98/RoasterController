#include <gtk/gtk.h>

//TODO: Cleanup includes, pretty sure only app.h is neccessary 
#include "app.h"
#include "roastEvent.h"
#include "roasterSettings.h"
#include "common.h"

//TODO: Move all application related stuff to app.c
//RoasterGlobalSettings *rs;

/* Event buttons and controls */
GtkWidget *gtk_tgbStartStop;
GtkWidget *gtk_btnReset;
GtkWidget *gtk_tgbCharge;
GtkWidget *gtk_tgbDryEnd;
GtkWidget *gtk_tgbFirstCrack;
GtkWidget *gtk_tgbDrop;

GtkWidget *gtk_sclBurner;
GtkWidget *gtk_lblTime;
GtkWidget *gtk_draGraph;
GtkWidget *gtk_lblStatusMessage;

GtkWidget *gtk_imtmNew;
GtkWidget *gtk_imtmOpen;
GtkWidget *gtk_imtmSave;
GtkWidget *gtk_imtmSaveAs;

void on_tgbStartStop_toggled() {
  if (isRoasting()) {
    gtk_button_set_label(GTK_BUTTON(gtk_tgbStartStop), "Start");
    stopRoasting();
  }
  else {
    gtk_button_set_label(GTK_BUTTON(gtk_tgbStartStop), "Stop");
    startRoasting();
  }

}

void on_btnReset_clicked() {
	
}

void on_tgbCharge_toggled() {
	
}

void on_tgbDryEnd_toggled() {
	
}

void on_tgbFirstCrack_toggled() {
	
}

void on_tgbDrop_toggled() {
	
}

void on_sclBurner_value_changed() {
  
}


void on_imtmQuit_activate() {
	gtk_main_quit();
}

void on_winRoasterSoftware_destroy() {
	gtk_main_quit();
}


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

  app = gtk_application_new("org.wolf.roasterSoftare", G_APPLICATION_FLAGS_NONE);

  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  
  status = g_application_run(G_APPLICATION(app), argc, argv);

  g_object_unref(app);

  return status;
}


