#include <stdint.h>
#include <assert.h>
#include <malloc.h>
#include "roasterSettings.h"
#include "common.h"

/* Using the set and get functions, math is handled and limits are checked. If hardware is determined to be more capable 
   then these limits suggest, it may be wise to either create versions of the hardware that have preset limits, or simply
   re-compile with different limits. 
   
   We could also write these to file and make it more obstructive to be changed, potentially preventing problems for less 
   advanced users but making the option available. */

#define TEMP_READ_COUNT_LIMIT     10
#define DATA_POINT_INTERVAL_LIMIT 10

/* Settings used to correctly build and display the user inerface and graph (respectively) */
struct RoasterGlobalSettings {
  /* Burner Min/Max is a range of pressure being commanded */
  double burnerMinValue;
  double burnerMaxValue;
  uint8_t tempReadingCount;  /* Shall control the number of readings per update to be averaged */
  uint8_t dataPointInterval; /* Shall control the number of updates per second to be received */
} roasterGlobalSettings;

/* This shall contain a single point of data from the roaster, populated */
struct RoasterDataPoints {
  double burnerFeedback;
  double beanTemperature;
  double environmentTemperature;
} roasterDataPoints;

void rsNew(struct RoasterGlobalSettings *rs){
  assert(rs);
  rs = malloc(sizeof(roasterGlobalSettings));
}

/* Utility functions for the data structs */
double getBurnerRange (struct RoasterGlobalSettings *rs) {
  assert(rs);
  return (rs->burnerMaxValue - rs->burnerMinValue);
}

uint8_t setBurnerMinMaxValues(struct RoasterGlobalSettings *rs, const double min, const double max) {
  assert(rs);
  if (min > max) {
    return FALSE;
  }
  else {
    rs->burnerMinValue = min;
    rs->burnerMaxValue = max;
    return TRUE;
  }
}

uint8_t setTempReadingCount(struct RoasterGlobalSettings *rs, const uint8_t rc) {
  assert(rs);
  if (rc > TEMP_READ_COUNT_LIMIT) {
    return FALSE;
  }
  else {
    rs->tempReadingCount = rc;
    return TRUE;
  } 
}

uint8_t setDataPointInterval(struct RoasterGlobalSettings *rs, const uint8_t dpi) {
  assert(rs);
  if (dpi > DATA_POINT_INTERVAL_LIMIT) {
    return FALSE;
  }
  else {
    rs->dataPointInterval = dpi;
    return TRUE;
  }
}
