Option _Explicit

'$Include:'webpanim.bi'
'$Include:'webpanim_helper_decl.bi'

Dim fileName As String
Dim ff As Integer
Dim fileSize As Long
Dim decHandle As _Offset
Dim rc As Long
Dim canvasWidth As Long
Dim canvasHeight As Long
Dim frameCount As Long
Dim loopCount As Long
Dim frameBytes As Long
Dim stampMs As Long
Dim currentLoop As Long
Dim shouldQuit As Long
Dim keyCode As Long
Dim screenHandle As Long
Dim frameImage As Long
Dim clockState As WebPFrameClock
Dim animationStart As Double
Dim targetMoment As Double
Dim nowValue As Double
Dim errText As String
Dim N As Long


ReDim fileData(0) As _Unsigned _Byte
ReDim bgraData(0) As _Unsigned _Byte
ReDim errData(255) As _Unsigned _Byte

ClockInit clockState
fileName = "firework.webp"

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

rc = wp_open_memory(_Offset(fileData(0)), fileSize, decHandle)
If rc <> WP_OK Then
    Print "wp_open_memory failed"
    End
End If

rc = wp_get_info(decHandle, canvasWidth, canvasHeight, frameCount, loopCount)
If rc <> WP_OK Then
    N = wp_get_last_error(decHandle, _Offset(errData(0)), UBound(errData) + 1)
    errText = BytesToString$(errData())
    Print "wp_get_info failed: "; errText
    N = wp_close(decHandle)
    End
End If

If canvasWidth <= 0 Or canvasHeight <= 0 Then
    Print "Invalid canvas size"
    N = wp_close(decHandle)
    End
End If

frameBytes = canvasWidth * canvasHeight * 4
ReDim bgraData(frameBytes - 1) As _Unsigned _Byte

screenHandle = _NewImage(canvasWidth, canvasHeight, 32)
If screenHandle > -2 Then
    Print "Could not create screen image"
    N = wp_close(decHandle)
    End
End If

Screen screenHandle
_Title "WebP animation player v3 - any key exits"
'_DontBlend
'Cls
'_Display

frameImage = _NewImage(canvasWidth, canvasHeight, 32)
If frameImage > -2 Then
    Print "Could not create frame image"
    N = wp_close(decHandle)
    End
End If

_Dest frameImage
'_DontBlend
'Cls
_Dest 0

animationStart = ClockNow#(clockState)
currentLoop = 0
shouldQuit = 0

Do
    rc = wp_get_next_bgra(decHandle, _Offset(bgraData(0)), frameBytes, stampMs)

    If rc = WP_END Then
        If loopCount = 0 Or currentLoop + 1 < loopCount Then
            rc = wp_reset(decHandle)
            If rc <> WP_OK Then
                N = wp_get_last_error(decHandle, _Offset(errData(0)), UBound(errData) + 1)
                errText = BytesToString$(errData())
                Print "wp_reset failed: "; errText
                Exit Do
            End If

            currentLoop = currentLoop + 1
            animationStart = ClockNow#(clockState)
        Else
            Exit Do
        End If
    ElseIf rc <> WP_OK Then
        N = wp_get_last_error(decHandle, _Offset(errData(0)), UBound(errData) + 1)
        errText = BytesToString$(errData())
        Print "Decode error: "; errText
        Exit Do
    Else

        CopyBytesToImage bgraData(), frameImage
        Cls
        _PutImage (0, 0), frameImage
        _Display


        targetMoment = animationStart + stampMs / 1000##
        Do
            keyCode = _KeyHit
            If keyCode <> 0 Then
                shouldQuit = -1
                Exit Do
            End If

            nowValue = ClockNow#(clockState)
            If nowValue >= targetMoment Then Exit Do
            _Limit 240
        Loop
    End If
Loop Until shouldQuit

N = wp_close(decHandle)
_FreeImage frameImage
End

'$Include:'webpanim_helper_impl.bi'
