#include "chart.h"

static void getTickString(char *text, uint8_t tick, uint32_t start, uint32_t step);

const char *temp_string         = "Temp";
const char *tempUnit_string     = "(Deg F)";
const char *time_string         = "Time (Minutes)";
const char *burnerValue_string1 = "Burner";
const char *burnerValue_string2 = "Command";


/* Initializes the chart, left side is temperatures, right side is burner command/value,
 * horizontal is time span */
void chartInit(cairo_t *cr,
               uint32_t width,                 /* Widith of drawing area         */
               uint32_t height,                /* Height of drawing area         */
               uint8_t  verticalTicks,         /* Vertical tick count            */
               uint8_t  horizontalTicks,       /* Horizontal tick count          */
               uint8_t  gridLines,             /* Grid lines on/off              */
               uint32_t leftVerticalMax,       /* Left Vertical max number       */
               uint32_t leftVerticalMin,       /* Left Vertical min number       */
               uint32_t rightVerticalMax,      /* Right Vertical max number      */
               uint32_t rightVerticalMin,      /* Right Veritcal min number      */
               uint32_t horizontalStartingMax) /* Horizontal starting max number */
{
  double scaleX         =  1.0;
  double scaleY         = -1.0;
  double lineWidth      =  1.0;
  double drawX          =  1.0;
  double drawY          =  1.0;
  double clip1          =  0.0;
  double clip2          =  0.0;
  double clip3          =  0.0;
  double clip4          =  0.0;
  uint32_t chartHeight = (height - (CHART_MARGIN_HORIZONTAL * 1.20));
  uint32_t chartWidth  = (width  - (CHART_MARGIN_VERTICAL   * 2));

  cairo_text_extents_t extents;
  double textX;
  double textY;

//Draw on a white background
//TODO: Color grid label backgrounds to the programs "gtk grey"
  cairo_set_source_rgba(cr, RGB_BLACK, GTK_ALPHA);
  cairo_paint(cr);

/* Change the transformation matrix */
  cairo_translate(cr, (double)0.0, height);
  cairo_scale    (cr, scaleX, scaleY);

/* Determine the data points to calculate (ie. those in the clipping zone */
  cairo_device_to_user_distance(cr, &drawX, &drawY);

 //TODO can we assign clip at a later time to ensure the live graph doesn't clip? or is does this need a separate handler?
  cairo_clip_extents(cr, &clip1, &clip2, &clip3, &clip4);

  cairo_set_line_width(cr, lineWidth);
  cairo_rectangle(cr,
                  CHART_MARGIN_VERTICAL,
                  CHART_MARGIN_HORIZONTAL,
                  (width  - (CHART_MARGIN_VERTICAL   * 2)),
                  (height - (CHART_MARGIN_HORIZONTAL * 1.20)));
  cairo_set_source_rgb(cr, RGB_WHITE);
  cairo_fill_preserve(cr);
  cairo_set_source_rgb(cr, RGB_BLACK);
  cairo_stroke(cr);


  //TODO: Insert grid labels
  cairo_translate(cr, (double)0.0, (double)height);
  cairo_scale    (cr, (double)scaleX, (double)scaleY);


  /* Draw Temperature ticks */
  for(int i = 0; i <= verticalTicks; i++) {
    char tickLabel[5];
    getTickString(tickLabel, i, leftVerticalMin, ((leftVerticalMax - leftVerticalMin) / verticalTicks));

    cairo_set_source_rgba(cr, RGB_BLACK, 1.0);
    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, 30.0);
    cairo_text_extents(cr, tickLabel, &extents);
    textX = 50.0-(extents.width/2 + extents.x_bearing);
    textY = (height - (CHART_MARGIN_HORIZONTAL * 1.05)) - ((chartHeight / verticalTicks) * i) - (extents.height/2 + extents.y_bearing);
    cairo_move_to(cr, textX+20, textY+7);
    cairo_show_text(cr, tickLabel);
    cairo_set_line_width(cr, lineWidth * 2);
    cairo_move_to(cr, textX+90, textY-2);
    cairo_line_to(cr, textX+118, textY-2);
    cairo_stroke(cr);
  }
  
  /* Draw Time ticks */
  for(int i = 0; i <= horizontalTicks; i++) {
    char tickLabel[5];
    getTickString(tickLabel, i, leftVerticalMin, ((leftVerticalMax - leftVerticalMin) / verticalTicks));

    cairo_set_source_rgba(cr, RGB_BLACK, 1.0);
    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, 30.0);
    cairo_text_extents(cr, tickLabel, &extents);
    textX = 50.0-(extents.width/2 + extents.x_bearing);
    textY =  (extents.height/2 + extents.y_bearing);
    cairo_move_to(cr, textX+20, textY+7);
    cairo_show_text(cr, tickLabel);
    cairo_set_line_width(cr, lineWidth * 2);
    cairo_move_to(cr, textX+90, textY-2);
    cairo_line_to(cr, textX+118, textY-2);
    cairo_stroke(cr);
  }

  cairo_translate(cr, (double)0.0, (double)height);
  cairo_scale    (cr, (double)scaleX, (double)scaleY);
  
    /* TODO: Draw Vertical Lines */

    /* TODO: Draw Horizontal Lines */

  //
  //TODO: Color remaining graph area to white
  //
  //
  //TODO: Draw grid lines
  //
  //
  //TODO: Rescale drawing area for graph drawing
  //
  //

}

void getTickString(char* ret, uint8_t tick, uint32_t start, uint32_t step) {
  uint32_t value = start + (tick * step);
  sprintf(ret, "%d", value);
}
