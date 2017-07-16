using System;

namespace gaudio
{
    public enum GErrorCode
    {
        AUDIO_NO_ERROR = 0,  /**< Success */
        AUDIO_NOT_INIT,      /**< No initialized */
        AUDIO_BAD_TAG,       /**< Invalid input tag parameter */
        AUDIO_BAD_VALUE,     /**< Invalid input value parameter */
        AUDIO_NOT_SUPPORT,   /**< GAudio don't support this function*/
        AUDIO_BAD_OPERATION, /**< This operation is prohibited */
        AUDIO_OUT_OF_MEMORY, /**< Out of memory -- some malloc() failed. */
        AUDIO_INNER_ERROR    /**< GAudio internal error */
    }
}
