#include "webpanim.h"

#include <stdlib.h>
#include <string.h>

#include <webp/demux.h>

typedef struct WpAnim {
    uint8_t* owned_data;
    int32_t owned_size;
    WebPData webp_data;
    WebPAnimDecoder* decoder;
    WebPAnimInfo info;
    char last_error[256];
} WpAnim;

static void wp_set_error(WpAnim* anim, const char* text) {
    if (!anim) return;

    if (!text) {
        text = "unknown error";
    }

    strncpy(anim->last_error, text, sizeof(anim->last_error) - 1u);
    anim->last_error[sizeof(anim->last_error) - 1u] = '\0';
}

static void wp_copy_text(char* dst, int32_t dst_size, const char* src) {
    size_t src_len;
    size_t copy_len;

    if (!dst || dst_size <= 0) return;

    if (!src) {
        src = "unknown error";
    }

    src_len = strlen(src);
    copy_len = (size_t)(dst_size - 1);
    if (copy_len > src_len) {
        copy_len = src_len;
    }

    if (copy_len > 0u) {
        memcpy(dst, src, copy_len);
    }
    dst[copy_len] = '\0';
}

static int32_t wp_get_frame_byte_size(const WpAnim* anim, size_t* frame_byte_size) {
    size_t width;
    size_t height;
    size_t total;

    if (!anim || !frame_byte_size) {
        return 0;
    }

    width = (size_t)anim->info.canvas_width;
    height = (size_t)anim->info.canvas_height;

    if (width == 0u || height == 0u) {
        return 0;
    }

    total = width * height;
    if (total / width != height) {
        return 0;
    }

    if (total > ((size_t)-1) / 4u) {
        return 0;
    }

    *frame_byte_size = total * 4u;
    return 1;
}

static int32_t wp_decode_next_frame(
    WpAnim* anim,
    uint8_t** frame_bgra,
    int32_t* timestamp_ms,
    size_t* frame_byte_size
) {
    int timestamp_local;

    if (!anim || !frame_bgra || !frame_byte_size) {
        return 0;
    }

    if (!WebPAnimDecoderHasMoreFrames(anim->decoder)) {
        return 2;
    }

    if (!wp_get_frame_byte_size(anim, frame_byte_size)) {
        wp_set_error(anim, "invalid frame byte size");
        return 0;
    }

    timestamp_local = 0;
    if (!WebPAnimDecoderGetNext(anim->decoder, frame_bgra, &timestamp_local)) {
        wp_set_error(anim, "WebPAnimDecoderGetNext failed");
        return 0;
    }

    if (timestamp_ms) {
        *timestamp_ms = timestamp_local;
    }

    return 1;
}

static int32_t wp_validate_dst_size(WpAnim* anim, int32_t dst_size, size_t needed_size) {
    if ((size_t)dst_size < needed_size) {
        wp_set_error(anim, "destination buffer too small");
        return 0;
    }
    return 1;
}

WP_API int32_t WP_CALL wp_open_memory(const void* src_data, int32_t src_size, void** handle) {
    WpAnim* anim;
    WebPAnimDecoderOptions options;

    if (!src_data || src_size <= 0 || !handle) {
        return 0;
    }

    *handle = NULL;

    anim = (WpAnim*)calloc(1u, sizeof(WpAnim));
    if (!anim) {
        return 0;
    }

    anim->owned_data = (uint8_t*)malloc((size_t)src_size);
    if (!anim->owned_data) {
        free(anim);
        return 0;
    }

    memcpy(anim->owned_data, src_data, (size_t)src_size);
    anim->owned_size = src_size;
    anim->webp_data.bytes = anim->owned_data;
    anim->webp_data.size = (size_t)anim->owned_size;

    if (!WebPAnimDecoderOptionsInit(&options)) {
        wp_set_error(anim, "WebPAnimDecoderOptionsInit failed");
        wp_close(anim);
        return 0;
    }

    options.color_mode = MODE_BGRA;
    options.use_threads = 1;

    anim->decoder = WebPAnimDecoderNew(&anim->webp_data, &options);
    if (!anim->decoder) {
        wp_set_error(anim, "WebPAnimDecoderNew failed");
        wp_close(anim);
        return 0;
    }

    if (!WebPAnimDecoderGetInfo(anim->decoder, &anim->info)) {
        wp_set_error(anim, "WebPAnimDecoderGetInfo failed");
        wp_close(anim);
        return 0;
    }

    *handle = anim;
    return 1;
}

