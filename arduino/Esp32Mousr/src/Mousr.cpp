#include "Mousr.h"
#include <cstring>
#include <vector>

void MousrData::initializeData(const MousrMessage msg, const MousrCommand cmd, const uint8_t *data, const size_t length, const size_t padding)
{
    // Default padding is 3, one for msg, one for cmd, one for trailing byte
    this->rawLength = length + padding;
    this->raw = (uint8_t*)malloc(this->rawLength);
    clearmem(this->raw, this->rawLength);
    memcpy(&this->raw[0], &msg, 1);
    memcpy(&this->raw[1], data, length);
    memcpy(&this->raw[length+1], &cmd, 1);
}

MousrData::MousrData(const MousrMessage msg, const MousrCommand cmd, const size_t length)
{
    uint8_t* data = (uint8_t*)malloc(length);
    clearmem(data, length);
    this->initializeData(msg, cmd, data, length);
}

MousrData::MousrData(const MousrMessage msg, const MousrCommand cmd, const uint8_t *data, const size_t length)
{
    this->initializeData(msg, cmd, data, length);
}

MousrData::MousrData(const uint8_t *data, size_t length)
{
    raw = (uint8_t*)malloc(length);
    if (raw == NULL)
    {
        s_printf("ERROR: Unable to allocate %zu bytes\n", length);
    }
    rawLength = length;
    memcpy(raw, data, length);
}

// TODO: Convert to binary 
MousrData::MousrData(const char *data)
{
    size_t length = strlen(data);
    int start = 0;
    if (length < 2)
    {
        s_println("ERROR: Invalid input data.");
        return;
    }

    if (data[0] == '0' && data[1] == 'x')
    {
        length -= 2;
        start = 2;
    }

    this->rawLength = length / 2;
    this->raw = (uint8_t*)malloc(this->rawLength);

    if (this->raw == nullptr)
    {
        s_println("ERROR: Could not allocate memory");
    }

    for (int i = 0, di = start; i < this->rawLength; i++, di += 2)
    {
        char hs[2] = { data[di], data[di+1] };
        raw[i] = (uint8_t)std::stoul(hs, nullptr, 16);
        s_printf("%d = %x\n", i, raw[i]);
    }
}

MousrData::~MousrData()
{
    if (raw != NULL)
    {
        free(raw);
        raw = NULL;
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
    length = rawLength;
    *data = (uint8_t *)malloc(length);
    memcpy(*data, raw, length);
}

MousrMessageData *MousrData::getMessageData()
{
    return (MousrMessageData *)raw;
}
