#ifndef NETWORK_WARP_H
#define NETWORK_WARP_H

int network_init(void);
void network_destroy(void);
int network_send(char *buf, int size);
int network_recv(char *buf, int size);
#endif

