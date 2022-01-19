#include "Mousr.h"
#include "SerialLog.h"

using namespace std;

MousrRawMessageData *raw;

void initialize(const uint8_t *data, size_t length)
{
    raw = (MousrRawMessageData*)malloc(sizeof(MousrRawMessageData));
    raw->data = (uint8_t*)malloc(length);
    memcpy(raw->data, data, length);
    raw->length = length;
}

void initialize(string data)
{
    vector<uint8_t> msg;

    for(unsigned i = 0; i < data.length(); i += 2)
    {
        string ss = data.substr(i, 2);

        if (i == 0 && (ss == "0x" || ss == "0X"))
        {
            continue;
        }
        
        uint8_t b = strtol(ss.c_str(), nullptr, 16);
        msg.push_back(b);
    }

    initialize(msg.data(), msg.size());
}

MousrData::MousrData(const uint8_t *data, size_t length)
{
    initialize(data, length);
}

MousrData::MousrData(const char* data)
{
    initialize(string(data));
}  

MousrData::MousrData(string data)
{
    initialize(data);
}

MousrData::~MousrData()
{
    if (raw != nullptr) 
    {
    }
}

MousrMessage MousrData::getMessageKind()
{
    return (MousrMessage)raw->data[0];
}

MousrRawMessageData *MousrData::getRawMessageData()
{
    return raw;
}

MousrMessageData *MousrData::getMessageData()
{
    return (MousrMessageData*)raw->data;
}