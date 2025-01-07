// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Sound/SoundWaveProcedural.h"

#include "XunFeiAIUIComponent.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FIATGetDelegateSignature, FString,test,FString,answer,int,id);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class XUNFEI_AIUI_API UXunFeiAIUIComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UXunFeiAIUIComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
	UFUNCTION(BlueprintCallable, Category = "XunFei", meta = (DisplayName = "AIUIInit", Keywords = "Speech Recognition Initialization"))
		void AIUIInit();

	UFUNCTION(BlueprintCallable, Category = "XunFei", meta = (DisplayName = "AIUIStartMic", Keywords = "Speech Recognition Initialization"))
		void AIUIStartMic();

	UFUNCTION(BlueprintCallable, Category = "XunFei", meta = (DisplayName = "AIUIStopMic", Keywords = "Speech Recognition Initialization"))
		void AIUIStopMic();


	void GetResult(FString test, FString Answer,int AnswerID);

	UPROPERTY(BlueprintAssignable, Category = "Test")
		FIATGetDelegateSignature OnGetResult;
public:

};
