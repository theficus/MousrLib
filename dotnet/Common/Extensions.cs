using System;
namespace FicusFactor.ReMousr.Common
{
    public static class Extensions
    {
        public static string ToHexString(this byte b)
        {
            return ToHexString(new[] { b });
        }

        public static string ToHexString(this byte[] data)
        {
            if (data is null)
            {
                throw new ArgumentNullException(nameof(data));
            }

            return $"0x{BitConverter.ToString(data).Replace("-", "")} ({data.Length} bytes)";
        }
    }
}
