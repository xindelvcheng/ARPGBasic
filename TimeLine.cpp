// #include "TimeLine.h"
//
//
//
// void UTimeLineTask::Tick(float DeltaSeconds)
// {
// 	
// }
//
// bool UTimeLine::Tick(float DeltaSeconds)
// {
// 	TimePassed += DeltaSeconds;
// 	for (int i = Tasks.Num() - 1; i >= 0; --i)
// 	{
// 		UTimeLineTask* Task = Tasks[i];
// 		if (Task && TimePassed > Task->StartTime)
// 		{
// 			if (!Task->IsExecuting)
// 			{
// 				Task->OnExecuted();
// 				Task->IsExecuting = true;
// 			}
// 			Task->Tick(DeltaSeconds);
// 			if (Task->EndTime != -1 && Task->EndTime > TimePassed)
// 			{
// 				Task->OnFinished();
// 				Tasks.RemoveAt(i);
// 			}
// 		}
// 	}
// 	return true;
// }