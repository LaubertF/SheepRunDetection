#include "PacketCircBuffer.h"

//Private Functions -
static void paAdvance_pointer(PACKET_CIRCULAR_BUF* cbuf)
{
	if(cbuf->full)
    {
        cbuf->tail = (cbuf->tail + 1) % cbuf->max;
    }
	cbuf->head = (cbuf->head + 1) % cbuf->max;
	// We mark full because we will advance tail on the next time around
	cbuf->full = (cbuf->head == cbuf->tail);
}

static void paRetreat_pointer(PACKET_CIRCULAR_BUF* cbuf)
{
	cbuf->full = false;
	cbuf->tail = (cbuf->tail + 1) % cbuf->max;
}

//funkcie API -
void paCircular_buf_init(PACKET_CIRCULAR_BUF* cbuf)
{
	cbuf->max = PACKET_CIRC_BUFFER_LEN;
	paCircular_buf_reset(cbuf);
}

void paCircular_buf_reset(PACKET_CIRCULAR_BUF* cbuf)
{
    cbuf->head = 0;
    cbuf->tail = 0;
    cbuf->full = false;
}

size_t paCircular_buf_size(PACKET_CIRCULAR_BUF* cbuf)
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

size_t paCircular_buf_capacity(PACKET_CIRCULAR_BUF* cbuf)
{
	return cbuf->max;
}

void paCircular_buf_put(PACKET_CIRCULAR_BUF* cbuf, Packet data)
{
    cbuf->buffer[cbuf->head].startDelimiter = data.startDelimiter;
    cbuf->buffer[cbuf->head].length[0] = data.length[0];
    cbuf->buffer[cbuf->head].length[1] = data.length[1];  
    cbuf->buffer[cbuf->head].frameID = data.frameID;
    for(int i = 0; i < data.getLength(); i++ ){
        cbuf->buffer[cbuf->head].frameData[i] = data.frameData[i];    
    }    
    cbuf->buffer[cbuf->head].crc = data.crc;
    paAdvance_pointer(cbuf);
}

int paCircular_buf_put2(PACKET_CIRCULAR_BUF* cbuf, Packet data)
{
    int r = -1;

    if(!paCircular_buf_full(cbuf))
    {
        cbuf->buffer[cbuf->head].startDelimiter = data.startDelimiter;
        cbuf->buffer[cbuf->head].length[0] = data.length[0];
        cbuf->buffer[cbuf->head].length[1] = data.length[1];  
        cbuf->buffer[cbuf->head].frameID = data.frameID;
        for(int i = 0; i < data.getLength(); i++ ){
            cbuf->buffer[cbuf->head].frameData[i] = data.frameData[i];    
        }    
        cbuf->buffer[cbuf->head].crc = data.crc;
        paAdvance_pointer(cbuf);
        r = 0;
    }

    return r;
}

int paCircular_buf_get(PACKET_CIRCULAR_BUF* cbuf, Packet* data)
{
    int r = -1;

    if(!paCircular_buf_empty(cbuf))
    {
        *data = cbuf->buffer[cbuf->tail];
        paRetreat_pointer(cbuf);

        r = 0;
    }

    return r;
}

bool paCircular_buf_empty(PACKET_CIRCULAR_BUF* cbuf)
{
    return (!cbuf->full && (cbuf->head == cbuf->tail));
}

bool paCircular_buf_full(PACKET_CIRCULAR_BUF* cbuf)
{
    return cbuf->full;
}