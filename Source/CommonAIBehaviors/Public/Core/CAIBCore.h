// By hzFishy - 2025 - Do whatever you want with it.

#pragma once

#include "Console/FUConsole.h"


#define CAIB_WITH_DEBUG FU_WITH_CONSOLE

namespace CAIB::Debug
{
#if CAIB_WITH_DEBUG
	/** 0: Disabled, 1: Enabled */
	extern int32 DebugStatesModeForAllValue;
	
	extern float DebugStatesMaxDistanceForAllValue;
	
	// TODO: per target version
#endif
}
