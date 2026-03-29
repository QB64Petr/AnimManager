$If WIN Then
    $If 32BIT Then
    Declare Dynamic Library "webpanim32"
    $Else
        Declare Dynamic Library "webpanim64"
        $End If

    $Else
DECLARE DYNAMIC LIBRARY "libwebpanim"
    $End If
    Function wp_open_memory& (ByVal srcData As _Offset, ByVal srcSize As Long, handle As _Offset)
    Function wp_get_info& (ByVal handle As _Offset, width As Long, height As Long, frameCount As Long, loopCount As Long)
    Function wp_get_next_rgba& (ByVal handle As _Offset, ByVal dstRgba As _Offset, ByVal dstSize As Long, timeStampMs As Long)
    Function wp_get_next_bgra& (ByVal handle As _Offset, ByVal dstBgra As _Offset, ByVal dstSize As Long, timeStampMs As Long)
    Function wp_reset& (ByVal handle As _Offset)
    Function wp_close& (ByVal handle As _Offset)
    Function wp_get_last_error& (ByVal handle As _Offset, ByVal dstText As _Offset, ByVal dstSize As Long)
End Declare

Const WP_OK = 1
Const WP_ERROR = 0
Const WP_END = 2
