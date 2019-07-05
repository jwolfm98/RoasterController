#include <gtk/gtk.h>

/* Event buttons and controls */
GtkWindow *window;

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
GtkWidget *gtk_imtmQuit;

GTimer    *roasterEventClock;

GSimpleAction *actQuit;

void   on_tgbStartStop_toggled();
void   on_btnReset_clicked();
void   on_tgbCharge_toggled();
void   on_tgbDryEnd_toggled();
void   on_tgbFirstCrack_toggled();
void   on_tgbDrop_toggled();
void   on_sclBurner_value_changed();
void   on_imtmQuit_activate();
void   on_winRoasterSoftware_destroy();

gchar* on_sclBurner_format_value (GtkScale *scale, gdouble value, gpointer user_data);
void   on_sclBurner_moved  (GtkRange *range, gpointer  user_data);

void   on_application_activate (GtkApplication *app, gpointer userData);
