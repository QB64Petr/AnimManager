FUNCTION BytesToString$ (datas() As _Unsigned _Byte)
    Dim i As Long
    Dim textValue As String

    textValue = ""

    For i = LBound(datas) To UBound(datas)
        If datas(i) = 0 Then Exit For
        textValue = textValue + Chr$(datas(i))
    Next i

    BytesToString$ = textValue
END FUNCTION

SUB ClockInit (clockState As WebPFrameClock)
    clockState.WrapOffset = 0
    clockState.LastRaw = TIMER
END SUB

FUNCTION ClockNow# (clockState As WebPFrameClock)
    Dim rawValue As Double
    Dim resultValue As Double

    rawValue = TIMER
    If rawValue < clockState.LastRaw Then
        clockState.WrapOffset = clockState.WrapOffset + 86400##
    End If

    clockState.LastRaw = rawValue
    resultValue = clockState.WrapOffset + rawValue
    ClockNow# = resultValue
END FUNCTION

SUB CopyBytesToImage (srcData() As _Unsigned _Byte, targetImage As Long)
    Dim byteCount As Long
    Dim sourceMem As _MEM
    Dim destMem As _MEM

    byteCount = UBound(srcData) - LBound(srcData) + 1
    sourceMem = _MEM(srcData(LBound(srcData)))
    destMem = _MEMIMAGE(targetImage)

    _MEMCOPY sourceMem, sourceMem.OFFSET, byteCount TO destMem, destMem.OFFSET

    _MEMFREE sourceMem
    _MEMFREE destMem
END SUB
