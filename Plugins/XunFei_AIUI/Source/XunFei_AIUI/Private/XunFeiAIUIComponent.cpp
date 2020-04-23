// Fill out your copyright notice in the Description page of Project Settings.


#include "XunFeiAIUIComponent.h"
#include "XunFeiSpeech.h"
#include "SpeechRecoginzer.h"
#include "Blueprint/UserWidget.h"

// Sets default values for this component's properties
UXunFeiAIUIComponent::UXunFeiAIUIComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UXunFeiAIUIComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UXunFeiAIUIComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//FString temp;

	//{
	//	FScopeLock ScopeLock(&TaskSync);
	//	if(IATResult.Num()>0)
	//	{
	//		auto StartTime = FPlatformTime::Seconds();
	//		UE_LOG(LogTemp, Log, TEXT("before generate UI:showtime: %d"), StartTime);
	//		temp = IATResult[0];
	//		IATResult.RemoveAt(0);
	//		auto PlayerController = Cast<ADigitalHumanPlayerController>(GetWorld()->GetFirstPlayerController());
	//		PlayerController->CreateIATwidget(temp);
	//		StartTime = FPlatformTime::Seconds();
	//		UE_LOG(LogTemp, Log, TEXT("after generate UI:showtime: %d"), StartTime);
	//	}
	//	if(TTSResult.Num()>0)
	//	{/*
	//		if(TTS)
	//		{
	//			auto StartTime = FPlatformTime::Seconds();
	//			UE_LOG(LogTemp, Log, TEXT("before PlayTTSResult:showtime: %d"), StartTime);
	//			TTS->PlayTTSResult(TTSResult[0]);
	//			TTSResult.RemoveAt(0);
	//			 StartTime = FPlatformTime::Seconds();
	//			UE_LOG(LogTemp, Log, TEXT("after PlayTTSResult:showtime: %d"), StartTime);
	//		}*/
	//		
	//	}
	//}
	
	
}

void UXunFeiAIUIComponent::AIUIInit()
{
	//
	AIUITester.setRootDir("");
	AIUITester.createAgent();
	AIUITester.wakeup();
	AIUITester.initWrite();
	FXunFeiIAT::GetInstance().StartIATThread();
	AIUITester.listener.OnIATFinished.BindUObject(this, &UXunFeiAIUIComponent::GetResult);
}

void UXunFeiAIUIComponent::AIUIStartMic()
{
	FXunFeiIAT::GetInstance().SetStart();
}

void UXunFeiAIUIComponent::AIUIStopMic()
{
	FXunFeiIAT::GetInstance().SetStop();
}

void UXunFeiAIUIComponent::GetResult(FString Result,FString Answer, int AnswerID)
{
	UE_LOG(LogTemp, Log, TEXT("xunfei result: %s"), *Result);
}

