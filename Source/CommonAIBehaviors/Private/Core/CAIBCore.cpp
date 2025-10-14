// By hzFishy - 2025 - Do whatever you want with it.


#include "Core/CAIBCore.h"
#include "Console/FUConsole.h"
#include "Core/CAIBBehaviorSubsystem.h"


namespace CAIB::Debug
{
#if CAIB_WITH_DEBUG
	FU_CMD_AUTOVAR_EXTERN(DebugStatesMode,
		"CAIB.Debug.DebugStatesMode",
		"Show debug information for AIs. 0: disabled, 1: single target, 2: all",
		int32, DebugStatesModeValue, 0
	);
	
	FU_CMD_AUTOVAR_EXTERN(DebugStatesMaxDistance,
		"CAIB.Debug.DebugStatesMaxDistance",
		"Used with DebugStatesMode to avoid displaying debug data if the target(s) is far away. Set to 0 for infinity",
		float, DebugStatesMaxDistanceValue, 2000
	);
	
	FU_CMD_RUNLAMBDA(DebugStatesGoToNextTarget,
		"CAIB.Debug.DebugStatesGoToNextTarget", "Used with DebugStatesMode",
		{
			if (auto* SS = GetValid(GWorld->GetSubsystem<UCAIBBehaviorSubsystem>()))
			{
				SS->GoToNextDebugAIPawn();
			}
		}
	);
	
	FU_CMD_RUNLAMBDA(DebugStatesGoToPreviousTarget,
		"CAIB.Debug.DebugStatesGoToPreviousTarget", "Used with DebugStatesMode",
		{
			if (auto* SS = GetValid(GWorld->GetSubsystem<UCAIBBehaviorSubsystem>()))
			{
				SS->GoToPreviousDebugAIPawn();
			}
		}
	);
	
#endif
}
