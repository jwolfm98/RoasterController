
/* Initializes the chart, left side is temperatures, right side is burner command/value,
 * horizontal is time span */
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
							 ) {

	//Draw on a white background
  //TODO: Color grid label backgrounds to the programs "grey"
  cairo_set_source_rgb(cr, RGB_WHITE);
  cairo_paint(cr);
  
	/* Change the transformation matrix */
  cairo_translate(cr, 0.0, height);
  cairo_scale    (cr, 1.0, -1.0);
	
	/* Determine the data points to calculate (ie. those in the clipping zone */
  cairo_device_to_user_distance(cr, 1.0, 1.0);
  cairo_set_line_width(cr, 1.0);

 //TODO can we assign clip at a later time to ensure the live graph doesn't clip? or is does this need a separate handler?
	cairo_clip_extents(cr, &clip_x1, &clip_y1, &clip_x2, &clip_y2);
	
	cairo_set_source_rgb(cr, RGB_BLACK);
	cairo_rectangle(cr, 
			            CHART_MARGIN_VERTICAL,
									CHART_MARGIN_HORIZONTAL,
									(width  - (CHART_MARGIN_VERTICAL   * 2)),
									(height - (CHART_MARGIN_HORIZONTAL * 2)));
	cairo_stroke(cr);

	
  //TODO: Insert grid labels
  cairo_set_source_rgba(RGB_BLACK, 0.3);
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
	
}
