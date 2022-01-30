#pragma once
#ifndef EMBEDDED_TESTS_H
#define EMBEDDED_TESTS_H

#include <unity.h>
#include "common.h"

// Log tests
void test_writeLogLn();
void test_writeLogF();
void test_writeLog();

// Mousr tests
void test_ParseMessage();
void test_toHexString();
void test_mousrAlloc();
void test_getRawData();
void test_convertToBytes();
void test_createFromRaw();
void test_connectionStatusMap();
void test_writeLogMemory();

// Settings tests
void test_writeUChar();

#endif EMBEDDED_TESTS_H