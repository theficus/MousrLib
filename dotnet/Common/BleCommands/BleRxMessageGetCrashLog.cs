using System;
using System.Text;

namespace FicusFactor.ReMousr.Common.BleCommands
{
    public class BleRxMessageGetCrashLog : BleRxMessage
    {
        public string CrashLog
        {
            get;
        }

        public BleRxMessageGetCrashLog(byte[] data) : base(data)
        {
            StringBuilder sb = new StringBuilder();
            for (int i = 1; i < data.Length; i++)
            {
                sb.Append((char)data[i]);
            }

            this.CrashLog = sb.ToString();
        }

        public override string ToString()
        {
            return $"{base.ToString()} Crash log: {this.CrashLog}";
        }
    }
}