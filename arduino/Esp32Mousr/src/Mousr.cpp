#include "Mousr.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdio>

using namespace std;

MousrRawMessageData *raw{};

void initialize(const uint8_t *data, size_t length)
{
    raw->data = (uint8_t*)malloc(length);
    memcpy(raw->data, data, length);
    raw->length = length;
}

MousrData::MousrData(const uint8_t *data, size_t length)
{
    initialize(data, length);
}

MousrData::MousrData(const char* data)
{
    // Great and idiomatic solution adapted from https://stackoverflow.com/a/55459931
    string sout{};
    istringstream ss(data);
    vector<uint8_t> msg;
    while(ss >> std::setw(2) >> sout)
    {
        unsigned u;
        istringstream byte(sout);
        byte >> setbase(16) >> u;
        msg.push_back((uint8_t)u);
    }

    initialize(msg.data(), msg.size());
}

MousrData::~MousrData()
{
    delete raw;
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
    return (MousrMessageData*)raw;
}