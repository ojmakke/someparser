#include "lib.h"
#include "sender.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <net/if.h>
#include <sys/ioctl.h>

#include <linux/can.h>
#include <linux/can/raw.h>

void testSend()
{
  int s; /* can raw socket */ 
  int nbytes;
  struct sockaddr_can addr;
  struct can_frame frame;
  struct ifreq ifr;

  char* arg2 = "362#0020000000000000";
  /* parse CAN frame */

  memset(&frame, 0, sizeof(frame));
  frame.can_dlc = 8;
  frame.can_id = 866; //362 hex
  frame.data[0] = 0;
  frame.data[1] = 31;
  
  /* open socket */
  if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
    perror("socket");
    return;
  }

  addr.can_family = AF_CAN;

  const char* arg1 = "can0";
  strcpy(ifr.ifr_name, arg1);
  if (ioctl(s, SIOCGIFINDEX, &ifr) < 0) {
    perror("SIOCGIFINDEX");
    return;
  }
  addr.can_ifindex = ifr.ifr_ifindex;

  /* disable default receive filter on this RAW socket */
  /* This is obsolete as we do not read from the socket at all, but for */
  /* this reason we can remove the receive list in the Kernel to save a */
  /* little (really a very little!) CPU usage.                          */
  setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);

  if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("bind");
    return;
  }

  /* send frame */
  if ((nbytes = write(s, &frame, sizeof(frame))) != sizeof(frame)) {
    perror("write");
    return ;
  }

  //fprint_long_canframe(stdout, &frame, "\n", 0);

  close(s);

  return;
}

void sendCan(struct MessageStruct* canMsg)
{
  int s; /* can raw socket */ 
  int nbytes;
  struct sockaddr_can addr;
  struct can_frame frame;
  struct ifreq ifr;


  memset(&frame, 0, sizeof(frame));
  frame.can_dlc = canMsg->length;
  frame.can_id = canMsg->id;
  {
    int ii;
    for(ii = 0; ii < canMsg->length; ii++)
      frame.data[ii] = canMsg->data[ii];
  }
  
  /* open socket */
  if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
    perror("socket");
    return;
  }

  addr.can_family = AF_CAN;

  const char* arg1 = "can0";
  strcpy(ifr.ifr_name, arg1);
  if (ioctl(s, SIOCGIFINDEX, &ifr) < 0) {
    perror("SIOCGIFINDEX");
    return;
  }
  addr.can_ifindex = ifr.ifr_ifindex;

  /* disable default receive filter on this RAW socket */
  /* This is obsolete as we do not read from the socket at all, but for */
  /* this reason we can remove the receive list in the Kernel to save a */
  /* little (really a very little!) CPU usage.                          */
  setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);

  if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("bind");
    return;
  }

  /* send frame */
  if ((nbytes = write(s, &frame, sizeof(frame))) != sizeof(frame)) {
    perror("write");
    return ;
  }

  //fprint_long_canframe(stdout, &frame, "\n", 0);

  close(s);

  return;
}


