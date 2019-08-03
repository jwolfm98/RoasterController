/* Don't trust yourself - make everything protected by only forward declaring*/

#include <stdint.h>
#include "common.h"


typedef enum {
  SETTINGS_INIT_DEFAULT,
  SETTINGS_INIT_LOADED,
  SETTINGS_INIT_CRITICAL
} SETTINGS_INIT;

typedef struct COLORS {
  float RED;
  float GREEN;
  float BLUE;
  float ALPHA;
} COLORS;

SETTINGS_INIT roasterSettingsInit();

int32_t  getTempMin();
int32_t  getTempMax();
uint32_t getInitTimeSpan();
uint32_t getPreRoastDataSpan();
uint32_t getDeltaSpan();
int32_t  getDeltaRate();
uint32_t getBurnerMin();
uint32_t getBurernMax();

uint8_t  updateGraphSettingsNow();

COLORS   getBeanTempColor();
COLORS   getBeanDeltaColor();
COLORS   getEnvTempColor();
COLORS   get(BurnerValColor);
