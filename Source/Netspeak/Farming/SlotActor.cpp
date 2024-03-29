// Fill out your copyright notice in the Description page of Project Settings.

#include "SlotActor.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"
#include "Net/UnrealNetwork.h"
#include <Engine/ActorChannel.h>


// Sets default values
ASlotActor::ASlotActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	// Create slot mesh
	SlotMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SlotMesh"));
	SlotMesh->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UMaterial> MaterialAsset(TEXT("Material'/Game/Farming/Slots/Materials/M_SlotMaterial.M_SlotMaterial'"));
	SlotMaterial = MaterialAsset.Object;

	// Create state handler
	static ConstructorHelpers::FObjectFinder<UBlueprint> DefaultStateAsset(TEXT("Blueprint'/Game/Farming/Slots/Blueprints/BP_SoilState.BP_SoilState'"));
	DefaultState = (UClass*)DefaultStateAsset.Object->GeneratedClass;

	// Set replicate actor
	bReplicates = true;
}

void ASlotActor::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASlotActor, SlotColor);
	DOREPLIFETIME(ASlotActor, NextStateText);
	DOREPLIFETIME(ASlotActor, MinimumTimeInState);
}

// Called when the game starts or when spawned
void ASlotActor::BeginPlay()
{
	Super::BeginPlay();

	SlotMesh->SetMaterial(0, SlotMaterial);

	auto DefaultStateObject = NewObject<USlotHandlerObject>(this, *DefaultState);
	if (HasAuthority())
	{
		Server_InitState(DefaultStateObject);
	}
	else
	{
		SlotColor = DefaultStateObject->GetStateColor();
		Client_UpdateSlotColor();
	}
}

// Called every frame
void ASlotActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (RemainingTimeInState > 0)
	{
		RemainingTimeInState -= DeltaTime;
		if (RemainingTimeInState <= 0)
		{
			RemainingTimeInState = 0;
		}
	}
}

bool ASlotActor::ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	if (SlotStateHandler != nullptr)
	{
		WroteSomething |= Channel->ReplicateSubobject(SlotStateHandler, *Bunch, *RepFlags);
	}

	return WroteSomething;
}

bool ASlotActor::CanReactToInput()
{
	return RemainingTimeInState <= 0;
}

FText ASlotActor::Client_GetNextStateText()
{
	if (RemainingTimeInState <= 0)
		return NextStateText;

	return FText::Format(FText::FromString("{0} in {1} seconds"), NextStateText, FMath::FloorToInt(RemainingTimeInState));
}

void ASlotActor::OnRep_SlotColor()
{
	Client_UpdateSlotColor();
}

void ASlotActor::OnRep_TimeInState()
{
	RemainingTimeInState = MinimumTimeInState;
}

void ASlotActor::Server_InitState(USlotHandlerObject* State)
{
	SlotStateHandler = State;

	SlotColor = SlotStateHandler->StateColor;
	NextStateText = SlotStateHandler->GetNextStateHandler()->StateText;
	MinimumTimeInState = SlotStateHandler->MinimumTimeInState;
	RemainingTimeInState = MinimumTimeInState;
}

void ASlotActor::Client_UpdateSlotColor()
{
	UMaterialInstanceDynamic* Material = (UMaterialInstanceDynamic*)SlotMesh->CreateAndSetMaterialInstanceDynamic(0);
	Material->SetVectorParameterValue(FName(TEXT("Color")), SlotColor);
}

void ASlotActor::SwitchToNextState()
{
	// double check the time requirements
	// I don't reuse the CanReactToInput() function for clarity
	// and sometimes that function may require additional condition
	if (RemainingTimeInState > 0)
		return;

	Server_InitState(SlotStateHandler->GetNextStateHandler());

	if (Role < ROLE_Authority)
	{
		Server_SwitchToNextState();
	}
}

void ASlotActor::Server_SwitchToNextState()
{
	SwitchToNextState();
}
