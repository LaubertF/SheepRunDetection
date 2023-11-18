#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define CIRC_BUFFER_LEN 64

// The definition of our circular buffer structure
typedef struct circular_buf_t {
	uint8_t buffer[CIRC_BUFFER_LEN];
	size_t head;
	size_t tail;
	size_t max; //of the buffer
	bool full;
}CIRCULAR_BUF_T;

/// Pass in a storage buffer and size, returns a circular buffer handle
/// Requires: buffer is not NULL, size > 0
/// Ensures: cbuf has been created and is returned in an empty state
void circular_buf_init(CIRCULAR_BUF_T *cbuf);

/// Reset the circular buffer to empty, head == tail. Data not cleared
/// Requires: cbuf is valid and created by circular_buf_init
void circular_buf_reset(CIRCULAR_BUF_T *cbuf);

/// Put version 1 continues to add data if the buffer is full
/// Old data is overwritten
/// Requires: cbuf is valid and created by circular_buf_init
void circular_buf_put(CIRCULAR_BUF_T *cbuf, uint8_t data);

/// Put Version 2 rejects new data if the buffer is full
/// Requires: cbuf is valid and created by circular_buf_init
/// Returns 0 on success, -1 if buffer is full
int circular_buf_put2(CIRCULAR_BUF_T *cbuf, uint8_t data);

/// Retrieve a value from the buffer
/// Requires: cbuf is valid and created by circular_buf_init
/// Returns 0 on success, -1 if the buffer is empty
int circular_buf_get(CIRCULAR_BUF_T *cbuf, uint8_t* data);

/// CHecks if the buffer is empty
/// Requires: cbuf is valid and created by circular_buf_init
/// Returns true if the buffer is empty
bool circular_buf_empty(CIRCULAR_BUF_T *cbuf);

/// Checks if the buffer is full
/// Requires: cbuf is valid and created by circular_buf_init
/// Returns true if the buffer is full
bool circular_buf_full(CIRCULAR_BUF_T *cbuf);

/// Check the capacity of the buffer
/// Requires: cbuf is valid and created by circular_buf_init
/// Returns the maximum capacity of the buffer
size_t circular_buf_capacity(CIRCULAR_BUF_T *cbuf);

/// Check the number of elements stored in the buffer
/// Requires: cbuf is valid and created by circular_buf_init
/// Returns the current number of elements in the buffer
size_t circular_buf_size(CIRCULAR_BUF_T *cbuf);

//TODO: int circular_buf_get_range(circular_buf_t cbuf, uint8_t *data, size_t len);
//TODO: int circular_buf_put_range(circular_buf_t cbuf, uint8_t * data, size_t len);