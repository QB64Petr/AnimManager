#ifndef WEBPANIM_H
#define WEBPANIM_H

#include <stdint.h>

#if defined(_WIN32) || defined(__CYGWIN__)
    #if defined(WEBPANIM_BUILD)
        #define WP_API __declspec(dllexport)
    #else
        #define WP_API __declspec(dllimport)
    #endif

    #if defined(_M_IX86) || defined(__i386__)
        #define WP_CALL __stdcall
    #else
        #define WP_CALL
    #endif
#else
    #define WP_API __attribute__((visibility("default")))
    #define WP_CALL
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Return codes used by this wrapper.
   1 = success
   0 = error
   2 = end of animation
*/

WP_API int32_t WP_CALL wp_open_memory(const void* src_data, int32_t src_size, void** handle);

WP_API int32_t WP_CALL wp_get_info(
    void* handle,
    int32_t* width,
    int32_t* height,
    int32_t* frame_count,
    int32_t* loop_count
);

/* Legacy slower path kept for compatibility with the second pack. */
WP_API int32_t WP_CALL wp_get_next_rgba(
    void* handle,
    void* dst_rgba,
    int32_t dst_size,
    int32_t* timestamp_ms
);

/* Fast path for QB64 32-bit _MEMIMAGE buffers: bytes are B,G,R,A in memory. */
WP_API int32_t WP_CALL wp_get_next_bgra(
    void* handle,
    void* dst_bgra,
    int32_t dst_size,
    int32_t* timestamp_ms
);

WP_API int32_t WP_CALL wp_reset(void* handle);

WP_API int32_t WP_CALL wp_close(void* handle);

WP_API int32_t WP_CALL wp_get_last_error(void* handle, void* dst_text, int32_t dst_size);

#ifdef __cplusplus
}
#endif

#endif