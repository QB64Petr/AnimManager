TYPE WebPFrameClock
    WrapOffset As Double
    LastRaw As Double
END TYPE

DECLARE FUNCTION BytesToString$ (data() As _Unsigned _Byte)
DECLARE SUB ClockInit (clockState As WebPFrameClock)
DECLARE FUNCTION ClockNow# (clockState As WebPFrameClock)
DECLARE SUB CopyBytesToImage (srcData() As _Unsigned _Byte, targetImage As Long)
