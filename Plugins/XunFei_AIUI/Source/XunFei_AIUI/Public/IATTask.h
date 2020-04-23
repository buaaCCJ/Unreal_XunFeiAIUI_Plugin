#pragma once

#include "XunFei_AIUI.h"
#include "Async/AsyncWork.h"

class FIATTask : public FNonAbandonableTask
{

	friend class FAutoDeleteAsyncTask<FIATTask>;

	FIATTask()
	{
		UE_LOG(LogTemp, Warning, TEXT("IAT Task be Create !"));
	}

	void DoWork();
	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FIATTask, STATGROUP_ThreadPoolAsyncTasks);
	}
};