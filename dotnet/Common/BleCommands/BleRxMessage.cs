using System;
using System.Diagnostics;
using System.Text;

namespace FicusFactor.ReMousr.Common.BleCommands
{
    public abstract class BleRxMessage
    {
        public BleMessage MessageType
        {
            get;
        }

        public byte[] Raw
        {
            get;
        }

        internal BleRxMessage(byte[] data)
        {
            if (data is null)
            {
                throw new ArgumentNullException(nameof(data));
            }

            this.MessageType = (BleMessage)data[0];
            this.Raw = data;
        }

        public static BleRxMessage Parse(byte[] data)
        {
            if (data is null)
            {
                throw new ArgumentNullException(nameof(data));
            }

            BleRxMessage msg = null;
            switch ((BleMessage)data[0])
            {
                case BleMessage.RobotPose:
                    msg = new BleRxMessageRobotPose(data);
                    break;
                case BleMessage.BatteryVoltage:
                    msg = new BleRxMessageBatteryVoltage(data);
                    break;
                case BleMessage.CrashLogString:
                    msg = new BleRxMessageGetCrashLog(data);
                    break;
                case BleMessage.CrashLogFinish:
                default:
                    msg = new BleRxMessageUnknown(data);
                    break;
            }

            Debug.WriteLine($"Parsed {data.Length} bytes into message of type {msg.GetType().Name}");
            return msg;
        }

        public override string ToString()
        {
            return $"{MessageType} (0x{(int)MessageType:x2}): {this.Raw.ToHexString()}";
        }
    }
}