// Fill out your copyright notice in the Description page of Project Settings.

#include "tiny_island.h"
#include "DataGathererActor.h"


// Sets default values
ADataGathererActor::ADataGathererActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADataGathererActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADataGathererActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

