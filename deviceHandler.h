
#define DEVICE_BUFFER    300
#define DATA_PACKET_SIZE 16
typedef enum {
  ps_NoErrors      = 0,
  ps_DeviceError   = 1,
  ps_IOError       = 2,
  ps_CriticalError = 4,
  ps_NoCallbackSet = 8
} parserStatus;

extern int deviceParserInit(char *deviceFilename, RoasterDataPoints *rdp);
extern void deviceParserDestroy();
gboolean deviceParser(GIOChannel *source, GIOCondition condition, gpointer data);
