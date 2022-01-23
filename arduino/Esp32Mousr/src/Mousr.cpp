#include "Mousr.h"

using namespace std;

vector<uint8_t> raw;

void initialize(string data)
{
    for (unsigned i = 0; i < data.length(); i += 2)
    {
        string ss = data.substr(i, 2);

        if (i == 0 &&
            (ss == "0x" || ss == "0X"))
        {
            continue;
        }

        uint8_t b = strtol(ss.c_str(), nullptr, 16);
        raw.push_back(b);
    }
}

MousrData::MousrData(const MousrMessage msg, const MousrCommand cmd, vector<uint8_t> data)
{
    raw.push_back((uint8_t)msg);
    int length = data.size();
    if (length < 12)
    {
        length = 12;
    }

    for (int i = 0; i < length; i++)
    {
        raw.push_back(data[i]);
    }

    raw.push_back((uint8_t)cmd);
    raw.push_back(0);
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
    return MousrData::toString(raw.data(), raw.size());
}

void MousrData::getRawMessageData(uint8_t **data, size_t &length)
{
    length = raw.size();
    *data = (uint8_t *)malloc(length);
    memcpy(*data, raw.data(), length);
}

MousrMessageData *MousrData::getMessageData()
{
    return (MousrMessageData *)raw.data();
}