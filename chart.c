//TODO:: OriginY of the charting area is actually at the top and endpoint is at the bottom
//i
//          meanign the charting area is actually being drawn from the top left corner
//          to the bottom right corner. This is why coordinates are all screwy. Moving to
//          a proper matrix will not only resolve this, but give more scalability to graphing
//          in a manner that is easily resized and stretched accoridng to the display
//          and charting parameters. Potentially changed on the fly without re-initializing.
//




#include "chart.h"

const uint32_t const len = 10;
int32_t d[10];
float   t[10];

cairo_t *cr;

static uint32_t chartOriginX;          /* pixel location of chart origin X */
static uint32_t chartOriginY;          /* pixel location of chart origin Y */
static uint32_t chartEndPointX;        /* relative chart end point from origin */
static uint32_t chartEndPointY;        /* relative chart end point from origin */
static uint32_t chartEndAbsoluteX;     /* pixel location of chart end point */
static uint32_t chartEndAbsoluteY;     /* pixel location of chart end point */
static uint32_t daWidth;               /* drawing area width */
static uint32_t daHeight;              /* drawing area height */
static uint32_t daLeftVColWidth;       /* drawing area left vertical column width */
static uint32_t daLeftVColHeight;      /* drawing area left vertical column height */
static uint32_t daLeftVColEndPointX;   /* drawing area left vertical column end pointX (absolute) */
static uint32_t daLeftVColEndPointY;   /* drawing area left vertical column end pointY (absolute) */
static uint32_t daRightVColWidth;      /* drawing area right vertical column width */
static uint32_t daRightVColHeight;     /* drawing area right vertical column height */
static uint32_t daRightVColOriginX;    /* drawing area right vertical column origin point X (absolute) */
static uint32_t daRightVColOriginY;    /* drawing area right vertical column origin point Y (absolute) */
static uint32_t daBottomRowWidth;      /* drawing area right vertical column width */
static uint32_t daBottomRowHeight;     /* drawing area right vertical column height */
static uint32_t timeScale;

void leftVerticalLadder(uint32_t minValue, uint32_t maxValue, uint32_t tickCount);
void rightVerticalLadder(uint32_t minValue, uint32_t maxValue, uint32_t tickCount);
void horizontalLadder(uint32_t minValue, uint32_t maxValue, uint32_t tickCount);
void getTickString(char *text, uint8_t tick, uint32_t start, uint32_t step);
int32_t timeToXPlot(float time);

const char *temp_string         = "Temp";
const char *tempUnit_string     = "(Deg F)";
const char *time_string         = "Time (Minutes)";
const char *burnerValue_string1 = "Burner";
const char *burnerValue_string2 = "Command";


/* Initializes the chart, left side is temperatures, right side is burner command/value,
 * horizontal is time span */
