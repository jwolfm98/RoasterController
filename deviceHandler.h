
#define DEVICE_BUFFER 300

enum {
  devHandRetCode_NoError,
  devHandRetCode_MemoryError,
  devHandRetCode_IOError
} deviceHandlerReturnCodes;

extern int deviceParserInit(char *deviceName);
extern void deviceParserDestroy();
gboolean deviceParser(GIOChannel *source, GIOCondition condition, gpointer data);
