// By hzFishy - 2025 - Do whatever you want with it.

#pragma once

#include "Console/FUConsole.h"


#define CAIB_WITH_DEBUG FU_WITH_CONSOLE

namespace CAIB::Debug
{
#if CAIB_WITH_DEBUG
	/** 0: disabled, 1: single target, 2: all */
	extern int32 DebugStatesModeValue;
	
	extern float DebugStatesMaxDistanceValue;
#endif
}
