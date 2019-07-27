
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include <glib.h>

#include "deviceHandler.h"

/* Parser State Machine variables */
int   state;
int   dataIndex;
int   bytesInBuffer;
char  chksm;
int   endCount;

gchar ch[DEVICE_BUFFER];
gsize bytesRead;
GError *nullError = NULL;
GError **error = &nullError;

enum {
  STATE_SYNC1,
  STATE_SYNC2,
  STATE_SYNC3,
  STATE_SYNC4,
  STATE_DATA,
  STATE_CHKSUM
};

int deviceParserInit(char *deviceName) {
  /* Setup state machine */
  state         = STATE_SYNC1;
  dataIndex     = 0;
  chksm         = 0;
  endCount      = 0;
  bytesInBuffer = 0;

  memset(&ch, '\0', DEVICE_BUFFER);

  printf("Initialized\n");

  return devHandRetCode_NoError;
}
void deviceParserDestroy() {
}


/* State machine that will read and parse incoming data from OUR device */
gboolean deviceParser(GIOChannel *source, GIOCondition condition, gpointer data) {
  int *tval = (int *)data;
  int messageComplete = FALSE;
  int  i = 0; /*interator for cleanup after parsed message */

  g_io_channel_read_chars(source, &ch[dataIndex], (DEVICE_BUFFER - bytesInBuffer), &bytesRead, error);

  bytesInBuffer += bytesRead;

  if (dataIndex > DEVICE_BUFFER ||
      bytesInBuffer > DEVICE_BUFFER) {
    /* BUFFER OVERFLOW */
    return FALSE;
  }

  /*continue completing data read or parse when packet has been read */
  while ((dataIndex < bytesInBuffer || state == STATE_CHKSUM) && !messageComplete) {
    printf("DataIndex %d\n", dataIndex);
    switch (state) {
      case STATE_SYNC1:
        if (ch[dataIndex] == 'a'){
          state = STATE_SYNC2;
        }
        dataIndex++;
        break;
      case STATE_SYNC2:
        if (ch[dataIndex] == '5'){
          state = STATE_SYNC3;
        }
        else {
          state = STATE_SYNC1;
        }
        dataIndex++;
        break;
      case STATE_SYNC3:
        if (ch[dataIndex] == '5'){
          state = STATE_SYNC4;
        }
        else {
          state = STATE_SYNC1;
        }
        dataIndex++;
        break;
      case STATE_SYNC4:
        if (ch[dataIndex] == 'a'){
          state = STATE_DATA;
        }
        else {
          state = STATE_SYNC1;
        }
        dataIndex++;
        break;
      case STATE_DATA:
        chksm += ch[dataIndex];
        if (ch[dataIndex] == ';') {
          endCount++;
        }
        else {
          endCount = 0;
        }
        if (endCount == 2) {
          state = STATE_CHKSUM;
        }
        else {
          dataIndex++;
        }
        break;
      case STATE_CHKSUM:
        dataIndex++;

        printf("New data::%s \n", ch);

        *tval += chksm;

        //shift excess bytes from read to the beginning for next parser iteration
        for (i = 0; dataIndex < bytesInBuffer; dataIndex++, i++) {
          printf("Cleanup i=%d idx=%d\n", i, dataIndex);
          ch[i] = ch[dataIndex];
        }

        bytesInBuffer = i;

        for (; i < DEVICE_BUFFER; i++) {
          printf("Zeroize i=%d \n", i);
          ch[i] = '\0';
        }

        state           = STATE_SYNC1;
        chksm           = 0;
        endCount        = 0;
        dataIndex       = 0;
        messageComplete = TRUE;

        break;
    }
  }
  return TRUE;
}
