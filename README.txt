Unified Anim manager for QB64PE

Files:
- anim_manager.bas            main dispatcher with public Anim... API
- anim_apng_backend.bi        APNG backend
- anim_gif89a_backend.bi      GIF89a backend
- anim_flic_backend.bi        FLI/FLC backend
- anim_webp_backend.bi        WebP backend
- webpanim32.dll              WebP runtime for 32-bit Windows
- webpanim64.dll              WebP runtime for 64-bit Windows
- libwebpanim.so              WebP runtime for Linux

Use:
Compile anim_manager.bas as the project main BAS together with the included backend files
in the same folder. Call AnimUpdate or AnimUpdateAll in your main loop.

Important:
- AnimDraw draws in native size.
- AnimDrawWindow stretches to the target rectangle without preserving aspect ratio.
- FLI/FLC backend is multi-instance safe, but it rebuilds frames on demand, so saving
  or reconstructing arbitrary frames is correct but slower than a cached design.
- For WebP, keep the correct DLL/SO next to the compiled program or in a location the OS loader can find.

Typical calls:
anim1 = AnimOpen("test.webp")
anim2 = AnimOpen("sele.gif")
anim3 = AnimOpen("bart.flc")

AnimStart anim1
AnimStart anim2
AnimStart anim3

Do
    _Limit 240
    AnimUpdateAll
    AnimDraw 100, 100, anim1
    AnimDrawWindow 100, 100, 350, 440, anim2
Loop Until _KeyHit = 27

AnimFreeAll
