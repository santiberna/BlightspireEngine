#pragma once

#include "fmod_common.h"

void FMOD_CHECKRESULT_fn(FMOD_RESULT result, [[maybe_unused]] const char* file, int line);
#define FMOD_CHECKRESULT(result) FMOD_CHECKRESULT_fn(result, __FILE__, __LINE__)

void StartFMODDebugLogger();