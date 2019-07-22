#include "chart.h"

static uint32_t chartOriginX;   /* pixel location of chart origin X  */
static uint32_t chartOriginY;   /* pixel location of chart origin Y  */
static uint32_t chartEndPointX; /* pixel location of chart end point */
static uint32_t chartEndPointY; /* pixel location of chart end point */
static uint32_t daWidth;
static uint32_t daHeight;
static uint32_t daLeftVColWidth;
static uint32_t daLeftVColHeight;
static uint32_t daLeftVColEndPointX;
static uint32_t daLeftVColEndPointY;
static uint32_t daRightVColWidth;
static uint32_t daRightVColHeight;
static uint32_t daRightVColOriginX;
static uint32_t daRightVColOriginY;


void leftVerticalLadder(cairo_t *cr, uint32_t minValue, uint32_t maxValue, uint32_t tickCount);
void rightVerticalLadder(cairo_t *cr, uint32_t minValue, uint32_t maxValue, uint32_t tickCount);
void horizontalLadder(cairo_t *cr, uint32_t minValue, uint32_t maxValue, uint32_t tickCount);
void getTickString(char *text, uint8_t tick, uint32_t start, uint32_t step);

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
  double   scaleX      =  1.0;
  double   scaleY      = -1.0;
  double   lineWidth   =  1.0;
  double   drawX       =  1.0;
  double   drawY       =  1.0;
  double   clip1       =  0.0;
  double   clip2       =  0.0;
  double   clip3       =  0.0;
  double   clip4       =  0.0;

  double textX;
  double textY;
  cairo_text_extents_t extents;

  /* Setup definitions for chart drawing area */
  daWidth  = width;
  daHeight = height;




  /* Paint the entire drawing area */
  cairo_set_source_rgba(cr, RGB_BLACK, GTK_ALPHA);
  cairo_paint(cr);

  /* Change the transformation matrix so that 0,0 is the bottom left hand corner */
  cairo_translate(cr, (double)0.0, (double)height);
  cairo_scale    (cr, scaleX, scaleY);

  /* Determine the data points to calculate (ie. those in the clipping zone */
  cairo_device_to_user_distance(cr, &drawX, &drawY);
  cairo_clip_extents           (cr, &clip1, &clip2, &clip3, &clip4);





  /*** Setup charting area ***/

  /* Origin point starts where the left vertical colum ends */
  chartOriginX   = (uint32_t)floor((double)(CHART_MARGIN_WIDTH         * width));
  chartOriginY   = (uint32_t)floor((double)(CHART_MARGIN_HEIGHT_BOTTOM * height));
  /* Chart end points are relvative to origin points, not absolute to the drawing area */
  chartEndPointX = (uint32_t)floor(width  - (double)(CHART_MARGIN_WIDTH  * width * 2));
  chartEndPointY = (uint32_t)floor(height - (double)(CHART_MARGIN_HEIGHT_BOTTOM * height +
                                                     CHART_MARGIN_HEIGHT_TOP    * height));

  /* Left Vertical Column Ladder Parameters */
  daLeftVColEndPointX = chartOriginX;
  daLeftVColEndPointY = chartOriginY;
  daLeftVColWidth     = daLeftVColEndPointX;
  daLeftVColHeight    = (uint32_t)(floor((double)(CHART_MARGIN_HEIGHT_BOTTOM * height)) -
                                 floor((double)(CHART_MARGIN_HEIGHT_TOP * height)));

  /* Right Vertical Column Ladder Parameters */
  daRightVColOriginX = (uint32_t)floor(width  - (double)(CHART_MARGIN_WIDTH  * width));
  daRightVColOriginY = (uint32_t)floor(height - (double)(CHART_MARGIN_HEIGHT_BOTTOM * height));
  daRightVColWidth   = daWidth - daRightVColOriginX;
  daRightVColHeight  = daLeftVColHeight;


  /* Setup graphing area inside the drawing area */
  cairo_set_line_width(cr, lineWidth);
  cairo_rectangle     (cr, chartOriginX, chartOriginY, chartEndPointX, chartEndPointY);
  cairo_set_source_rgb(cr, RGB_WHITE);
  cairo_fill_preserve (cr);
  cairo_set_source_rgb(cr, RGB_BLACK);
  cairo_stroke        (cr);

  /* Invert translation to make drawing easier */
  cairo_translate(cr, (double)0.0,    (double)height);
  cairo_scale    (cr, scaleX, scaleY);

  leftVerticalLadder (cr, leftVerticalMin, leftVerticalMax, verticalTicks);
  rightVerticalLadder(cr, rightVerticalMin, rightVerticalMax, verticalTicks);
  horizontalLadder   (cr, 0, horizontalStartingMax, horizontalStartingMax);

  cairo_translate(cr, (double)0.0, (double)height);
  cairo_scale    (cr, scaleX, scaleY);

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

