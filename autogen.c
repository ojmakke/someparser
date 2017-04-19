#include "autogen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "grammer.tab.h"

void printAllInfo();

struct AllMessages allMessages;
struct MessageStruct *activeMessage; /* Signals will be added to this one */

void init()
{
  allMessages.messageCount = 0;
  allMessages.messages = NULL;
  activeMessage = NULL;
}

void clean()
{
  
}


/* Adding functions */

void addMessage(char* name,
		char* source,
		unsigned int id,
		unsigned int length)
{
  struct MessageStruct *newMessage =
    (struct MessageStruct*) malloc(sizeof(struct MessageStruct));
  if(newMessage == NULL)
    {
      fprintf(stdout, "Cannot allocate memory for new message\n");
      return;
    }

  newMessage->id = id;
  newMessage->length = length;
  newMessage->messageName = name;
  newMessage->sender = source;
  activeMessage = newMessage;
  newMessage->data = NULL;
  printf("Active message set to %s\n", newMessage->messageName);
  newMessage->signals = NULL;
  newMessage->signalNumber = 0;
  struct MessageStruct **newAllMessages =
    (struct MessageStruct **) realloc(allMessages.messages,
				      (allMessages.messageCount+1)*
				      sizeof(struct MessageStruct*));

  if(newAllMessages == NULL)
    {
      fprintf(stdout, "Cannot allocate memory for new message");
      free(newMessage);
      return;
    }

  allMessages.messages = newAllMessages;
  allMessages.messages[allMessages.messageCount++] = newMessage;
  printf("Message added\n");
  //  printAllInfo();
  
}

void addSignal(char* name,
	       unsigned int bitStart,
	       unsigned int length,
	       float offset,
	       float scale,
	       unsigned char isSigned,
	       unsigned char isLittleEndian)
{

  printf("Addinng signal\n");
  struct SignalStruct* newSignal = (struct SignalStruct* )
    malloc(sizeof(struct SignalStruct));
  if(newSignal == NULL)
    {
      fprintf(stdout, "Cannot create new signal\n");
      return;
    }
  printf("Signal # before adding:%d\n", activeMessage->signalNumber);
  struct SignalStruct** newSignals = (struct SignalStruct **)
    realloc(activeMessage->signals,
	    (activeMessage->signalNumber+1)*sizeof(struct SignalStruct*));
 
  if(newSignals == NULL)
    {
      fprintf(stdout, "Cannot allocate memory for new signal\n");
      free(newSignal);
      return;
    }
 
  newSignal->bitStart = bitStart;
  newSignal->isLittleEndian = isLittleEndian;
  newSignal->isSigned = isSigned;
  newSignal->length = length;
  newSignal->offset = offset;
  newSignal->scale = scale;
  newSignal->signalName = name;

  activeMessage->signals = newSignals;
  activeMessage->signals[activeMessage->signalNumber++] = newSignal;
  printf("Added signal\n");
  // printAllInfo();
}





/* Printing functions */
void printSignalInfo(struct SignalStruct *signal)
{
  printf("+++++\n");
  printf("Signal name:\t\t%s\n", signal->signalName);
  printf("Bit Start:\t\t%d\n", signal->bitStart);
  printf("Bit Length:\t\t%d\n", signal->length);
  printf("Little Endian:\t\t%d\n", signal->isLittleEndian);
  printf("Signed:\t\t%d\n", signal->isSigned);
  printf("Offset:\t\t%f\n", signal->offset);
  printf("Scale:\t\t%f\n", signal->scale);
}

void printMessageInfo(struct MessageStruct *message)
{
  if(message == NULL)
    {
      printf("Message not found in dbc\n");
      return;
    }
  fprintf(stdout, "Message id:\t\t%d\n", message->id);
  printf("Message length:\t\t%d\n", message->length);
  if(message->messageName != NULL)
    printf("Message name:\t\t%s\n", message->messageName);

  if(message->sender != NULL)
    printf("Message source:\t\t%s\n", message->sender);

  int ii;
  for(ii = 0; ii < message->signalNumber; ii++)
    {
      printSignalInfo( message->signals[ii]);
    }
  printf("_______________________________\n");
}
void printAllInfo()
{
  if(allMessages.messageCount == 0)
    {
      fprintf(stdout, "No Messages found\n");
      return;
    }

  int ii;
  for(ii = 0; ii < allMessages.messageCount; ii++)
    {
      printMessageInfo(allMessages.messages[ii]);
    }
}


/* Sorting */
void sortById()
{
  int ii;
  int jj;
  struct MessageStruct *tempMessage;
  int sorting = 0;
  
  if(allMessages.messageCount < 2) return; /* Nothing to sort */
  for(jj = 1; jj < allMessages.messageCount; jj++)
    {
      sorting = 0;
      for(ii = 1; ii < allMessages.messageCount; ii++)
	{
	  if(allMessages.messages[ii-1]->id > allMessages.messages[ii]->id)
	    {
	      tempMessage = allMessages.messages[ii];
	      allMessages.messages[ii] = allMessages.messages[ii-1];
	      allMessages.messages[ii-1] = tempMessage;
	      sorting = 1;
	    }
	}
      if(!sorting) break; /* Quit if nothing remains to sort */
    }
}

