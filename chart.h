
#include <stdint.h>
#include <stdio.h>
#include <cairo.h>
#include <math.h>


/* Margin Percentages */
#define CHART_MARGIN_WIDTH         0.05
#define CHART_MARGIN_HEIGHT_BOTTOM 0.10
#define CHART_MARGIN_HEIGHT_TOP    0.05
#define TICK_MARK_LENGTH           0.20
#define TICK_LABEL_DISTANCE        0.50

#define RGB_BLACK   0,   0,   0
#define RGB_RED   1.0,   0,   0
#define RGB_GREEN   0, 1.0,   0
#define RGB_BLUE    0,   0, 1.0
#define RGB_WHITE 1.0, 1.0, 1.0
#define RGB_GREY  1.0, 1.0, 1.0, 0.4

#define ALPHA_TRANSPARENT 0.0
#define ALPHA_OPAQUE      1.0
#define GTK_ALPHA         0.005
#define ALPHA_GRID_LINE   0.4


extern void chartInit(cairo_t *cr,
             uint32_t width,                /* Widith of drawing area         */
             uint32_t height,               /* Height of drawing area         */
             uint8_t  verticalTicks,        /* Vertical tick count            */
             uint8_t  horizontalTicks,      /* Horizontal tick count          */
             uint8_t  gridLines,            /* Grid lines on/off              */
             uint32_t leftVerticalMax,      /* Left Vertical max number       */
             uint32_t leftVerticalMin,      /* Left Vertical min number       */
             uint32_t rightVerticalMax,     /* Right Vertical max number      */
             uint32_t rightVerticalMin,     /* Right Veritcal min number      */
             uint32_t horizontalStartingMax); /* Horizontal starting max number */

extern void plot(int32_t *data,
                 float    *time,
                 uint32_t  length,
                 float     R,
                 float     G,
                 float     B,
                 float     A);

