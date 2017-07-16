#ifndef INTERNAL_H
#define INTERNAL_H

#define DELETE_BUFFER(buffer)\
    if(buffer)\
    {\
        delete []buffer;\
        buffer = 0;\
    }
    
#endif
//! ccsdu2004
