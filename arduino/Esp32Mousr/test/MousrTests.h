#pragma once
#ifndef _MOUSR_TESTS_H
#define _MOUSR_TESTS_H

void test_ParseMessage();
void test_mousrAlloc();
void test_getRawData();
void test_convertToBytes();
void test_createFromRaw();
void test_connectionStatusMap();
void test_messageCtor();

void runMousrTests();

#endif