/*
TODO:
  This file should probably handle basic application setup, widgets, and the user interface.

  --  A "main" loop for communications to the hardware and the data tracking might need to be a separate thread.

  -- Timer has a race condition that enables double incrementing

*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <math.h>

#include <gtk/gtk.h>

#include <glib.h>
#include <glib-unix.h>

#include <cairo.h>

#include "deviceHandler.h"
#include "chart.h"
#include "roasterSettings.h"
#include "roastEvent.h"
#include "app.h"


GMainLoop *loop;
GSource   *deviceHandlerSource; /* Gsource for the device handler */

GIOChannel *deviceChannel;

char *defaultDevice = "/dev/ttyUSB0";
int   device;
char *message;
int   _testVal;


//TODO:
//
// Define Axis graphics parameters
//
// X Axis is Time
// Y1 Axis is Temperatures
// Y2 Axis is Burner Command

// Define width of the gap between the drawing edge and the Axis line,
// where TEMP_BURN is the gap between the left edge and the start of the TIME line
// and TIME is the gap between the bottom edge and the start of the TEMP and BURNER lines.
#define AXIS_GAP_TIME      100.0
#define AXIS_GAP_TEMP_BURN 100.0

// Define number of tick lines for each Axis
#define AXIS_TICKS_TIME 10
#define AXIS_TICKS_TEMP 10
#define AXIS_TICKS_BURN 10

// Define graphing area parameters

// Percentage margin for vertical axis
#define AXIS_MARGIN (0.02)

// Define length of time axis, where X is window width and y is the window height
#define AXIS_LENGTH_TIME(x)      (x - (AXIS_GAP_TEMP_BURN * 2))
#define AXIS_LENGTH_TEMP_BURN(y) (y - AXIS_GAP_TIME - (AXIS_MARGIN * y))

static long  practiceSeconds = 60 * 13; //15 minutes
static float bTemps[1200], eTemps[1200], bCommand[1200];
static float bMin, bMax;
static long  timeConstraint;
static long  endOfGraphBuffer = 30; //seconds

static long  tempRangeMin = 250, tempRangeMax = 450;
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
    gtk_label_set_label(GTK_LABEL(gtk_lblStatusMessage), g_strdup_printf("%s", message));
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

  /* Determine GtkDrawingArea dimensions */
  gdk_window_get_geometry(window, &da.x, &da.y, &da.width, &da.height);

  chartInit(cr,
            (uint32_t)da.width,
            (uint32_t)da.height,
            8,  /* verticalTicks */
            15,  /* horizontalTicks */
            1,   /* gridLines */
            tempRangeMax, /* leftVerticalMax */
            tempRangeMin, /* leftVerticalMin */
            deltaTempRangeMax,  /* rightVerticalMax */
            deltaTempRangeMin,  /* rightVeritcalMin */
            timeMax); /* horizontalStartingMax */


  /* Draws x and y axis */
  cairo_set_source_rgb (cr, RGB_GREEN);
  for (long i = 0; i < practiceSeconds; i++) {
    if (bTemps[i] != -1) {
      cairo_move_to(cr, scaleTime(i, da.width), scaleTemp(bTemps[i], da.height));
      cairo_line_to(cr, scaleTime(i + 1, da.width), scaleTemp(bTemps[i + 1], da.height));
    }
  }
  cairo_stroke(cr);

  return FALSE;
}

void quitApplication(GSimpleAction *action, GVariant *parameter, gpointer data) {
  GApplication *application = data;
  g_application_quit(application);
  printf("Closing Device\n");
  close(device);
}

gboolean parseNewDataFromDevice(gpointer data) {
  char *message = (char *)data;
  gtk_label_set_label(GTK_LABEL(gtk_lblStatusMessage), g_strdup_printf("Message: %s", message));
  return TRUE;
}

void finishProgramSetup() {
  deviceParserInit(defaultDevice);

  device = open(defaultDevice, O_RDONLY);

  if (device < 0) {
    gtk_label_set_label(GTK_LABEL(gtk_lblStatusMessage), g_strdup_printf("Failed to open device handler."));
  }
  else {
    gtk_label_set_label(GTK_LABEL(gtk_lblStatusMessage), g_strdup_printf("Opened device handler."));
  }

  deviceChannel = g_io_channel_unix_new(device);
  g_io_add_watch(deviceChannel, G_IO_IN, deviceParser, &_testVal);
  g_io_channel_unref(deviceChannel);

}


static void _activate (GtkApplication *app, gpointer userData) {
  GtkBuilder *builder;
  //FILE *settingsFile;

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

  finishProgramSetup();

}

void on_application_activate(GtkApplication *app, gpointer userData) {
  _activate(app, userData);
}

