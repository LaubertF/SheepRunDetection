#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "Packet.h"

#ifndef PACKETCIRCBUFFER_H
#define PACKETCIRCBUFFER_H

#define PACKET_CIRC_BUFFER_LEN 4

// The definition of our circular buffer structure
typedef struct packetCircularBuf {
	Packet buffer[PACKET_CIRC_BUFFER_LEN];
	size_t head;
	size_t tail;
	size_t max; //of the buffer
	bool full;
}PACKET_CIRCULAR_BUF;

/// Pass in a storage buffer and size, returns a circular buffer handle
/// Requires: buffer is not NULL, size > 0
/// Ensures: cbuf has been created and is returned in an empty state
void paCircular_buf_init(PACKET_CIRCULAR_BUF *cbuf);

/// Reset the circular buffer to empty, head == tail. Data not cleared
/// Requires: cbuf is valid and created by circular_buf_init
void paCircular_buf_reset(PACKET_CIRCULAR_BUF *cbuf);

/// Put version 1 continues to add data if the buffer is full
/// Old data is overwritten
/// Requires: cbuf is valid and created by circular_buf_init
void paCircular_buf_put(PACKET_CIRCULAR_BUF *cbuf, Packet data);

/// Put Version 2 rejects new data if the buffer is full
/// Requires: cbuf is valid and created by circular_buf_init
/// Returns 0 on success, -1 if buffer is full
int paCircular_buf_put2(PACKET_CIRCULAR_BUF *cbuf, Packet data);

/// Retrieve a value from the buffer
/// Requires: cbuf is valid and created by circular_buf_init
/// Returns 0 on success, -1 if the buffer is empty
int paCircular_buf_get(PACKET_CIRCULAR_BUF *cbuf, Packet* data);

/// CHecks if the buffer is empty
/// Requires: cbuf is valid and created by circular_buf_init
/// Returns true if the buffer is empty
bool paCircular_buf_empty(PACKET_CIRCULAR_BUF *cbuf);

/// Checks if the buffer is full
/// Requires: cbuf is valid and created by circular_buf_init
/// Returns true if the buffer is full
bool paCircular_buf_full(PACKET_CIRCULAR_BUF *cbuf);

/// Check the capacity of the buffer
/// Requires: cbuf is valid and created by circular_buf_init
/// Returns the maximum capacity of the buffer
size_t paCircular_buf_capacity(PACKET_CIRCULAR_BUF *cbuf);

/// Check the number of elements stored in the buffer
/// Requires: cbuf is valid and created by circular_buf_init
/// Returns the current number of elements in the buffer
size_t paCircular_buf_size(PACKET_CIRCULAR_BUF *cbuf);

//TODO: int circular_buf_get_range(circular_buf_t cbuf, uint8_t *data, size_t len);
//TODO: int circular_buf_put_range(circular_buf_t cbuf, uint8_t * data, size_t len);
#endif //PACKETCIRCBUFFER_H