#include <gtk/gtk.h>
#include "roastEvent.h"
#include "roasterSettings.h"
#include "common.h"

typedef struct roastEvents {
  long time_charged;
  long time_dryEnd;
  long time_firstCrack;
  long time_drop;
  long time_complete;

} roastEvent;

/* This shall contain a single point of data from the roaster, populated */
struct RoasterDataPoints {
  int32_t beanTemp;
  int32_t envTemp;
  int32_t burnerValue;

  int32_t beanTempDelta;
  int32_t envTempDetlta;

  int32_t timeStamp;

} roasterDataPoints;

typedef struct RoastProfile {
  roastEvent eventsStamps;

  roasterDataPoints *rdps;

} RoastProfile;


RoastProfile profile;
RoastProfile profileShdw;
gboolean     newData;
gboolean     roastStarted;
uint32_t     dataIndex;

gboolean initRoasterEventData() {
  //allocate an hours worth of data at 4 points per second, a lot more than we really need
  //TODO: fix this later
  profile.rdps     = (RoasterDataPoints *)malloc(sizeof(RoasterDataPoints) * 3600 * 4);
  profileShdw.rdps = (RoasterDataPoints *)malloc(sizeof(RoasterDataPoints) * 3600 * 4);
  newData      = FALSE;
  roastStarted = FALSE;
  //TODO: make this dynamic later
  dataIndex    = (getPreRoastDataSpan() * 4); //again at 4 points per second
}


gboolean isRoasting() {
  return roastStarted;
}
void startRoasting() {
  roastStarted = TRUE;
}
void stopRoasting() {
  roastStarted = FALSE;
}

void calcDeltas() {
  /* Initial algorithm for demo, simply split the difference */
  profile.rdps[dataIndex]->beanTempDelta = profile.rdps[dataIndex]->beanTemp - profile.rdps[dataIndex-1]->beanTemp;
  profile.rdps[dataIndex]->envTempDelta  = profile.rdps[dataIndex]->envTemp  - profile.rdps[dataIndex-1]->envTemp;
}

void addData(int32_t beanTemp, int32_t envTemp, int32_t burnerValue, int32_t timeStamp) {
  profile.rdps[dataIndex]->beanTemp    = beanTemp;
  profile.rdps[dataIndex]->envTemp     = envTemp;
  profile.rdps[dataIndex]->burnerValue = burnerValue;
  profile.rdps[dataIndex]->timeStamp   = timeStamp;

  if (isRoasting()) {
    calcDeltas();
    dataIndex++;
  }
  else {
    /* Hold onto data coming in before roasting starts, dataIndex is initialized to the buffer point first */
    for (int i = 0; i < dataIndex; i++) {
      profile.rdps[i]->beanTemp    = profile.rdps[i+1]->beanTemp;
      profile.rdps[i]->envTemp     = profile.rdps[i+1]->envTemp;
      profile.rdps[i]->burnerValue = profile.rdps[i+1]->burnerValue;
      profile.rdps[i]->timeStamp   = profile.rdps[i+1]->timeStamp;
    {
  }
}

/* Get Current Data Utility Functions
 *  :: Always return the previous index as the current index is what is waiting to be set */
int32_t getCurrentBeanTemp() {
  return profile.rdps[dataIndex-1]->beanTemp;
}
int32_t getCurrentEnvTemp() {
  return profile.rdps[dataIndex-1]->envTemp;
}
int32_t getCurrentBurnerValue() {
  return profile.rdps[dataIndex-1]->burnerValue;
}

