#include "gaudio.h"

void GAPIENTRY gaudio_init(const char* addons){}
void GAPIENTRY gaudio_deinit(){}
int32_t GAPIENTRY gaudio_is_support(int32_t param,int32_t value){return 0;}
void GAPIENTRY gaudio_set_float(int32_t param,float value){}
void GAPIENTRY gaudio_get_float(int32_t param,float* value){}
void GAPIENTRY gaudio_set_int32(int32_t param,int32_t value){}
void GAPIENTRY gaudio_get_int32(int32_t param,int32_t* value){}
void GAPIENTRY gaudio_set_string(int32_t param,const char* value){}
int32_t GAPIENTRY gaudio_get_string(int32_t param,char* value){return 0;}
int32_t GAPIENTRY gaudio_error_get(){return 0;}
void GAPIENTRY gaudio_listener_set_float3(int32_t param,float x,float y,float z){}
void GAPIENTRY gaudio_listener_get_float3(int32_t param,float* x,float* y,float* z){}
gsource* GAPIENTRY gaudio_source_create_from_file(const char* file,int32_t flag){return 0;}
gsource* GAPIENTRY gaudio_source_create_from_buffer(int8_t* buffer,uint32_t length){return 0;}
gsource* GAPIENTRY gaudio_source_create_from_virtual_io(gread read,gseek seek,gtell tell,gclose close,void* user){return 0;}
gsource* GAPIENTRY gaudio_source_create_from_buffer2(int32_t samplerate,int32_t channels,int32_t format,int8_t* pcm,int32_t length){return 0;}
void  GAPIENTRY gaudio_source_destroy(gsource* source){}
void  GAPIENTRY gaudio_source_set_user(gsource* source,void* user){}
void* GAPIENTRY gaudio_source_get_user(gsource* source){return 0;}
void GAPIENTRY gaudio_source_set_finish_callback(gsource* source,gfinish finish){}
void GAPIENTRY gaudio_source_set_error_callback(gsource* source,gerror error){}
void GAPIENTRY gaudio_source_set_float3(gsource* source,int32_t param,float x,float y,float z){}
void GAPIENTRY gaudio_source_get_float3(gsource* source,int32_t param,float* x,float* y,float* z){}
void GAPIENTRY gaudio_source_play(gsource* source,int32_t flag){}
void GAPIENTRY gaudio_source_play3(gsource* source,int32_t flag){}
void GAPIENTRY gaudio_source_pause(gsource* source){}
void GAPIENTRY gaudio_source_stop(gsource* source){}
void GAPIENTRY gaudio_source_seek(gsource* source,int32_t missecond){}
void GAPIENTRY gaudio_source_set_position_callback(gsource* source,gposition position){}
void GAPIENTRY gaudio_source_set_float(gsource* source,int32_t param,float value){}
void GAPIENTRY gaudio_source_get_float(gsource* source,int32_t param,float* value){}
void GAPIENTRY gaudio_source_set_int32(gsource* source,int32_t param,int32_t value){}
void GAPIENTRY gaudio_source_get_int32(gsource* source,int32_t param,int32_t* value){}
int32_t GAPIENTRY gaudio_source_get_string(gsource* source,int32_t param,char* value){return 0;}
geffect* GAPIENTRY gaudio_effect_create(int32_t effect,int32_t flag){return 0;}
void GAPIENTRY gaudio_effect_destroy(geffect* effect){}
void GAPIENTRY gaudio_effect_set_fft_callback(geffect*,gefxcb){}
void GAPIENTRY gaudio_effect_set_pcm_callback(geffect* efx,gefxpcmcb cb){}
void GAPIENTRY gaudio_effect_bind(geffect* effect,int32_t flag){}
void GAPIENTRY gaudio_effect_bind_to_source(geffect* effect,gsource* source){}
void GAPIENTRY gaudio_effect_unbind(geffect* effect){}
void GAPIENTRY gaudio_effect_unbind_from_source(geffect* effect,gsource* source){}
void GAPIENTRY gaudio_effect_set_float(geffect* effect,int32_t param,float value){}
void GAPIENTRY gaudio_effect_get_float(geffect* effect,int32_t param,float* value){}
void GAPIENTRY gaudio_effect_set_int32(geffect* effect,int32_t param,int32_t value){}
void GAPIENTRY gaudio_effect_get_int32(geffect* effect,int32_t param,int32_t* value){}
grecorder* GAPIENTRY gaudio_recorder_create(const char* filename,uint32_t type){return 0;}
void GAPIENTRY gaudio_recorder_write(grecorder* recorder){}
void GAPIENTRY gaudio_recorder_pause(grecorder* recorder){}
void GAPIENTRY gaudio_recorder_stop(grecorder* recorder){}
void GAPIENTRY gaudio_recorder_destroy(grecorder* recorder){}
void* GAPIENTRY gaudio_address_get(const char* name){return 0;}
