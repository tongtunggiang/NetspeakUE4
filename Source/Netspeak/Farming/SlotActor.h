// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "SlotHandlerObject.h"

#include "SlotActor.generated.h"

UCLASS()
class NETSPEAK_API ASlotActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASlotActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual bool ReplicateSubobjects(class UActorChannel *Channel, class FOutBunch *Bunch, FReplicationFlags *RepFlags) override;


public:

	bool CanReactToInput();

	FText Client_GetNextStateText();

	void SwitchToNextState();

	void Server_SwitchToNextState();

private:

	/** Default state when interact with an "empty" slot, aka the state that an actor would get upon its creation */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "States", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<USlotHandlerObject> DefaultState;

	/** Default state when interact with an "empty" slot, aka the state that an actor would get upon its creation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "States|Visual", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* SlotMesh;

private:

	/** CAUTION: Server variable only, don't get replicated at all */
	USlotHandlerObject* SlotStateHandler;

	UPROPERTY()
	UMaterial* SlotMaterial;

private:
	// Values that get replicated

	UPROPERTY(ReplicatedUsing=OnRep_SlotColor)
	FLinearColor SlotColor;

	UPROPERTY(Replicated)
	FText NextStateText;

	UPROPERTY(ReplicatedUsing =OnRep_TimeInState)
	float MinimumTimeInState;

	UFUNCTION()
	void OnRep_SlotColor();

	UFUNCTION()
	void OnRep_TimeInState();

	void Server_InitState(USlotHandlerObject* State);

	void Client_UpdateSlotColor();

	float RemainingTimeInState;
	
};
