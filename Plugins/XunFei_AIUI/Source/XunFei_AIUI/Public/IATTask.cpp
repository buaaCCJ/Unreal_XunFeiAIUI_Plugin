#include "IATTask.h"
#include "XunFeiSpeech.h"

void FIATTask::DoWork()
{
	FXunFeiIAT::beginMicLoop();
	UE_LOG(LogTemp, Log, TEXT("DO WORK"));
}
