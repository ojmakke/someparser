#ifndef AUTOGEN_H
#define AUTOGEN_H

extern struct AllMessages allMessages;

extern void printAllInfo();
extern void init();
extern void clean();
extern void sortById();
extern void test();
extern struct MessageStruct *getMessage(unsigned int messageID);
extern void printSignalValues(struct MessageStruct* message);


extern void addMessage(char* name,
		       char* source,
		       unsigned int id,
		       unsigned int length);

extern void addSignal(char* name,
		      unsigned int bitStart,
		      unsigned int length,
		      float offset,
		      float scale,
		      unsigned char isSigned,
		      unsigned char isLittleEndian);

		       
		       

struct SignalStruct
{
  unsigned int bitStart;
  unsigned int length;
  float offset;
  float scale;
  char *signalName;
  unsigned char isSigned;
  unsigned char isLittleEndian;
  
};

struct MessageStruct
{
  struct SignalStruct **signals;
  unsigned int signalNumber;
  unsigned int length;
  char *messageName;
  char *sender;
  unsigned char* data;
  unsigned int id;
};

struct AllMessages
{
  unsigned int messageCount;
  struct MessageStruct **messages;
};


#endif
