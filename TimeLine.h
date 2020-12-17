#pragma once

#include "TimeLine.generated.h"
//
// UCLASS()
// class UTimeLineTask : public UObject
// {
// 	GENERATED_BODY()
// public:
// 	float StartTime;
// 	float EndTime = -1;
// 	float TotalDuration;
// 	float TimeLeft;
// 	float TimePassed;
// 	bool IsExecuting;
// 	virtual void OnExecuted(){};
// 	virtual void OnFinished(){};
//
// 	DECLARE_MULTICAST_DELEGATE(FTaskDelegate);
// 	DECLARE_MULTICAST_DELEGATE(FTaskDelegate);
// 	
// 	virtual void Tick(float DeltaSeconds);
// };
//
// UCLASS()
// class UTimeLine : public UObject
// {
// 	GENERATED_BODY()
//
// 	float TimePassed;
//
// 	UPROPERTY()
// 	TArray<UTimeLineTask*> Tasks;
// public:
// 	UTimeLine()
// 	{
// 		TimePassed = 0;
// 		FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this,&UTimeLine::Tick));
// 	};
//
// 	UFUNCTION()
//     bool Tick(float DeltaSeconds);
// };