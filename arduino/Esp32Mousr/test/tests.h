#pragma once
#ifndef _MOUSR_TEST_H
#define _MOUSR_TEST_H

#include <cstring>
#include <unity.h>
#include "Mousr.h"
#include "utility.h"
#include "logging.h"
#include "Settings.h"

#define t_checkpoint(checkpoint); s_printf("[%s@%s:%d] Checkpoint: %s\n", __FUNCTION__, __FILE__, __LINE__, checkpoint);

#endif