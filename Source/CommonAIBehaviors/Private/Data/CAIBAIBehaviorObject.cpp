// Copyright SurvivalMetroGame Dev Team. All Rights Reserved.


#include "Data/CAIBAIBehaviorObject.h"

	
	/*----------------------------------------------------------------------------
		Defaults
	----------------------------------------------------------------------------*/
UCAIBAIBehaviorObject::UCAIBAIBehaviorObject():
	ElapsedTime(0),
	bFinished(false)
{}

	
	/*----------------------------------------------------------------------------
		Core
	----------------------------------------------------------------------------*/
void UCAIBAIBehaviorObject::Start()
{
	
}

void UCAIBAIBehaviorObject::Tick(float DeltaTime)
{
	ElapsedTime += DeltaTime;
}

void UCAIBAIBehaviorObject::Stop()
{
	
}

void UCAIBAIBehaviorObject::MarkAsFinished()
{
	bFinished = true;
}

bool UCAIBAIBehaviorObject::IsMarkedAsFinished() const
{
	return bFinished;
}
