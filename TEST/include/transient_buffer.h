#ifndef TRANSIENT_BUFFER_H
#define TRANSIENT_BUFFER_H

void transient_buffer_init(void);
void transient_buffer_destroy(void);
void write_transient_buffer(unsigned char data);
unsigned char read_transient_buffer(void);
void transient_buffer_reset(void);

#endif
