// By hzFishy - 2025 - Do whatever you want with it.


#include "Data/CAIBPerceptionTypes.h"


	/*----------------------------------------------------------------------------
		Defaults
	----------------------------------------------------------------------------*/
UCAIBAISense_Sight::UCAIBAISense_Sight()
{
	//NotifyType = EAISenseNotifyType::OnEveryPerception;
}

UCAIBAISenseConfig_Sight::UCAIBAISenseConfig_Sight()
{
	
}

TSubclassOf<UAISense> UCAIBAISenseConfig_Sight::GetSenseImplementation() const
{
	return UCAIBAISense_Sight::StaticClass();
}
