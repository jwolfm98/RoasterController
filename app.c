/*
TODO:
  This file should probably handle basic application setup, widgets, and the user interface.

  --  A "main" loop for communications to the hardware and the data tracking might need to be a separate thread.

  -- Timer has a race condition that enables double incrementing

*/

#include <stdio.h>
#include <gtk/gtk.h>
#include <math.h>
#include <cairo.h>
#include "roasterSettings.h"
#include "roastEvent.h"
#include "app.h"

#define RGB_BLACK   0,   0,   0
#define RGB_RED   255,   0,   0
#define RGB_GREEN   0, 255,   0
#define RGB_BLUE    0,   0, 255
#define RGB_WHITE 255, 255, 255

#define ZOOM_X 1.0
#define ZOOM_Y 1.0

static long  practiceSeconds = 60 * 13; //15 minutes
static float bTemps[1200], eTemps[1200], bCommand[1200];
static float bMin, bMax;
static long  timeConstraint;
static long  endOfGraphBuffer = 30; //seconds

static long  tempRangeMin = 150, tempRangeMax = 500;
static long  deltaTempRangeMin = 0, deltaTempRangeMax = 60;
static long  timeMax = 60 * 15;


RoasterGlobalSettings *rs;
RoasterDataPoints     *rdp;

double burnCommand = 0.0;
static long secExpired = 0;

void setupPracticeGraph() {
  float testVal = 200.0;
  bMin = 0;
  bMax = 10;
  timeConstraint = 4 * 60 + endOfGraphBuffer;
  for (int i = 0; i <= practiceSeconds; i++) {
    if (i < 4) {
      bTemps[i] = -1;
      eTemps[i] = -1;
      bCommand[i] = -1;
    }
    else {
      bTemps[i] = testVal + (i * (float)(300.0/practiceSeconds));
      eTemps[i] = testVal + i + i;
      bCommand[i] = (float)(testVal + (float)(i / 10.0));
    }
  }
}

/* Main Roasting Event loop
 * This loop is responsible for handling I/O to the hardware, graphing the data received
 * and storing this data in memory for saving to file */
void roastEventGraph() {
  while (isRoasting()) {


    /* Finally, handle and GUI updates that need to happen */
    while(gtk_events_pending()) {
      gtk_main_iteration();
    }
  }
}

/* Roaster event timer utility functions */
/* Update the timer per the timeout */
gboolean roasterClockUpdate(gpointer data) {
  int sec, min;
  if (isRoasting() == TRUE) {
    secExpired++;
    if (secExpired > (59 + (59 *60))) {
      secExpired = 0;
    }
    sec = secExpired % 60;
    min = secExpired / 60;
    gtk_label_set_label(GTK_LABEL(gtk_lblTime), g_strdup_printf("%02d:%02d", min, sec));
  }
  return isRoasting();
}
/* Clear the timer by setting the label to 00:00 and setting the global to 0 */
void roasterClockClear() {
  gtk_label_set_label(GTK_LABEL(gtk_lblTime), g_strdup_printf("00:00"));
  secExpired = 0;
}

/* Handle the start/stop button toggle. */
void on_tgbStartStop_toggled() {
  if (isRoasting() == TRUE) {
    //If currently roasting, set start button "back to start" and stop the roasting event
    gtk_button_set_label(GTK_BUTTON(gtk_tgbStartStop), "Start");
    stopRoasting();
  }
  else {
    //if not currently roasting, start thge roasting event and re-label the start button to say "stop"
    gtk_button_set_label(GTK_BUTTON(gtk_tgbStartStop), "Stop");
    g_timeout_add_seconds(1, roasterClockUpdate, NULL);
    startRoasting();
    roastEventGraph();
    /* Timer is not cleared here because the roast event shall remain on screen for the user
     * to save the profile first. Only when the users clicks new or open from the file menu, or the 
     * reset button, should thge timer be reset */
  }
}

/* Handle the roast event reset action */
void on_btnReset_clicked() {
  /* If currently roasting, emulate the user pressing the stop button */
  if (isRoasting() == TRUE) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_tgbStartStop),  FALSE);
  }
  /* Clear the timer for a new roast event */
  roasterClockClear();

  //Reset buttons
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_tgbCharge),     FALSE);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_tgbDryEnd),     FALSE);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_tgbFirstCrack), FALSE);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_tgbDrop),       FALSE);
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

void on_sclBurner_moved (GtkRange *range, gpointer  user_data) {
  burnCommand = gtk_range_get_value(range);
}

gchar* on_sclBurner_format_value (GtkScale *scale, gdouble value, gpointer user_data) {
  return g_strdup_printf("%.1f", value);
}

gfloat f(gfloat x) {
  return (0.03 * pow(x,3));
}

gfloat scaleTemp(long temp, long maxH){
  /* Get percentage of temperature in relation to the desired visible scale, and multiply
   * against the total height */
  return (((float)(maxH / (tempRangeMax - tempRangeMin))) * temp);
}
gfloat scaleTime(long time, long scale) {
  return ((float)(scale / timeMax) * time);
}

