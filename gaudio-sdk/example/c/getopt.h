#pragma once
#ifdef __cplusplus
extern "C" {
#endif

extern int  opterr;    
extern int  optind;       
extern int  optopt;        
extern int  optreset;     
extern char *optarg;      
int getopt(int argc, char * const argv[], const char *optstring);

#ifdef __cplusplus
}    
#endif
