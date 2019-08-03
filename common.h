#ifndef __COMMON_H
#define __COMMON_H

#ifndef FALSE
#define FALSE (0)
#endif
#ifndef TRUE
#define TRUE (!FALSE)
#endif
/* Wrap debug functions after major initial testing is complete
 * and a baseline has been established. Otherwise, assume all
 * debug functionality should be enabled for development testing 
 * purposes (unless the debug process might have major performance
 * impact) */
#define DEBUG
#ifdef DEBUG
int DEBUG_STATE;
typedef enum {
  DEBUG_ALL     = 0,
  DEBUG_DEVICES = 1,
  DEBUG_PARSER  = 2,
  DEBUG_APP     = 4,
  DEBUG_CHART   = 8
} DEBUG_STATES;
#endif


#endif