gboolean on_draGraph_draw(GtkWidget* widget, cairo_t *cr,  gpointer data) {
  /* Initialize drawing area */
  GdkRectangle da;
  GdkWindow *window = gtk_widget_get_window(widget);
  /* Pixels between each point */
  gdouble dx = 1.0, dy = 1.0;
  gdouble i, clip_x1 = 0.0, clip_y1 = 0.0, clip_x2 = 0.0, clip_y2 = 0.0;

  /* Determine GtkDrawingArea dimensions */
  gdk_window_get_geometry(window, &da.x, &da.y, &da.width, &da.height);

  gtk_label_set_label(GTK_LABEL(gtk_lblStatusMessage), g_strdup_printf("X=%d; Y=%d; Width=%d; Height=%d", da.x, da.y, da.width, da.height));

  //TODO: Scale graphing area to include two columns on the left and right, and a bottom row for the grid lines and text
  //
  //
  //TODO: Color grid label backgrounds to the programs "grey"
  //
  //
  //TODO: Insert grid labels
  //
  //
  //TODO: Color remaining graph area to white
  //
  //
  //TODO: Draw grid lines
  //
  //
  //TODO: Rescale drawing area for grah drawing
  //
  //

  /* Draw on a black background */
  cairo_set_source_rgb(cr, RGB_WHITE);
  cairo_paint(cr);

  /* Change the transformation matrix */
  cairo_translate(cr, 0.0, da.height);
  cairo_scale(cr, ZOOM_X, -ZOOM_Y);

  /* Determine the data points to calculate (ie. those in the clipping zone */
  cairo_device_to_user_distance(cr, &dx, &dy);
  cairo_clip_extents(cr, &clip_x1, &clip_y1, &clip_x2, &clip_y2);
  cairo_set_line_width(cr, 1.0);

  /* Draws x and y axis */
  cairo_set_source_rgb (cr, RGB_GREEN);
  for (long i = 0; i < practiceSeconds; i++) {
    if (bTemps[i] != -1) {
      cairo_move_to(cr, scaleTime(i, da.width), scaleTemp(bTemps[i], da.height));
      cairo_line_to(cr, scaleTime(i + 1, da.width), scaleTemp(bTemps[i + 1], da.height));
    }
  }

  return FALSE;
}

void quitApplication(GSimpleAction *action, GVariant *parameter, gpointer data) {
  GApplication *application = data;
  g_application_quit(application);
}

static void _activate (GtkApplication *app, gpointer userData) {
  GtkBuilder *builder;

  /* Setup action for file menu item 'quit' */
  actQuit = g_simple_action_new("quit", NULL);
  g_signal_connect(actQuit, "activate", G_CALLBACK(quitApplication), app);
  g_action_map_add_action(G_ACTION_MAP(app), G_ACTION(actQuit));

  /* Setup builder for using .glade ui file */
  builder = gtk_builder_new();
  gtk_builder_add_from_file (builder, "RoasterSoftwareGui.glade", NULL);

  /* Add window to application */
  window               = GTK_WINDOW(gtk_builder_get_object(builder, "winRoasterSoftware"));
  gtk_application_add_window(app, GTK_WINDOW(window));

  /* Connect signals and build widgets */
  gtk_builder_connect_signals(builder, NULL);

  gtk_tgbStartStop     = GTK_WIDGET(gtk_builder_get_object(builder, "tgbStartStop"));
  gtk_btnReset         = GTK_WIDGET(gtk_builder_get_object(builder, "btnReset"));
  gtk_tgbCharge        = GTK_WIDGET(gtk_builder_get_object(builder, "tgbCharge"));
  gtk_tgbDryEnd        = GTK_WIDGET(gtk_builder_get_object(builder, "tgbDryEnd"));
  gtk_tgbFirstCrack    = GTK_WIDGET(gtk_builder_get_object(builder, "tgbFirstCrack"));
  gtk_tgbDrop          = GTK_WIDGET(gtk_builder_get_object(builder, "tgbDrop"));
  gtk_sclBurner        = GTK_WIDGET(gtk_builder_get_object(builder, "sclBurner"));
  gtk_lblTime          = GTK_WIDGET(gtk_builder_get_object(builder, "lblTime"));
  gtk_draGraph         = GTK_WIDGET(gtk_builder_get_object(builder, "draGraph"));
  gtk_lblStatusMessage = GTK_WIDGET(gtk_builder_get_object(builder, "lblStatusMessage"));
  gtk_imtmNew          = GTK_WIDGET(gtk_builder_get_object(builder, "imtmNew"));
  gtk_imtmOpen         = GTK_WIDGET(gtk_builder_get_object(builder, "imtmOpen"));
  gtk_imtmSave         = GTK_WIDGET(gtk_builder_get_object(builder, "imtmSave"));
  gtk_imtmSaveAs       = GTK_WIDGET(gtk_builder_get_object(builder, "imtmSaveAs"));
  gtk_imtmQuit         = GTK_WIDGET(gtk_builder_get_object(builder, "imtmQuit"));

  /* free builder */
  g_object_unref(builder);

  /* initialize roaster settings */
  rsNew(&rs);
  setBurnerMinMaxValues(rs, 0.0, 9.0);
  gtk_range_set_range(GTK_RANGE(gtk_sclBurner), 0.0, 9.0);
  gtk_range_set_increments(GTK_RANGE(gtk_sclBurner), 1.0, 1.0);
  gtk_range_set_value(GTK_RANGE(gtk_sclBurner), 4.0);
  stopRoasting();

  /* Show window */
  gtk_widget_show(GTK_WIDGET(window));

  /* Fullscreen window and remove title bar */
  gtk_window_set_decorated(window, FALSE);
  gtk_window_fullscreen(window);

  /* Setup graph drawing signal with initial drawing */
  g_signal_connect(G_OBJECT(gtk_draGraph), "draw", G_CALLBACK(on_draGraph_draw), NULL);

  //TODO: Remove when graph is working.
  setupPracticeGraph();

}

void on_application_activate(GtkApplication *app, gpointer userData) {
  _activate(app, userData);
}

