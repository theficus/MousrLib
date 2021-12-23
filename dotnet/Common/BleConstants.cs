using System;

namespace FicusFactor.ReMousr.Common
{
    public static class BleConstants
    {
        // Nordic UART service UUID
        public static readonly Guid UartServiceUuid = new Guid("6e400001-b5a3-f393-e0a9-e50e24dcca9e");

        // Nordic UART TX characteristic 
        public static readonly Guid UartWriteCharacteristic = new Guid("6e400002-b5a3-f393-e0a9-e50e24dcca9e");

        // Nordic UART RX characteristic
        public static readonly Guid UartSubscribeCharacteristic = new Guid("6e400003-b5a3-f393-e0a9-e50e24dcca9e");

        // Nordic DFU service UUID
        public static readonly Guid DfuServiceUuid = new Guid("0000fe59-0000-1000-8000-00805f9b34fb");

        // Nordic DFU TX characteristic
        public static readonly Guid DfuWriteCharacteristic = new Guid("8ec90002-f315-4f60-9fb8-838830daea50");

        // Nordic DFU RX characterstic
        public static readonly Guid DfuSubscribeCharacteristic = new Guid("8ec90001-f315-4f60-9fb8-838830daea50");
    }
}