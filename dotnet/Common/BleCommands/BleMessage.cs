namespace FicusFactor.ReMousr.Common.BleCommands
{
    public enum BleMessage : byte
    {
        Unknown = 0,
        AutoAck = 0x0f,
        FirmwareVersion = 0x1c,
        HardwareVersion = 0x1d,
        InitDone = 0x1e,
        RobotPose = 0x30,
        ResetTailFailInfo = 0x32,
        StuckTofMessage = 0x40,
        CrashLogFinish = 0x5f,
        CrashLogString = 0x60,
        DebugInfo = 0x61,
        BatteryVoltage = 0x62,
        RobotStopped = 0x63,
        RcStuck = 0x64,
        Nack = 0xff
    }
}