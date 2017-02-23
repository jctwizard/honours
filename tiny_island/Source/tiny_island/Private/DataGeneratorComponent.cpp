// Fill out your copyright notice in the Description page of Project Settings.

#include "tiny_island.h"
#include "DataGeneratorComponent.h"


// Sets default values for this component's properties
UDataGeneratorComponent::UDataGeneratorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UDataGeneratorComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADataGathererActor::StaticClass(), FoundActors);

	if (FoundActors.Num() > 0)
	{
		DataGathererActor = (ADataGathererActor*)FoundActors[0];

		DataGathererActor->TrackActor(GetOwner());
	}
}


// Called every frame
void UDataGeneratorComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if (GetOwner()->IsPendingKill())
	{
		DataGathererActor->UntrackActor(GetOwner());
	}
}

// Create an event in the data gatherer
void UDataGeneratorComponent::GenerateEvent(FString EventName, bool Success)
{
	DataGathererActor->GatherEvent(GetOwner(), EventName, Success);
}