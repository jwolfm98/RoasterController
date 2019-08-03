#include "common.h"
#include <gtk/gtk.h>

typedef enum {
  DATA_CONTEXT_BEAN_TEMP,
  DATA_CONTEXT_ENV_TEMP,
  DATA_CONTEXT_BURNER_VALUE,
} DATA_CONTEXT;

typedef struct RoasterDataPoints      RoasterDataPoints;

/* Control Functions */
void     startRoasting(void);
void     stopRoasting(void);

/* Utility Functions */
gboolean isRoasting(void);
int32_t  getCurrentBeanTemp(void);
int32_t  getCurrentEnvTemp(void);
int32_t  getCurrentBurnerValue(void);