//TODO: Text Offset should be based on width of vertical column
//TODO: Tick line length should be based on a percentage of the vertical column width size
void leftVerticalLadder(cairo_t *cr, uint32_t minValue, uint32_t maxValue, uint32_t tickCount) {
  cairo_text_extents_t extents;
  double textX;
  double textY;

  /* Draw Temperature ticks */
  for(int i = 0; i <= tickCount; i++) {
    char tickLabel[5];
    getTickString(tickLabel, i, minValue, ((maxValue - minValue) / tickCount));

    /* Set style */
    cairo_set_source_rgba (cr, RGB_BLACK, 1.0);
    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size   (cr, 30.0);

    /* Set Extents */
    cairo_text_extents(cr, tickLabel, &extents);

    /* Set Location */
    textX = daLeftVColEndPointX - (daLeftVColEndPointX * TICK_LABEL_DISTANCE) - (extents.width + extents.x_bearing);
    textY = (chartEndPointY) - (((chartEndPointY - chartOriginY) / tickCount) * i) - (extents.height/2 + extents.y_bearing);

    /* Draw Text */
    cairo_move_to(cr, textX, textY+7);
    cairo_show_text(cr, tickLabel);

    /* Draw Tick Line */
    cairo_set_line_width(cr, 2);
    cairo_move_to(cr, daLeftVColEndPointX,    textY-2);
    cairo_line_to(cr, daLeftVColEndPointX - (daLeftVColEndPointX * TICK_MARK_LENGTH), textY-2);
    cairo_stroke(cr);

    /* Draw corresponding grid line */
    cairo_set_source_rgba(cr, RGB_BLACK, ALPHA_GRID_LINE);
    cairo_set_line_width(cr, 0.5);
    cairo_move_to(cr, daLeftVColEndPointX, textY-2);
    cairo_line_to(cr, daLeftVColEndPointX+chartEndPointX, textY-2);
    cairo_stroke(cr);
  }
}
//TODO: Text Offset should be based on width of vertical column
//TODO: Tick line length should be based on a percentage of the vertical column width size
void rightVerticalLadder(cairo_t *cr, uint32_t minValue, uint32_t maxValue, uint32_t tickCount) {
  cairo_text_extents_t extents;
  double textX;
  double textY;

  /* Draw Temperature ticks */
  for(int i = 0; i <= tickCount; i++) {
    char tickLabel[5];
    getTickString(tickLabel, i, minValue, ((maxValue - minValue) / tickCount));

    /* Set style */
    cairo_set_source_rgba (cr, RGB_BLACK, 1.0);
    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size   (cr, 30.0);

    /* Set Extents */
    cairo_text_extents(cr, tickLabel, &extents);

    /* Set Location */
    textX = daRightVColOriginX + ((daWidth - daRightVColOriginX) * (TICK_MARK_LENGTH + 0.05));
    textY = (chartEndPointY) - (((chartEndPointY - chartOriginY) / tickCount) * i) - (extents.height/2 + extents.y_bearing);

    /* Draw Text */
    cairo_move_to(cr, textX, textY+7);
    cairo_show_text(cr, tickLabel);

    /* Draw Tick Line */
    cairo_set_line_width(cr, 2);
    cairo_move_to(cr, daRightVColOriginX,    textY-2);
    cairo_line_to(cr, daRightVColOriginX + ((daWidth - daRightVColOriginX) * TICK_MARK_LENGTH), textY-2);
    cairo_stroke(cr);
  }
}

void horizontalLadder(cairo_t *cr, uint32_t minValue, uint32_t maxValue, uint32_t tickCount) {
  cairo_text_extents_t extents;
  double textX;
  double textY;

  /* Draw Temperature ticks */
  for(int i = 0; i <= tickCount; i++) {
    char tickLabel[5];
    getTickString(tickLabel, i, minValue, ((maxValue - minValue) / tickCount));

    /* Set style */
    cairo_set_source_rgba (cr, RGB_BLACK, 1.0);
    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size   (cr, 30.0);

    /* Set Extents */
    cairo_text_extents(cr, tickLabel, &extents);

    /* Set Location */
    //textX = daLeftVColEndPointX;
    //textY = (chartEndPointY) - (((chartEndPointY - chartOriginY) / tickCount) * i) - (extents.height/2 + extents.y_bearing);
    textX = chartOriginX - (((chartEndPointX - chartOriginX) / tickCount) * i) - (extents.width/2 + extents.x_bearing);
    textY = chartOriginY;

    /* Draw Text */
    cairo_move_to  (cr, textX, textY);
    cairo_show_text(cr, tickLabel);

    /* Draw Tick Line */
    cairo_set_line_width(cr, 2);
    cairo_move_to(cr, daRightVColOriginX + (CHART_MARGIN_WIDTH  * daWidth),    textY-2);
    cairo_line_to(cr, daRightVColOriginX + (CHART_MARGIN_WIDTH  * daWidth) + 20, textY-2);
    cairo_stroke(cr);
  }
}
