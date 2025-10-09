// By hzFishy - 2025 - Do whatever you want with it.


#include "Data/CAIBPerceptionTypes.h"


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


CAIBTrackedStimuliSource::CAIBTrackedStimuliSource():
	TargetId(-1)
{}

CAIBTrackedStimuliSource::CAIBTrackedStimuliSource(const FActorPerceptionUpdateInfo& UpdateInfo):
	TargetId(UpdateInfo.TargetId),
	Target(UpdateInfo.Target),
	LatestStimulus(UpdateInfo.Stimulus)
{}
