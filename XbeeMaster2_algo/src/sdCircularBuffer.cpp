#include "sdCircularBuffer.h"

//Private Functions -
static void sd_advance_pointer(volatile SD_CIRCULAR_BUF_T* cbuf)
{
	if(cbuf->full)
    {
        cbuf->tail = (cbuf->tail + 1) % cbuf->max;
    }
	cbuf->head = (cbuf->head + 1) % cbuf->max;
	// We mark full because we will advance tail on the next time around
	cbuf->full = (cbuf->head == cbuf->tail);
}

static void sd_retreat_pointer(volatile SD_CIRCULAR_BUF_T* cbuf)
{
	cbuf->full = false;
	cbuf->tail = (cbuf->tail + 1) % cbuf->max;
}

//funkcie API -
void sd_circular_buf_init(volatile SD_CIRCULAR_BUF_T* cbuf)
{
	cbuf->max = SD_CIRC_BUFFER_LEN;
	sd_circular_buf_reset(cbuf);
}

void sd_circular_buf_reset(volatile SD_CIRCULAR_BUF_T* cbuf)
{
    cbuf->head = 0;
    cbuf->tail = 0;
    cbuf->full = false;
}

size_t sd_circular_buf_size(volatile SD_CIRCULAR_BUF_T* cbuf)
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

size_t sd_circular_buf_capacity(volatile SD_CIRCULAR_BUF_T* cbuf)
{
	return cbuf->max;
}

void sd_circular_buf_put(volatile SD_CIRCULAR_BUF_T* cbuf, LOG data)
{
    cbuf->buffer[cbuf->head].timestamp = data.timestamp;
    cbuf->buffer[cbuf->head].num_sample_persec = data.num_sample_persec;
    cbuf->buffer[cbuf->head].accx[0] = data.accx[0];
    cbuf->buffer[cbuf->head].accx[1] = data.accx[1];
    cbuf->buffer[cbuf->head].accy[0] = data.accy[0];
    cbuf->buffer[cbuf->head].accy[1] = data.accy[1];
    cbuf->buffer[cbuf->head].accz[0] = data.accz[0];
    cbuf->buffer[cbuf->head].accz[1] = data.accz[1];

    cbuf->buffer[cbuf->head].gx[0] = data.gx[0];
    cbuf->buffer[cbuf->head].gx[1] = data.gx[1];
    cbuf->buffer[cbuf->head].gy[0] = data.gy[0];
    cbuf->buffer[cbuf->head].gy[1] = data.gy[1];
    cbuf->buffer[cbuf->head].gz[0] = data.gz[0];
    cbuf->buffer[cbuf->head].gz[1] = data.gz[1];

    cbuf->buffer[cbuf->head].magx[0] = data.magx[0];
    cbuf->buffer[cbuf->head].magx[1] = data.magx[1];
    cbuf->buffer[cbuf->head].magy[0] = data.magy[0];
    cbuf->buffer[cbuf->head].magy[1] = data.magy[1];
    cbuf->buffer[cbuf->head].magz[0] = data.magz[0];
    cbuf->buffer[cbuf->head].magz[1] = data.magz[1];
    sd_advance_pointer(cbuf);
}

int sd_circular_buf_put2(volatile SD_CIRCULAR_BUF_T* cbuf, volatile LOG *data)
{
    int r = -1;

    if(!sd_circular_buf_full(cbuf))
    {
        cbuf->buffer[cbuf->head].timestamp = data[0].timestamp;
        cbuf->buffer[cbuf->head].num_sample_persec = data[0].num_sample_persec;
        cbuf->buffer[cbuf->head].accx[0] = data[0].accx[0];
        cbuf->buffer[cbuf->head].accx[1] = data[0].accx[1];
        cbuf->buffer[cbuf->head].accy[0] = data[0].accy[0];
        cbuf->buffer[cbuf->head].accy[1] = data[0].accy[1];
        cbuf->buffer[cbuf->head].accz[0] = data[0].accz[0];
        cbuf->buffer[cbuf->head].accz[1] = data[0].accz[1];
    
        cbuf->buffer[cbuf->head].gx[0] = data[0].gx[0];
        cbuf->buffer[cbuf->head].gx[1] = data[0].gx[1];
        cbuf->buffer[cbuf->head].gy[0] = data[0].gy[0];
        cbuf->buffer[cbuf->head].gy[1] = data[0].gy[1];
        cbuf->buffer[cbuf->head].gz[0] = data[0].gz[0];
        cbuf->buffer[cbuf->head].gz[1] = data[0].gz[1];

        cbuf->buffer[cbuf->head].magx[0] = data[0].magx[0];
        cbuf->buffer[cbuf->head].magx[1] = data[0].magx[1];
        cbuf->buffer[cbuf->head].magy[0] = data[0].magy[0];
        cbuf->buffer[cbuf->head].magy[1] = data[0].magy[1];
        cbuf->buffer[cbuf->head].magz[0] = data[0].magz[0];
        cbuf->buffer[cbuf->head].magz[1] = data[0].magz[1];        
        sd_advance_pointer(cbuf);
        r = 0;
    }

    return r;
}

int sd_circular_buf_get(volatile SD_CIRCULAR_BUF_T* cbuf, volatile LOG * data)
{
    int r = -1;

    if(!sd_circular_buf_empty(cbuf))
    {
        data->timestamp = cbuf->buffer[cbuf->tail].timestamp;
        data->num_sample_persec = cbuf->buffer[cbuf->tail].num_sample_persec;
        data->accx[0] = cbuf->buffer[cbuf->tail].accx[0];
        data->accx[1] = cbuf->buffer[cbuf->tail].accx[1];
        data->accy[0] = cbuf->buffer[cbuf->tail].accy[0];
        data->accy[1] = cbuf->buffer[cbuf->tail].accy[1];
        data->accz[0] = cbuf->buffer[cbuf->tail].accz[0];
        data->accz[1] = cbuf->buffer[cbuf->tail].accz[1];

        data->gx[0] = cbuf->buffer[cbuf->tail].gx[0];
        data->gx[1] = cbuf->buffer[cbuf->tail].gx[1];
        data->gy[0] = cbuf->buffer[cbuf->tail].gy[0];
        data->gy[1] = cbuf->buffer[cbuf->tail].gy[1];
        data->gz[0] = cbuf->buffer[cbuf->tail].gz[0];
        data->gz[1] = cbuf->buffer[cbuf->tail].gz[1];

        data->magx[0] = cbuf->buffer[cbuf->tail].magx[0];
        data->magx[1] = cbuf->buffer[cbuf->tail].magx[1];
        data->magy[0] = cbuf->buffer[cbuf->tail].magy[0];
        data->magy[1] = cbuf->buffer[cbuf->tail].magy[1];
        data->magz[0] = cbuf->buffer[cbuf->tail].magz[0];
        data->magz[1] = cbuf->buffer[cbuf->tail].magz[1];
        sd_retreat_pointer(cbuf);

        r = 0;
    }

    return r;
}

bool sd_circular_buf_empty(volatile SD_CIRCULAR_BUF_T* cbuf)
{
    return (!cbuf->full && (cbuf->head == cbuf->tail));
}

bool sd_circular_buf_full(volatile SD_CIRCULAR_BUF_T* cbuf)
{
    return cbuf->full;
}