// By hzFishy - 2025 - Do whatever you want with it.


#include "Core/CAIBCore.h"
#include "Console/FUConsole.h"


namespace CAIB::Debug
{
#if CAIB_WITH_DEBUG
	FU_CMD_AUTOVAR_EXTERN(DebugStatesModeForAll,
		"CAIB.Debug.DebugStatesModeForAll", "Show debug information for all AIs. 0 or 1 to disable/enable",
		int32, DebugStatesModeForAllValue, 0
	);
	FU_CMD_AUTOVAR_EXTERN(DebugStatesMaxDistanceForAll,
		"CAIB.Debug.DebugStatesMaxDistanceForAll", "Used with DebugStatesModeForAll to avoid displaying far data",
		float, DebugStatesMaxDistanceForAllValue, 1000
	);
	// TODO: per target versions
#endif
}
