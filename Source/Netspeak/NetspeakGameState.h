// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Farming/SlotActor.h"

#include "NetspeakGameState.generated.h"

/**
 *
 */
UCLASS()
class NETSPEAK_API ANetspeakGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ANetspeakGameState();

public:
	UFUNCTION(BlueprintCallable)
	float GetSlotSize() { return SlotSize; }

	UFUNCTION(BlueprintCallable)
	UClass* GetDefaultSlotActorClass() { return *DefaultSlotActorClass; }

private:
	// Configurations
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = "Configuration", meta = (AllowPrivateAccess = "true"))
	float SlotSize;


	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = "Configuration", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ASlotActor> DefaultSlotActorClass;
	// End Configuration

};
