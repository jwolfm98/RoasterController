
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdint.h>

#include <glib.h>

#include "deviceHandler.h"
#include "roastEvent.h"
#include "chart.h"


FILE *debugOut;

/* Status of parser */
parserStatus status;

/* data storage for parsed data */
int32_t beanTemp;
int32_t envTemp;
int32_t burnValue;
int32_t timeStamp;

/* Parser State Machine variables */
typedef enum {
  STATE_SYNC1,
  STATE_SYNC2,
  STATE_SYNC3,
  STATE_SYNC4,
  STATE_DATA,
  STATE_CHKSUM
} PARSER_STATE;
PARSER_STATE state;
uint32_t     dataIndex;
int32_t      lastKnownGoodIndex;
uint8_t      checksumFailed;
uint32_t     bytesInBuffer;
uint8_t      chksm;
uint8_t      dataCount;
uint32_t     syncErrors;
uint32_t     checksumErrors;
uint32_t     parserSawtooth;

/* Glib variables for reading from GIOChannel */
GIOChannel *deviceChannel;
gchar       ch[DEVICE_BUFFER];
gsize       bytesRead;
GError     *nullError = NULL;
GError    **error = &nullError;

gboolean deviceParserInit(char *defaultDevice, RoasterDataPoints *rdp) {
  gboolean status = TRUE;

  beanTemp  = 0;
  envTemp   = 0;
  burnValue = 0;
  timeStamp = 0;

  /* Setup state machine */
  state              =  STATE_SYNC1;
  dataIndex          =  0;
  chksm              =  0;
  dataCount          =  0;
  bytesInBuffer      =  0;
  syncErrors         =  0;
  checksumErrors     =  0;
  parserSawtooth     =  0;
  checksumFailed     =  0;
  lastKnownGoodIndex = -1;

  /* Initialize buffer to make debug print easier */
  memset(&ch, '\0', DEVICE_BUFFER);

  //TODO: Refactor this into a connectToDevice()
  /* Open device and setup watch function handler */
  if ((device = open(defaultDevice, O_RDONLY)) < 0) {
    status = FALSE;
  }
  else {
    /* Send parser function to main gloop context */
    deviceChannel = g_io_channel_unix_new(defaultDevice);
    g_io_add_watch(deviceChannel, G_IO_IN, deviceParser, rdp);
    g_io_channel_unref(deviceChannel);
  }
  //:TODO End


#ifdef DEBUG
  if (DEBUG_STATE == (DEBUG_ALL | DEBUG_DEVICES | DEBUG_PARSER)) {
    if ((debugOut = fopen("debugDeviceParser.txt", "w")) == NULL) {
      printf("Warning: Debug output for devices and parser could not open dump file: debugDeviceParser.txt\n");
    }
  }
#endif

  return status;
}

/* TODO: Implement connectToDevice
 *
 * Why: This function will serve both deviceParserInit() as well as subsequent attempts
 * to connecting to a device. This is needed because it is easily possible for a USB
 * device to take on a different name if it is reconnected after boot.
 *
 * How: This will need to ensure that the Init function keeps a reference to the parser
 * loop assigned by g_io_add_watch. This watch will need to be removed and then replaced
 * with a new watch on the new device. This should happen even if the device name has not changed.
 * It may be best practice to call this function from deviceParserInit(), check for and existing
 * watch, and re-attempt the open/watch process. This will also need to close the current device
 * before opening.
 *
 * */
#if 0
gboolean connectToDevice(char *device, RoasterDataPoints *rdp) {
}
#endif



void deviceParserDestroy() {
  /* Pretty sure we can simple call fclose on the debug file rather
   * than checking debug state */
#ifdef DEBUG
  if (DEBUG_STATE == (DEBUG_ALL | DEBUG_DEVICES | DEBUG_PARSER)) {
    fclose(debugOut);
  }
#endif
  close(device);
}


