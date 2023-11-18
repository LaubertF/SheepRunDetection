#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define SD_CIRC_BUFFER_LEN 50

// Definicia jednej vzorky ukladanej na SD kartu
typedef struct log{
	uint32_t timestamp;
	uint16_t num_sample_persec;
	uint8_t accx[2];
	uint8_t accy[2];
	uint8_t accz[2];
	uint8_t gx[2];
	uint8_t gy[2];
	uint8_t gz[2];
	uint8_t magx[2];
	uint8_t magy[2];
	uint8_t magz[2];
}LOG;

// The definition of our circular buffer structure
typedef struct sd_circular_buf_t {
	LOG buffer[SD_CIRC_BUFFER_LEN];
	size_t head;
	size_t tail;
	size_t max; //of the buffer
	bool full;
}SD_CIRCULAR_BUF_T;



/// Pass in a storage buffer and size, returns a circular buffer handle
/// Requires: buffer is not NULL, size > 0
/// Ensures: cbuf has been created and is returned in an empty state
void sd_circular_buf_init(volatile SD_CIRCULAR_BUF_T *cbuf);

/// Reset the circular buffer to empty, head == tail. Data not cleared
/// Requires: cbuf is valid and created by circular_buf_init
void sd_circular_buf_reset(volatile SD_CIRCULAR_BUF_T *cbuf);

/// Put version 1 continues to add data if the buffer is full
/// Old data is overwritten
/// Requires: cbuf is valid and created by circular_buf_init
void sd_circular_buf_put(volatile SD_CIRCULAR_BUF_T *cbuf, LOG data);

/// Put Version 2 rejects new data if the buffer is full
/// Requires: cbuf is valid and created by circular_buf_init
/// Returns 0 on success, -1 if buffer is full
int sd_circular_buf_put2(volatile SD_CIRCULAR_BUF_T *cbuf, volatile LOG *data);

/// Retrieve a value from the buffer
/// Requires: cbuf is valid and created by circular_buf_init
/// Returns 0 on success, -1 if the buffer is empty
int sd_circular_buf_get(volatile SD_CIRCULAR_BUF_T *cbuf, volatile LOG* data);

/// CHecks if the buffer is empty
/// Requires: cbuf is valid and created by circular_buf_init
/// Returns true if the buffer is empty
bool sd_circular_buf_empty(volatile SD_CIRCULAR_BUF_T *cbuf);

/// Checks if the buffer is full
/// Requires: cbuf is valid and created by circular_buf_init
/// Returns true if the buffer is full
bool sd_circular_buf_full(volatile SD_CIRCULAR_BUF_T *cbuf);

/// Check the capacity of the buffer
/// Requires: cbuf is valid and created by circular_buf_init
/// Returns the maximum capacity of the buffer
size_t sd_circular_buf_capacity(volatile SD_CIRCULAR_BUF_T *cbuf);

/// Check the number of elements stored in the buffer
/// Requires: cbuf is valid and created by circular_buf_init
/// Returns the current number of elements in the buffer
size_t sd_circular_buf_size(volatile SD_CIRCULAR_BUF_T *cbuf);

//TODO: int circular_buf_get_range(circular_buf_t cbuf, uint8_t *data, size_t len);
//TODO: int circular_buf_put_range(circular_buf_t cbuf, uint8_t * data, size_t len);