using System;

namespace FicusFactor.ReMousr.Common.BleCommands
{
    public class BleRxMessageRobotPose : BleRxMessage
    {
        public float Angle
        {
            get;
        }

        public float Speed
        {
            get;
        }

        public float Hold
        {
            get;
        }

        public bool IsFlipped
        {
            get;
        }

        internal BleRxMessageRobotPose(byte[] data) : base(data)
        {
            this.Speed = BitConverter.ToSingle(data, 1);
            this.Hold = BitConverter.ToSingle(data, 5);
            this.Angle = BitConverter.ToSingle(data, 9);
            this.IsFlipped = data[13] == 1;
        }

        public override string ToString()
        {
            return $"{base.ToString()} Pose: Angle={this.Angle} Speed={this.Speed} Hold={this.Hold} Flipped={this.IsFlipped}";
        }
    }
}