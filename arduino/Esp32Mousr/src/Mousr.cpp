#include "Mousr.h"
#include <cstring>
#include <vector>

uint8_t *raw;
size_t rawLength;

MousrData::MousrData(const MousrMessage msg, const MousrCommand cmd, const uint8_t *data, const size_t length)
{
    rawLength = length + 3; // +1 for msg, +1 for cmd, +1 for trailing byte
    raw = (uint8_t *)malloc(rawLength);
    clearmem(raw, length + 3);
    memcpy(&raw[0], &msg, 1);
    memcpy(&raw[1], data, length);
    memcpy(&raw[length + 1], &cmd, 1);
}

MousrData::MousrData(const uint8_t *data, size_t length)
{
    raw = (uint8_t*)malloc(length);
    rawLength = length;
    memcpy(raw, data, length);
}

// TODO: Convert to binary 
MousrData::MousrData(const char *data)
{
    rawLength = strlen(data);
    raw = (uint8_t*)malloc(rawLength);
    memcpy(raw, data, rawLength);
}

MousrData::~MousrData()
{
    if (raw != nullptr)
    {
        free(raw);
    }
}

size_t MousrData::getMessageLength()
{
    return rawLength;
}

MousrMessage MousrData::getMessageKind()
{
    return (MousrMessage)raw[0];
}

std::string MousrData::toString()
{
    // s_writeLogF("MousrData::toString: %p (%zu)\n", raw.data(), raw.size());
    return MousrData::toString(raw, rawLength);
}

void MousrData::getRawMessageData(uint8_t **data, size_t &length)
{
    *data = (uint8_t *)malloc(rawLength);
    memcpy(*data, raw, length);
}

MousrMessageData *MousrData::getMessageData()
{
    return (MousrMessageData *)raw;
}