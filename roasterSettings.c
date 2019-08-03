#include <stdint.h>
#include <malloc.h>
#include <stdint.h>
#include "roasterSettings.h"

#define SETTINGS_FILE_NAME        "config.ini"
#define DEFAULT_DEVICE_NAME       "/dev/ttyUSB0"
#define DEFAULT_DEVICE_LABEL      "USB0"
#define STRING_BUFFER_SIZE        (1024)

/* Settings used to correctly build and display the user inerface and graph (respectively) */
typedef struct RoasterGlobalSettings {
  /* Default Device Settings */
  char deviceLabel[STRING_BUFFER_SIZE];
  char deviceFileName[STRING_BUFFER_SIZE];

  /* Graph Settings */
  int32_t   tempMin;
  int32_t   tempMax;
  uint32_t  initialTimeSpan;
  uint32_t  preRoastDataSpan; /* How many seconds of data pre-roast should be saved */
  uint32_t  deltaSpan;
  int32_t   deltaRate;
  uint32_t  burnerMin;
  uint32_t  burnerMax;
  uint8_t   graphSettingsUpdated;

  COLORS   beanTempColor;
  COLORS   beanDeltaColor;
  COLORS   envTempColor;
  COLORS   burnerValueColor;
};

RoasterGlobalSettings *rs;

/* TODO: GLIB has an interface for serializing and storing application settings.
 *        Would be better to implement this down the line for ease of use. */
SETTINGS_INIT roasterSettingsInit() {
  /* setup and open defaults file */
  rs = (RoasterGlobalSettings *)malloc(sizeof(RoasterGlobalSettings));

  /* fail on inability to allocate memory */
  if ((rs == NULL)) {
    return SETTINGS_INIT_CRITICAL;
  }

  /* do default */
  memset(&rs->deviceLabel, "\0", STRING_BUFFER_SIZE);
  memset(&rs->deviceName,  "\0", STRING_BUFFER_SIZE);

  sprintf(rs->deviceLabel, DEFAULT_DEVICE_LABEL);
  sprintf(rs->deviceName,  DEFAULT_DEVICE_NAME);

  rs->tempMin          = 0;
  rs->tempMax          = 450;
  rs->initialTimeSpan  = 60 * 10; //seconds
  rs->preRoastDataSpan = 20;      //seconds
  rs->deltaSpan        = 15;
  rs->deltaRate        = 15;
  rs->burnerMin        = 0;
  rs->burnerMax        = 10;

  /* Set this so that chart.c knows to updated the graph */
  rs->graphSettingsUpdated = 1;

  rs->beanTempColor.RED   = 1.0f;
  rs->beanTempColor.GREEN = 0.0f;
  rs->beanTempColor.BLUE  = 0.0f;
  rs->beanTempColor.ALPHA = 1.0f;

  rs->beanDeltaColor.RED   = 0.0f;
  rs->beanDeltaColor.GREEN = 0.0f;
  rs->beanDeltaColor.BLUE  = 1.0f;
  rs->beanDeltaColor.ALPHA = 1.0f;

  rs->envTempColor.RED   = 0.0f;
  rs->envTempColor.GREEN = 1.0f;
  rs->envTempColor.BLUE  = 0.0f;
  rs->envTempColor.ALPHA = 1.0f;

  rs->burnerValueColor.RED   = 0.80f;
  rs->burnerValueColor.GREEN = 0.0f;
  rs->burnerValueColor.BLUE  = 0.20f;
  rs->burnerValueColor.ALPHA = 1.0f;

  return SETTINGS_INIT_DEFAULT;

}

int32_t  getTempMin()             { return rs->tempMin; }
int32_t  getTempMax()             { return rs->tempMax; }
uint32_t getInitTimeSpan()        { return rs->initialTimeSpan; }
uint32_t getPreRoastDataSpan()    { return rs->preRoastDataSpan; }
uint32_t getDeltaSpan()           { return rs->deltaSpan; }
int32_t  getDeltaRate()           { return rs->deltaRate; }
uint32_t getBurnerMin()           { return rs->burnerMin; }
uint32_t getBurernMax()           { return rs->burnerMax; }

uint8_t  updateGraphSettingsNow() { return rs->graphSettingsUpdated; }

COLORS getBeanTempColor()         { return rs->beanTempColor; }
COLORS getBeanDeltaColor()        { return rs->beanDeltaColor; }
COLORS getEnvTempColor()          { return rs->envTempColor; }
COLORS get(BurnerValColor)        { return rs->burnerValueColor; }