WP_API int32_t WP_CALL wp_get_info(
    void* handle,
    int32_t* width,
    int32_t* height,
    int32_t* frame_count,
    int32_t* loop_count
) {
    WpAnim* anim;

    anim = (WpAnim*)handle;
    if (!anim) {
        return 0;
    }

    if (width) {
        *width = (int32_t)anim->info.canvas_width;
    }
    if (height) {
        *height = (int32_t)anim->info.canvas_height;
    }
    if (frame_count) {
        *frame_count = (int32_t)anim->info.frame_count;
    }
    if (loop_count) {
        *loop_count = (int32_t)anim->info.loop_count;
    }

    return 1;
}

WP_API int32_t WP_CALL wp_get_next_rgba(
    void* handle,
    void* dst_rgba,
    int32_t dst_size,
    int32_t* timestamp_ms
) {
    WpAnim* anim;
    uint8_t* frame_bgra;
    uint8_t* dst;
    size_t needed_size;
    size_t i;
    int32_t rc;

    anim = (WpAnim*)handle;
    if (!anim || !dst_rgba) {
        return 0;
    }

    rc = wp_decode_next_frame(anim, &frame_bgra, timestamp_ms, &needed_size);
    if (rc != 1) {
        return rc;
    }

    if (!wp_validate_dst_size(anim, dst_size, needed_size)) {
        return 0;
    }

    dst = (uint8_t*)dst_rgba;
    for (i = 0u; i < needed_size; i += 4u) {
        dst[i + 0u] = frame_bgra[i + 2u];
        dst[i + 1u] = frame_bgra[i + 1u];
        dst[i + 2u] = frame_bgra[i + 0u];
        dst[i + 3u] = frame_bgra[i + 3u];
    }

    return 1;
}

WP_API int32_t WP_CALL wp_get_next_bgra(
    void* handle,
    void* dst_bgra,
    int32_t dst_size,
    int32_t* timestamp_ms
) {
    WpAnim* anim;
    uint8_t* frame_bgra;
    size_t needed_size;
    int32_t rc;

    anim = (WpAnim*)handle;
    if (!anim || !dst_bgra) {
        return 0;
    }

    rc = wp_decode_next_frame(anim, &frame_bgra, timestamp_ms, &needed_size);
    if (rc != 1) {
        return rc;
    }

    if (!wp_validate_dst_size(anim, dst_size, needed_size)) {
        return 0;
    }

    memcpy(dst_bgra, frame_bgra, needed_size);
    return 1;
}

WP_API int32_t WP_CALL wp_reset(void* handle) {
    WpAnim* anim;

    anim = (WpAnim*)handle;
    if (!anim) {
        return 0;
    }

    WebPAnimDecoderReset(anim->decoder);
    return 1;
}

WP_API int32_t WP_CALL wp_close(void* handle) {
    WpAnim* anim;

    anim = (WpAnim*)handle;
    if (!anim) {
        return 0;
    }

    if (anim->decoder) {
        WebPAnimDecoderDelete(anim->decoder);
        anim->decoder = NULL;
    }

    if (anim->owned_data) {
        free(anim->owned_data);
        anim->owned_data = NULL;
    }

    free(anim);
    return 1;
}

WP_API int32_t WP_CALL wp_get_last_error(void* handle, void* dst_text, int32_t dst_size) {
    WpAnim* anim;

    if (!dst_text || dst_size <= 0) {
        return 0;
    }

    anim = (WpAnim*)handle;
    if (!anim) {
        wp_copy_text((char*)dst_text, dst_size, "invalid handle");
        return 1;
    }

    if (anim->last_error[0] == '\0') {
        wp_copy_text((char*)dst_text, dst_size, "no error");
    } else {
        wp_copy_text((char*)dst_text, dst_size, anim->last_error);
    }

    return 1;
}
