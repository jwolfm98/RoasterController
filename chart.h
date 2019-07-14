
#include <cairo.h>
#include <math.h>

#define CHART_MARGIN_VERTICAL   100.0
#define CHART_MARGIN_HORIZONTAL 180.0

#define RGB_BLACK   0,   0,   0
#define RGB_RED   1.0,   0,   0
#define RGB_GREEN   0, 1.0,   0
#define RGB_BLUE    0,   0, 1.0
#define RGB_WHITE 1.0, 1.0, 1.0
#define RGB_GREY  1.0, 1.0, 1.0, 0.4 


void chartInit(cairo_t *cr,
	             uint32_t width,                /* Widith of drawing area         */
							 uint32_t height,               /* Height of drawing area         */
							 uint8_t  leftVerticalTicks,    /* Left vertical tick count       */
							 uint8_t  rightVerticalTicks,   /* Right vertical tick count      */
							 uint8_t  horizontalTicks,      /* Horizontal tick count          */
							 uint8_t  gridLines,            /* Grid lines on/off              */
							 uint32_t leftVerticalMax,      /* Left Vertical max number       */
							 uint32_t leftVerticalMin,      /* Left Vertical min number       */
							 uint32_t rightVerticalMax,     /* Right Vertical max number      */
							 uint32_t rightVerticalMin,     /* Right Veritcal min number      */
							 uint32_t horizontalStartingMax /* Horizontal starting max number */
							 );
