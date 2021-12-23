using System.Collections.Generic;

namespace FicusFactor.ReMousr.Common
{
    public interface IBlePeripheral
    {
        void DiscoverServices();
        void DiscoverCharacteristics(IBleService service);
    }
}
