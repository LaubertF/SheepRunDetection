#include "circular_buffer.h"

//Private Functions -
static void advance_pointer(CIRCULAR_BUF_T* cbuf)
{
	if(cbuf->full)
    {
        cbuf->tail = (cbuf->tail + 1) % cbuf->max;
    }
	cbuf->head = (cbuf->head + 1) % cbuf->max;
	// We mark full because we will advance tail on the next time around
	cbuf->full = (cbuf->head == cbuf->tail);
}

static void retreat_pointer(CIRCULAR_BUF_T* cbuf)
{
	cbuf->full = false;
	cbuf->tail = (cbuf->tail + 1) % cbuf->max;
}

//funkcie API -
void circular_buf_init(CIRCULAR_BUF_T* cbuf)
{
	cbuf->max = CIRC_BUFFER_LEN;
	circular_buf_reset(cbuf);
}

void circular_buf_reset(CIRCULAR_BUF_T* cbuf)
{
    cbuf->head = 0;
    cbuf->tail = 0;
    cbuf->full = false;
}

size_t circular_buf_size(CIRCULAR_BUF_T* cbuf)
{
	size_t size = cbuf->max;

	if(!cbuf->full)
	{
		if(cbuf->head >= cbuf->tail)
		{
			size = (cbuf->head - cbuf->tail);
		}
		else
		{
			size = (cbuf->max + cbuf->head - cbuf->tail);
		}
	}

	return size;
}

size_t circular_buf_capacity(CIRCULAR_BUF_T* cbuf)
{
	return cbuf->max;
}

void circular_buf_put(CIRCULAR_BUF_T* cbuf, uint8_t data)
{
    cbuf->buffer[cbuf->head] = data;
    advance_pointer(cbuf);
}

int circular_buf_put2(CIRCULAR_BUF_T* cbuf, uint8_t data)
{
    int r = -1;

    if(!circular_buf_full(cbuf))
    {
        cbuf->buffer[cbuf->head] = data;
        advance_pointer(cbuf);
        r = 0;
    }

    return r;
}

int circular_buf_get(CIRCULAR_BUF_T* cbuf, uint8_t * data)
{
    int r = -1;

    if(!circular_buf_empty(cbuf))
    {
        *data = cbuf->buffer[cbuf->tail];
        retreat_pointer(cbuf);

        r = 0;
    }

    return r;
}

bool circular_buf_empty(CIRCULAR_BUF_T* cbuf)
{
    return (!cbuf->full && (cbuf->head == cbuf->tail));
}

bool circular_buf_full(CIRCULAR_BUF_T* cbuf)
{
    return cbuf->full;
}