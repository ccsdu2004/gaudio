#include  <string.h>
#include  <stdlib.h>
#include "sfifo.h"
 
SFIFO_SCOPE int sfifo_init(sfifo_t *f, int size)
{
	memset(f, 0, sizeof(sfifo_t));

	if(size > SFIFO_MAX_BUFFER_SIZE)
		return -EINVAL;
 
	f->size = 1;
	for(; f->size <= size; f->size <<= 1)
		;
 
	if( 0 == (f->buffer = (char*)malloc(f->size)) )
		return -ENOMEM;

	return 0;
}
 
SFIFO_SCOPE void sfifo_close(sfifo_t *f)
{
	if(f->buffer) {
		free(f->buffer);
		f->buffer = NULL;	/* Prevent double free */
	}
}

/*
 * Empty FIFO buffer
 */
SFIFO_SCOPE void sfifo_flush(sfifo_t *f)
{
	/* Reset positions */
	f->readpos = 0;
	f->writepos = 0;
}

/*
 * Write bytes to a FIFO
 * Return number of bytes written, or an error code
 */
SFIFO_SCOPE int sfifo_write(sfifo_t *f, const void *_buf, int len)
{
	int total;
	int i;
	const char *buf = (const char *)_buf;

	if(!f->buffer)
		return -ENODEV;	/* No buffer! */

	/* total = len = min(space, len) */
	total = sfifo_space(f);
	//debug1("sfifo_space() = %d",total);
	if(len > total)
		len = total;
	else
		total = len;

	i = f->writepos;
	if(i + len > f->size)
	{
		memcpy(f->buffer + i, buf, f->size - i);
		buf += f->size - i;
		len -= f->size - i;
		i = 0;
	}
	memcpy(f->buffer + i, buf, len);
	f->writepos = i + len;

	return total;
}
 
SFIFO_SCOPE int sfifo_read(sfifo_t *f, void *_buf, int len)
{
	int total;
	int i;
	char *buf = (char *)_buf;

	if(!f->buffer)
		return -ENODEV;	 
 
	total = sfifo_used(f);
 
	if(len > total)
		len = total;
	else
		total = len;

	i = f->readpos;
	if(i + len > f->size)
	{
		memcpy(buf, f->buffer + i, f->size - i);
		buf += f->size - i;
		len -= f->size - i;
		i = 0;
	}
	memcpy(buf, f->buffer + i, len);
	f->readpos = i + len;

	return total;
}

