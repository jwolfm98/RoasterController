#include <gtk/gtk.h>
#include "roastEvent.h"
#include "common.h"

#define dataIntervalHZ (2) /* HZ */
#define dataTotal      (dataIntervalHZ * 60 * 30) /* to allocate 30 minutes of data at given dataInterval */

typedef struct roastDatas {
  float dataPoints[dataTotal];
} roastData;

typedef struct roastEvents {
  long time_charged;
  long time_dryEnd;
  long time_firstCrack;
  long time_drop;
  long time_complete;

} roastEvent;

typedef struct roastProfiles {
  roastEvent eventsStamps;

  roastData beanTemps;
  roastData beanDelta;
  roastData envTemps;
  roastData envDelta;
  roastData burnerRate;

} roastProfile;

gboolean roastStarted = FALSE;

gboolean isRoasting() {
  return roastStarted;
}
void startRoasting() {
  roastStarted = TRUE;
}
void stopRoasting() {
  roastStarted = FALSE;
}
