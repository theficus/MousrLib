#include "Mousr.h"
#include "SerialLog.h"

using namespace std;

vector<uint8_t> raw;

void initialize(string data)
{
    for (unsigned i = 0; i < data.length(); i += 2)
    {
        string ss = data.substr(i, 2);

        if (i == 0 && (ss == "0x" || ss == "0X"))
        {
            continue;
        }

        uint8_t b = strtol(ss.c_str(), nullptr, 16);
        raw.push_back(b);
    }
}

MousrData::MousrData(const uint8_t *data, size_t length)
{
    for (int i = 0; i < length; i++)
    {
        raw.push_back(data[i]);
    }
}

MousrData::MousrData(const char *data)
{
    initialize(string(data));
}

MousrData::MousrData(string data)
{
    initialize(data);
}

MousrData::~MousrData()
{
    raw.clear();
}

size_t MousrData::getMessageLength()
{
    return raw.size();
}

MousrMessage MousrData::getMessageKind()
{
    return (MousrMessage)raw.front();
}

string MousrData::toString()
{
    stringstream ss;
    ss << "0x";
    ss << hex << setfill('0');

    size_t length = raw.size();
    for (int i = 0; i < length; i++)
    {
        ss << setw(2) << (unsigned)raw[i];
    }

    return ss.str();
}

MousrMessageData *MousrData::getMessageData()
{
    return (MousrMessageData *)raw.data();
}

void MousrData::sendMessage(BLERemoteCharacteristic *characteristic)
{
    debugLogF("Sending: %s", this->toString().c_str());
    characteristic->writeValue(raw.data(), raw.size());
}