struct MessageStruct *getMessage(unsigned int messageID)
{
  int index = 0;
  int upperBound = allMessages.messageCount;
  int lowerBound = 0;
  struct MessageStruct *retValue = NULL;
  while(1) /* Loop until we find the value or searh all */
    {
      if(allMessages.messages[index]->id == messageID)
	{
	  retValue = allMessages.messages[index];
	  break;
	}
      else if(allMessages.messages[index]->id >messageID)
	{
	  upperBound = index;
	  index -= (index - lowerBound)/2;
	}
      else
	{
	  lowerBound = index;
	  index += (upperBound - index)/2;
	}

      if(index == lowerBound || index == upperBound) break;
    }
  return retValue;
}

/* This pushes everything to the right to align
   For CAN, byte 0 is on the left.
BYTE0 BYTE1 BYTE2 ...

BYTEX: Bitn Bitn-1, ... Bit0 (Inverted Bit Numbering)

BitStart starts at 0. Bitlength > 0 always
*/
unsigned char* littleFronBigEndian(unsigned char* in, int bitStart, int bitLength)
{
  /* Using reverse bits is confusing here. bit 7 means leftmost bit */
  unsigned int startByte = bitStart/8;
  unsigned int lrBitStart = (startByte)*8 + 7-bitStart%8; /* Starting from left to right*/
  unsigned int endBit = lrBitStart + bitLength -1; /* Last bit, inclusive */
  unsigned int endByte = endBit/8;
  printf("startByte:\t%d\n lrBitStart\t%d\n endBit:\t%d\n endByte\t%d\n",
	 startByte, lrBitStart, endBit, endByte);
  size_t bufferSize = (endByte - startByte) + 1; /* 0-7, 1. 8-15, 2 */
  printf("Bufer size: %lu\n", bufferSize);
  unsigned char* buffer = (unsigned char *) malloc(bufferSize);
  memset(buffer, 0, bufferSize);
  
  /* Now copy bytes to buffer. This will have exccess data */
  
  int ii;
  for(ii = 0; ii < bufferSize; ii++)
    {
      unsigned char mask = 0xFF;
      if(ii == 0)
	{
	  mask = (1<<(bitStart%8+1))-1; /* Create FFFFF... up to bitStart */
	}
    
      buffer[ii] = in[startByte + ii]&mask;
      printf("Mask is %d and byte is %d\n", mask, buffer[ii]);
      printf("Looping ... \n");
    }

  char rightBit = 7 - endBit%8; /* This is how many bits should be empty on the right */
  printf("Right Bit = %d\n", rightBit);
  int jj; /* Now we shift bytes to right. In fact, we shift shorts to shift across arrays */
  typedef union UTrick_Def
  {
    unsigned short* temp;
    unsigned char* data;
  } UTrick;
  
  UTrick trick;
  unsigned short tempValue;
  if(sizeof(unsigned short) != 2)
    {
      printf("ALERT: Compatibility issues.\n");
      free(buffer);
      return NULL;
    }
  for(jj = bufferSize-2; jj >= 0; jj--)
    {
      printf("original buffer is %d\n", buffer[jj]);
      trick.temp = (unsigned short *) &buffer[jj];
      tempValue = *trick.temp; /* now tempValue has 2 bytes */
      /* Now temp points to a short in memory. LSB is at jj. MSB is at jj+1
	 abuse that :) */
      tempValue = (tempValue)>>rightBit; /* we shift to left because in little endian, MSB is on left */
      trick.temp = &tempValue;
      buffer[jj+1] = trick.data[1];
      printf("Copied a byte \n");
      printf("Data copied was %d\n", (unsigned char)trick.data[1]);
    }

  /* Now take care of the last byte */
  trick.temp = (unsigned short *) &buffer[0];
  tempValue = *trick.temp;
  tempValue = (tempValue)>>rightBit;
  trick.temp = &tempValue;
  buffer[0] = trick.data[0];
 
  
  /* Now swap byte order to make it big endian */
  for(jj = 0; jj < (bufferSize)/2; jj++)
    {
      unsigned char temp = buffer[jj];
      printf("Temp is %d and buffer is%d and jj is%d\n", temp, buffer[jj], jj);
      buffer[jj] = buffer[bufferSize-1-jj];
      buffer[bufferSize-1-jj] = temp;
    }
  return buffer;
}

void printSignalValues(struct MessageStruct* message)
{
  if(message == NULL) return;

  int ii;
  for(ii = 0; ii < message->signalNumber; ii++)
    {
      if(message->signals[ii] == NULL)
	{
	  printf("Error: Message is not set\n");
	  return;
	}
     
      struct SignalStruct *signal = message->signals[ii];
      if(message->length < signal->bitStart + signal->length)
	{
	  printf("Error: Signal dbc has signal outside the message boundary\n");
	  return;
	}
      /* imagine a signal starting from bit 7 to bit 17. That will require two bytes.
       */    
      
    }
}

void test()
{
  printf("testing\n");
  unsigned char* data;
  unsigned int val;
  data = &val;
  data[0] = 170;
  data[1] = 192;
  data[2] = 0;
  data[3] = 0;
  // val = 43712;
  struct MessageStruct message;
  message.length = 4;
  message.id = 13;
  message.data = data;
  int ii;
    for(ii = 0; ii < message.length; ii++)
    {
      printf("Byte %d is %d\n", (unsigned int) ii, data[ii]);
    }
    unsigned char* value = littleFronBigEndian(data, 0, 2);
  
  for(ii = 0; ii < 1; ii++)
    {
      printf("Byte %d is %d\n", (unsigned int) ii, value[ii]);
    }
  printf("Test data is %d\n", *((unsigned short*) value));
  
}


