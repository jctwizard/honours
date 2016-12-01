// Fill out your copyright notice in the Description page of Project Settings.

#include "tiny_island.h"
#include "MotionControllerActor.h"


// Sets default values
AMotionControllerActor::AMotionControllerActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMotionControllerActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMotionControllerActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AMotionControllerActor::Grab()
{

}

void AMotionControllerActor::Release()
{

}