void chartInit(cairo_t *_cr,
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
  cr = _cr;
  double   scaleX      =  1.0;
  double   scaleY      = -1.0;
  double   lineWidth   =  1.0;
  double   drawX       =  1.0;
  double   drawY       =  1.0;
  double   clip1       =  0.0;
  double   clip2       =  0.0;
  double   clip3       =  0.0;
  double   clip4       =  0.0;

  //double textX;
  //double textY;
  //cairo_text_extents_t extents;

  /* Setup definitions for chart drawing area */
  daWidth  = width;
  daHeight = height;

  for(int g = 0; g < len; g++) {
    d[g] = 300 + g;
    t[g] = 1.0 + (float) g / 2.0;
  }


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

  /* Charting area is drawn from top left to bottom right
   *__________________________________________
   *|  originX, endY ----------- endX, endY   |
   *|         |                      |        |
   *|         |                      |        |
   *|         |                      |        |
   *|         |                      |        |
   *|         |                      |        |
   *|   originX,Y ------------- endX, originY |
   *|_________________________________________|
   * ^ drawingArea
   */

  /* Origin point starts where the left vertical colum ends */
  chartOriginX   = (uint32_t)floor((double)(CHART_MARGIN_WIDTH         * width));
  chartOriginY   = (uint32_t)floor((double)(CHART_MARGIN_HEIGHT_BOTTOM * height));
  /* Chart end points are relvative to origin points, not absolute to the drawing area */
  chartEndPointX = (uint32_t)floor(width  - (double)(CHART_MARGIN_WIDTH  * width * 2));
  chartEndPointY = (uint32_t)floor(height - (double)(CHART_MARGIN_HEIGHT_BOTTOM * height +
                                                     CHART_MARGIN_HEIGHT_TOP    * height));

  chartEndAbsoluteX = (uint32_t)floor(width  - (double)(CHART_MARGIN_WIDTH         * width));
  chartEndAbsoluteY = (uint32_t)floor(height - (double)(CHART_MARGIN_HEIGHT_BOTTOM * height));

  /* Left Vertical Column Ladder Parameters */
  daLeftVColEndPointX = chartOriginX;
  daLeftVColEndPointY = chartOriginY;
  daLeftVColWidth     = daLeftVColEndPointX;
  daLeftVColHeight    = (uint32_t)(floor((double)(CHART_MARGIN_HEIGHT_BOTTOM * height)) -
                                   floor((double)(CHART_MARGIN_HEIGHT_TOP    * height)));

  /* Right Vertical Column Ladder Parameters */
  daRightVColOriginX = (uint32_t)floor(width  - (double)(CHART_MARGIN_WIDTH  * width));
  daRightVColOriginY = (uint32_t)floor(height - (double)(CHART_MARGIN_HEIGHT_BOTTOM * height));
  daRightVColWidth   = daWidth - daRightVColOriginX;
  daRightVColHeight  = daLeftVColHeight;

  daBottomRowHeight  = CHART_MARGIN_HEIGHT_BOTTOM * height;
  daBottomRowWidth   = CHART_MARGIN_WIDTH  * width;

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

  leftVerticalLadder (leftVerticalMin,  leftVerticalMax,       verticalTicks);
  rightVerticalLadder(rightVerticalMin, rightVerticalMax,      verticalTicks);
  horizontalLadder   (0,                horizontalStartingMax, horizontalStartingMax);


  plot(d, t, len, RGB_BLACK, ALPHA_OPAQUE);


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


//TODO: ladder initial x.y should refer the a point on the edge of the graph, text and tick marks should be offset from there.




//TODO: Text Offset should be based on width of vertical column
//TODO: Tick line length should be based on a percentage of the vertical column width size
void leftVerticalLadder(uint32_t minValue, uint32_t maxValue, uint32_t tickCount) {
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
    textX = daLeftVColEndPointX - (daLeftVColEndPointX * TICK_LABEL_DISTANCE) - (extents.width/1.8 + extents.x_bearing);
    textY = (chartEndPointY) - (((chartEndPointY - chartOriginY) / tickCount) * i) - (extents.height/2 + extents.y_bearing);

    /* Draw Text */
    cairo_move_to(cr, textX, textY+7);
    cairo_show_text(cr, tickLabel);

    /* Draw Tick Line */
    cairo_set_line_width(cr, 2);
    cairo_move_to(cr, daLeftVColEndPointX, textY-2);
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
void rightVerticalLadder(uint32_t minValue, uint32_t maxValue, uint32_t tickCount) {
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
    //TODO: change TICK_MARK_LENGTH to TICK_LABEL_DISTANCE and adjust accordingly
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



//TODO: Start timeline at a negative time.
//        1. Time shall start at a negative time
//        2. Chart shall plot from 0 seconds to -X seconds, is the minimum value sent to the horizontal ladder function
//        3. The chart shall plot data from 0 to the aforementioned -X until the time data becomes positive
//
void horizontalLadder(uint32_t minValue, uint32_t maxValue, uint32_t tickCount) {
  cairo_text_extents_t extents; /* stores text alignment variables */
  double tickX; /* initial position for the tick mark */
  double tickY; /* initial position for the tick mark */
  timeScale = tickCount / 60; /* all time is in seconds, label graph in minutes */

  /* Draw Time ticks */
  for(int i = 0; i <= timeScale; i++) {
    char tickLabel[5];
    getTickString(tickLabel, i, minValue, ((maxValue - minValue) / tickCount));

    /* Set style */
    cairo_set_source_rgba (cr, RGB_BLACK, 1.0);
    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size   (cr, 30.0);

    /* Set Extents */
    cairo_text_extents(cr, tickLabel, &extents);

    /* Set Location of the Tick Mark (this aligns with the actual data in the graph */
    tickX = chartOriginX +                                              /* Offset + */
            (((chartEndPointX - chartOriginX) / timeScale) * i) + /* Tick Iteration Space + */
            (((chartEndPointX - chartOriginX) / timeScale) * 0.5);/* Tick Half step (so 0 is not on the edge) */
    tickY = chartEndAbsoluteY;

    /* Draw Text */
    cairo_move_to  (cr, tickX - (extents.width/2 + extents.x_bearing), tickY + (daBottomRowHeight * TICK_LABEL_DISTANCE));
    cairo_show_text(cr, tickLabel);

    /* Draw Tick Line */
    cairo_set_line_width(cr, 2);
    cairo_move_to(cr, tickX, tickY);
    cairo_line_to(cr, tickX, tickY + (daBottomRowHeight * TICK_MARK_LENGTH));
    cairo_stroke(cr);

    /* Draw corresponding grid line */
    cairo_set_source_rgba(cr, RGB_BLACK, ALPHA_GRID_LINE);
    cairo_set_line_width(cr, 0.5);
    cairo_move_to(cr, tickX, tickY);
    cairo_line_to(cr, tickX, (chartOriginY - (CHART_MARGIN_HEIGHT_TOP * daHeight)));
    cairo_stroke(cr);
  }
}

void plot(
  int32_t  *data,
  float    *time,
  uint32_t  length,
  float     R,
  float     G,
  float     B,
  float     A)
{
  cairo_set_source_rgba(cr, R, G, B, A);
  for (uint32_t i = 0; i < length - 1; i++) {
    cairo_move_to(cr, timeToXPlot(time[i]), data[i]);
    cairo_line_to(cr, timeToXPlot(time[i+1]), data[i+1]);
  }
  cairo_stroke(cr);
}

int32_t timeToXPlot(float time) {
  return chartOriginX + (((float)(chartEndPointX - chartOriginX) / (timeScale * 60)) * time);
}
