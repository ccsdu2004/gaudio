#ifndef GID3V1TAG_H
#define GID3V1TAG_H

struct id3v1tag
{
    char tag[3];
    char title[30];
    char artist[30];
    char album[30];
    char year[4];
    char comment[30];
    unsigned char genre;
};

#endif
