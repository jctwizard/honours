// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Runtime/HeadMountedDisplay/Public/MotionControllerComponent.h"
#include "MotionControllerActor.generated.h"

UCLASS()
class AMotionControllerActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMotionControllerActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	void Grab();
	void Release();

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UMotionControllerComponent* MotionController;
};