OPTION _EXPLICIT

'$INCLUDE:'webpanim.bi'

Dim fileName As String
Dim ff As Integer
Dim fileSize As Long
Dim decHandle As _OFFSET
Dim rc As Long
Dim width As Long
Dim height As Long
Dim frameCount As Long
Dim loopCount As Long
Dim frameBytes As Long
Dim timeStampMs As Long
Dim frameIndex As Long

ReDim fileData(0) As _Unsigned _Byte
ReDim rgbaData(0) As _Unsigned _Byte
ReDim errData(255) As _Unsigned _Byte

fileName = "test.webp"

ff = FreeFile
Open fileName For Binary As #ff
fileSize = LOF(ff)
If fileSize <= 0 Then
    Close #ff
    Print "Empty file"
    End
End If

ReDim fileData(fileSize - 1) As _Unsigned _Byte
Get #ff, , fileData()
Close #ff

rc = wp_open_memory(_OFFSET(fileData(0)), fileSize, decHandle)
If rc <> WP_OK Then
    Print "wp_open_memory failed"
    End
End If

rc = wp_get_info(decHandle, width, height, frameCount, loopCount)
If rc <> WP_OK Then
    wp_get_last_error decHandle, _OFFSET(errData(0)), UBound(errData) + 1
    Print "wp_get_info failed: "; BytesToString(errData())
    wp_close decHandle
    End
End If

Print "Width      : "; width
Print "Height     : "; height
Print "Frame count: "; frameCount
Print "Loop count : "; loopCount

frameBytes = width * height * 4
ReDim rgbaData(frameBytes - 1) As _Unsigned _Byte

frameIndex = 0
Do
    rc = wp_get_next_rgba(decHandle, _OFFSET(rgbaData(0)), frameBytes, timeStampMs)

    If rc = WP_END Then
        Exit Do
    End If

    If rc <> WP_OK Then
        wp_get_last_error decHandle, _OFFSET(errData(0)), UBound(errData) + 1
        Print "Decode error: "; BytesToString(errData())
        Exit Do
    End If

    Print "Frame "; frameIndex; " timestamp(ms)="; timeStampMs; " bytes="; frameBytes
    frameIndex = frameIndex + 1
Loop

wp_close decHandle
Print "Done"
End

Function BytesToString$ (data() As _Unsigned _Byte)
    Dim i As Long
    Dim s As String

    s = ""
    For i = LBound(data) To UBound(data)
        If data(i) = 0 Then Exit For
        s = s + Chr$(data(i))
    Next i

    BytesToString$ = s
End Function