/* State machine that will read and parse incoming data from OUR device */
gboolean deviceParser(GIOChannel *source, GIOCondition condition, gpointer data) {

  RoasterDataPoints *rdp             = (RoasterDataPoints *)data;
  uint8_t            messageComplete = FALSE;
  static uint32_t    firstDataByte;

  /*interator for cleanup after parsed message */
  uint32_t i = 0;

  parserSawtooth++;

  g_io_channel_read_chars(source, &ch[dataIndex], (DEVICE_BUFFER - bytesInBuffer), &bytesRead, error);

  bytesInBuffer += bytesRead;

  if (dataIndex > DEVICE_BUFFER ||
      bytesInBuffer > DEVICE_BUFFER) {
    /* BUFFER OVERFLOW */
    return FALSE;
  }

  /*continue completing data read or parse when packet has been read */
  while (((dataIndex < bytesInBuffer) && !messageComplete) || checksumFailed) {
    /* If the checksum fails, the following could be true:
     *   1. The device was recently connected and
     *        - the data was capture mid-message
     *        - data had matching sync bytes
     *   2. Noise occured and corrupted the data on the line
     *
     *   In both events, it may be worth attempting to salvage a message by starting
     *   at 2 counts after the last known message ended, which is 1 more than where
     *   the initial message parse began. If the last known good index is not a valid
     *   index the we cannot proceed and should simply keep looking for a match.
     *
     *   TODO: Another corner case is that the data in the message could be constant
     *   for many cycles and might require that the parser attempt to find sync bytes
     *   by searching backwards to find the synce bytes that align with the packet
     *   and not the sync bytes that are appearing in the data section of the packet.
     */
    if (checksumFailed == 1 && lastKnownGoodIndex != -1) {
      dataIndex = lastKnownGoodIndex + 2;
    }
    checksumFailed = 0;

    switch (state) {

      /* Sync States */
      case STATE_SYNC1:
        if (ch[dataIndex] == 'a'){
          state = STATE_SYNC2;
        }
        else {
          syncErrors++;
        }
        dataIndex++;
        break;

      case STATE_SYNC2:
        if (ch[dataIndex] == '5'){
          state = STATE_SYNC3;
        }
        else {
          syncErrors++;
          state = STATE_SYNC1;
        }
        dataIndex++;
        break;

      case STATE_SYNC3:
        if (ch[dataIndex] == '5'){
          state = STATE_SYNC4;
        }
        else {
          syncErrors++;
          state = STATE_SYNC1;
        }
        dataIndex++;
        break;

      case STATE_SYNC4:
        if (ch[dataIndex] == 'a'){
          state      = STATE_DATA;
          syncErrors = 0;
        }
        else {
          syncErrors++;
          state = STATE_SYNC1;
        }
        dataIndex++;
        firstDataByte = dataIndex;
        dataCount     = 0;
        break;
      /* End Sync States */

      case STATE_DATA:
        /* start checksum calc */
        chksm += ch[dataIndex];
        dataCount++;

        if (dataCount == DATA_PACKET_SIZE) {
          state = STATE_CHKSUM;
        }

        dataIndex++;
        break;

      case STATE_CHKSUM:
        if (ch[dataIndex] != chksm) {
          checksumFailed = 1;
        }
        else {
          /* TODO: Update device firmware to push data in 4-byte format instead of raw number as ascii */
          beanTemp   = 0;
          envTemp    = 0;
          burnValue  = 0;
          timeStamp  = 0;

          beanTemp  |= (int32_t)((ch[firstDataByte++] << 24) & 0xFF);
          beanTemp  |= (int32_t)((ch[firstDataByte++] << 16) & 0xFFFF);
          beanTemp  |= (int32_t)((ch[firstDataByte++] <<  8) & 0xFFFFFF);
          beanTemp  |= (int32_t) (ch[firstDataByte++]        & 0xFFFFFFFF);
          envTemp   |= (int32_t)((ch[firstDataByte++] << 24) & 0xFF);
          envTemp   |= (int32_t)((ch[firstDataByte++] << 16) & 0xFFFF);
          envTemp   |= (int32_t)((ch[firstDataByte++] <<  8) & 0xFFFFFF);
          envTemp   |= (int32_t) (ch[firstDataByte++]        & 0xFFFFFFFF);
          burnValue |= (int32_t)((ch[firstDataByte++] << 24) & 0xFF);
          burnValue |= (int32_t)((ch[firstDataByte++] << 16) & 0xFFFF);
          burnValue |= (int32_t)((ch[firstDataByte++] <<  8) & 0xFFFFFF);
          burnValue |= (int32_t) (ch[firstDataByte++]        & 0xFFFFFFFF);
          timeStamp |= (int32_t)((ch[firstDataByte++] << 24) & 0xFF);
          timeStamp |= (int32_t)((ch[firstDataByte++] << 16) & 0xFFFF);
          timeStamp |= (int32_t)((ch[firstDataByte++] <<  8) & 0xFFFFFF);
          timeStamp |= (int32_t) (ch[firstDataByte++]        & 0xFFFFFFFF);

          rdp->addData(beanTemp, envTemp, burnValue, timeStamp);

          dataIndex++;

          //shift excess bytes from read to the beginning for next parser iteration
          for (i = 0; dataIndex < bytesInBuffer; dataIndex++, i++) {
            ch[i] = ch[dataIndex];
          }

          bytesInBuffer = i;

          for (; i < DEVICE_BUFFER; i++) {
            ch[i] = '\0';
          }

          lastKnownGoodIndex = dataIndex;
          chksm              = 0;
          dataIndex          = 0;
          messageComplete    = TRUE;
        }

        state = STATE_SYNC1;
        break;
    }
  }
  return TRUE;
}
