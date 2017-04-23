#include "rs232.h"
#include "serial_warp.h"
#include "file_warp.h"

int cport_nr = 1, bdrate=115200;
char mode[] = {'8','N','1',0};

int serial_send(unsigned char *buf, int size)
{
  return RS232_SendBuf(cport_nr, buf, size);
}

int serial_recv(unsigned char *buf, int size)
{
  return RS232_PollComport(cport_nr, buf, size);
}

void serial_init(void)
{
  if(RS232_OpenComport(cport_nr, bdrate, mode))
  {
    lprintf("Can not open comport\n");
    return;
  }
	
} 

void serial_destroy(void)
{
   RS232_CloseComport(cport_nr);
}

