namespace FicusFactor.ReMousr.Common.BleCommands
{
    public class BleRxMessageBatteryVoltage : BleRxMessage
    {
        public int Percent
        {
            get;
        }

        internal BleRxMessageBatteryVoltage(byte[] data)
            : base(data)
        {
            this.Percent = (int)data[1];
        }

        public override string ToString()
        {
            return $"{base.ToString()} Battery percent: {this.Percent}";
        }
    }
}