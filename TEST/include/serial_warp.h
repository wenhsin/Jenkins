#include <stdio.h>
#include <stdlib.h>
#ifndef SERIAL_WARP_H
#define SERIAL_WARP_H

int serial_send(unsigned char *buf, int size);
int serial_recv(unsigned char *buf, int size);

void serial_init(void);
void serial_destroy(void);

#endif

