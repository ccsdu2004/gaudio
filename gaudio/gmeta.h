#ifndef G_META_H
#define G_META_H
#include "streamBuffer.h"

struct gmeta_t
{
    char    id3v1[128];
    int32_t id3v1length;
    char*   id3v2;
    int32_t id3v2length;
};

void gmeta_init(gmeta_t*);
void gmeta_deinit(gmeta_t*);

#define ID2_TITLE   0
#define ID2_ARTIST  1
#define ID2_ALBUM   2
#define ID2_YEAR    3
#define ID2_COMMENT 4
#define ID2_GENRE   5

int id2_meta_check(const char*,int,char*);

struct ID3Header
{
    char header[3];
    char ver;
    char revision;
    char flag;
    char size[4];
};

int32_t id3_header_size_get(streamBuffer* stream);

#endif
//! by duwenhua2013
