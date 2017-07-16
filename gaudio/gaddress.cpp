#include <string.h>
#include "gaudio.h"
#include "gcontext.h"
#include "streamBuffer.h"
#include "gresample.h"
#include "gqueue.h"

struct gFunctionStruct
{
    const char* name;
    void* address;
};

#define ADDR_WRAPPER(name,ptr) {name,(void*)ptr}

static gFunctionStruct  gFunctions[] =
{
    ADDR_WRAPPER("stream_tell",stream_tell),
    ADDR_WRAPPER("stream_seek",stream_seek),
    ADDR_WRAPPER("stream_read",stream_read),
    ADDR_WRAPPER("stream_file",stream_file),
    ADDR_WRAPPER("gresample_create",gresample_create),
    ADDR_WRAPPER("gresample_process",gresample_process),
    ADDR_WRAPPER("gresample_close",gresample_close),
    ADDR_WRAPPER("gqueue_create",gqueue_create),
    ADDR_WRAPPER("gqueue_destroy",gqueue_destroy),
    ADDR_WRAPPER("gqueue_resize",gqueue_resize),
    ADDR_WRAPPER("gqueue_process",gqueue_process),
    ADDR_WRAPPER("gaudio_init",gaudio_init),
    ADDR_WRAPPER("gaudio_deinit",gaudio_deinit),
    ADDR_WRAPPER("gaudio_is_support",gaudio_is_support),
    ADDR_WRAPPER("gaudio_set_float",gaudio_set_float),
    ADDR_WRAPPER("gaudio_get_float",gaudio_get_float),
    ADDR_WRAPPER("gaudio_set_int32",gaudio_set_int32),
    ADDR_WRAPPER("gaudio_get_int32",gaudio_get_int32),
    ADDR_WRAPPER("gaudio_set_string",gaudio_set_string),
    ADDR_WRAPPER("gaudio_get_string",gaudio_get_string),
    ADDR_WRAPPER("gaudio_error_get",gaudio_error_get),
    ADDR_WRAPPER("gaudio_listener_set_float3",gaudio_listener_set_float3),
    ADDR_WRAPPER("gaudio_listener_get_float3",gaudio_listener_get_float3),
    ADDR_WRAPPER("gaudio_source_create_from_file",gaudio_source_create_from_file),
    ADDR_WRAPPER("gaudio_source_create_from_buffer",gaudio_source_create_from_buffer),
    ADDR_WRAPPER("gaudio_source_create_from_virtual_io",gaudio_source_create_from_virtual_io),
    ADDR_WRAPPER("gaudio_source_create_from_buffer2",gaudio_source_create_from_buffer2),
    ADDR_WRAPPER("gaudio_source_destroy",gaudio_source_destroy),
    ADDR_WRAPPER("gaudio_source_set_user",gaudio_source_set_user),
    ADDR_WRAPPER("gaudio_source_get_user",gaudio_source_get_user),
    ADDR_WRAPPER("gaudio_source_set_finish_callback",gaudio_source_set_finish_callback),
    ADDR_WRAPPER("gaudio_source_set_error_callback",gaudio_source_set_error_callback),
    ADDR_WRAPPER("gaudio_source_set_float3",gaudio_source_set_float3),
    ADDR_WRAPPER("gaudio_source_get_float3",gaudio_source_get_float3),
    ADDR_WRAPPER("gaudio_source_play",gaudio_source_play),
    ADDR_WRAPPER("gaudio_source_play3",gaudio_source_play3),
    ADDR_WRAPPER("gaudio_source_pause",gaudio_source_pause),
    ADDR_WRAPPER("gaudio_source_stop",gaudio_source_stop),
    ADDR_WRAPPER("gaudio_source_seek",gaudio_source_seek),
    ADDR_WRAPPER("gaudio_source_set_position_callback",gaudio_source_set_position_callback),
    ADDR_WRAPPER("gaudio_source_set_float",gaudio_source_set_float),
    ADDR_WRAPPER("gaudio_source_get_float",gaudio_source_get_float),
    ADDR_WRAPPER("gaudio_source_set_int32",gaudio_source_set_int32),
    ADDR_WRAPPER("gaudio_source_get_int32",gaudio_source_get_int32),
    ADDR_WRAPPER("gaudio_source_get_string",gaudio_source_get_string),
    ADDR_WRAPPER("gaudio_effect_create",gaudio_effect_create),
    ADDR_WRAPPER("gaudio_effect_destroy",gaudio_effect_destroy),
    ADDR_WRAPPER("gaudio_effect_set_fft_callback",gaudio_effect_set_fft_callback),
    ADDR_WRAPPER("gaudio_effect_bind",gaudio_effect_bind),
    ADDR_WRAPPER("gaudio_effect_bind_to_source",gaudio_effect_bind_to_source),
    ADDR_WRAPPER("gaudio_effect_unbind",gaudio_effect_unbind),
    ADDR_WRAPPER("gaudio_effect_unbind_from_source",gaudio_effect_unbind_from_source),
    ADDR_WRAPPER("gaudio_effect_set_float",gaudio_effect_set_float),
    ADDR_WRAPPER("gaudio_effect_get_float",gaudio_effect_get_float),
    ADDR_WRAPPER("gaudio_effect_set_int32",gaudio_effect_set_int32),
    ADDR_WRAPPER("gaudio_effect_get_int32",gaudio_effect_get_int32),
    //ADDR_WRAPPER("getCurrentContext",getCurrentContext),
    {NULL,         (void*)NULL}
};

void* gaudio_address_get(const char* name)
{
    void* function = NULL;
    int32_t i = 0;

    if(name)
    {
        while(gFunctions[i].name && strcmp(gFunctions[i].name,name) != 0)
            i++;
        function = gFunctions[i].address;
    }
    else
        gerror_set(AUDIO_BAD_VALUE);

    return function;
}
