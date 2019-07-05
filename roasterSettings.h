/* Don't trust yourself - make everything protected by only forward declaring*/

#include <stdint.h>
#include "common.h"

typedef struct RoasterGlobalSettings  RoasterGlobalSettings;
typedef struct RoasterDataPoints      RoasterDataPoints;

extern void    rsNew                (struct RoasterGlobalSettings **rs);
extern double  getBurnerRange       (struct RoasterGlobalSettings *rs);
extern uint8_t setBurnerMinMaxValues(struct RoasterGlobalSettings *rs, const double min, const double max); 
extern uint8_t setTempReadingCount  (struct RoasterGlobalSettings *rs, const uint8_t rc);
extern uint8_t setDataPointInterval (struct RoasterGlobalSettings *rs, const uint8_t dpi);
