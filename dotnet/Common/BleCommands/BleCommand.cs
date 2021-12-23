using System;
using System.Diagnostics;
using System.IO;
using System.Reflection;

namespace FicusFactor.ReMousr.Common.BleCommands
{
    public static class BleCommand
    {
        public static byte[] Move(float speed, float hold, float angle)
        {
            return CreateCommand(BleMessage.RobotPose, BleCommandType.Move, speed, hold, angle);
        }

        public static byte[] ResetHeading()
        {
            return CreateCommand(BleMessage.RobotPose, BleCommandType.ResetHeading);
        }

        public static byte[] TailCalibration()
        {
            return CreateCommand(BleMessage.RobotPose, BleCommandType.TailCalibrationSignal);
        }

        public static byte[] Volume(uint level)
        {
            if (level > 100)
            {
                throw new ArgumentOutOfRangeException(nameof(level), $"Invalid level {level}. Cannot be greater than 100");
            }

            return CreateCommand(BleMessage.RobotPose, BleCommandType.SoundVolume, level);
        }

        public static byte[] GetDebugLog()
        {
            return CreateCommand(BleMessage.RobotPose, BleCommandType.GetDebugLog);
        }

        public static byte[] Stop()
        {
            return CreateCommand(BleMessage.RobotPose, BleCommandType.Stop);
        }

        public static byte[] Sleep()
        {
            return CreateCommand(BleMessage.RobotPose, BleCommandType.Sleep);
        }

        public static byte[] PowerOff()
        {
            return CreateCommand(BleMessage.RobotPose, BleCommandType.PowerOff);
        }

        private static byte[] CreateCommand(BleMessage message, BleCommandType commandType)
        {
            return CreateCommandRaw(message, 0F, 0F, 0F, commandType, false);
        }

        private static byte[] CreateCommand(BleMessage message, BleCommandType commandType, uint parameter)
        {
            return CreateCommandRaw(message, parameter, 0F, 0F, commandType, false);
        }

        private static byte[] CreateCommand(BleMessage message, BleCommandType commandType, float speed, float hold, float angle)
        {
            return CreateCommandRaw(message, speed, hold, angle, commandType, false);
        }

        private static byte[] CreateCommandRaw(params object[] parts)
        {
            MemoryStream ms = new MemoryStream();
            foreach (object o in parts)
            {
                byte[] b = null;
                if (o is float f)
                {
                    b = BitConverter.GetBytes(f);
                }
                else if (o is int i)
                {
                    b = BitConverter.GetBytes(i);
                }
                else if (o is uint u)
                {
                    b = BitConverter.GetBytes(u);
                }
                else if (o is byte bb)
                {
                    b = new[] { bb };
                }
                else if (o is BleMessage bm)
                {
                    b = new[] { (byte)bm };
                }
                else if (o is BleCommandType bc)
                {
                    b = new[] { (byte)bc };
                }
                else if (o is bool bl)
                {
                    b = BitConverter.GetBytes(bl);
                }
                else if (o is long l)
                {
                    b = BitConverter.GetBytes(l);
                }
                else if (o is ulong ul)
                {
                    b = BitConverter.GetBytes(ul);
                }
                else
                {
                    throw new NotSupportedException($"Unsupported type {o.GetType()} for command");
                }

                //Debug.WriteLine($"Adding bytes: {b.ToHexString()}");
                ms.Write(b, 0, b.Length);
            }

            byte[] data = ms.ToArray();
            Debug.WriteLine($"Command data: {data.ToHexString()}");
            return data;
        }
    }
}