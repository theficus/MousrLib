namespace FicusFactor.ReMousr.Common.BleCommands
{
    public enum BleCommandType : byte
    {
        Stop = 0x0,
        Spin = 0x1,
        Move = 0x2,
        ResetHeading = 0x3,
        GetDebugLog = 0x4,
        SpinPlan = 0x5,
        EnterDfuMode = 0x8,
        PowerOff = 0x9,
        Sleep = 0xa,
        ConfigAutoMode = 0xf,
        Chirp = 0x12,
        SoundVolume = 0x13,
        FlickSignal = 0x17,
        ReverseSignal = 0x18,
        TailCalibrationSignal = 0x19,
        SetTailSignal = 0x1a,
        InitializeDevice = 0x1c,
        FlipRobot = 0x1f,
        ConfigureDriverAssist = 0x29,
        TutorialStep = 0x2d,
        CommandSetTime = 0x2e,
        Invalid = 0x64
    }
}