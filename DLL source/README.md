# webpanim - third pack

This third pack keeps the original memory-based loader but adds a faster path for QB64PE 32-bit images.

## What changed vs second pack

- The decoder is now created with `MODE_BGRA`.
- New exported function: `wp_get_next_bgra()`.
- Old `wp_get_next_rgba()` is still present for compatibility, but it is now the slower legacy path.
- The QB64PE demo no longer converts every pixel with `_RGBA32(...)`.
- The new demo copies the decoded BGRA bytes directly into a 32-bit QB64PE image using `_MEMIMAGE` + `_MEMCOPY`.

## Why this is faster

QB64PE 32-bit image memory uses 4 bytes per pixel in Blue, Green, Red, Alpha byte order.
`libwebp` animated decoding supports `MODE_BGRA`, so the wrapper can ask libwebp for bytes in the same order that QB64PE wants in `_MEMIMAGE`.
That removes the expensive BASIC-side per-pixel conversion loop.

## Public API

The wrapper exports:

- `wp_open_memory`
- `wp_get_info`
- `wp_get_next_rgba` (legacy slower path)
- `wp_get_next_bgra` (fast QB64PE path)
- `wp_reset`
- `wp_close`
- `wp_get_last_error`

## QB64PE files

- `qb64/webpanim.bi`
- `qb64/webpanim_helper_decl.bi`
- `qb64/webpanim_helper_impl.bi`
- `qb64/demo_webpanim_player.bas`

Put the right shared library next to your compiled QB64PE program:

- Windows 32/64-bit: `webpanim.dll`
- Linux 64-bit: `libwebpanim.so`
- macOS: `libwebpanim.dylib`

## Build notes

### Linux/macOS using system libwebp

```bash
cmake -S . -B build
cmake --build build --config Release
```

### Using bundled libwebp source

```bash
cmake -S . -B build -DWEBPANIM_USE_BUNDLED_LIBWEBP=ON
cmake --build build --config Release
```

### Windows examples

Visual Studio generator:

```bash
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

Win32 build:

```bash
cmake -S . -B build32 -G "Visual Studio 17 2022" -A Win32
cmake --build build32 --config Release
```

## Honest warning

The C wrapper is real and buildable. The QB64PE demo logic is designed to match QB64PE memory rules, but I did not compile the BASIC source in QB64PE inside this environment.
So the pack is a strong first working candidate, not a lie about full cross-platform proof.
