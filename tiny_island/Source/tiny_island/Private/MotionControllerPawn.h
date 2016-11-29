// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include <iostream>
#include <fstream>
#include "MotionControllerPawn.generated.h"

using namespace std;

UCLASS()
class AMotionControllerPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMotionControllerPawn();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InInputComponent) override;

	void GrabLeft();
	void GrabRight();

	void ReleaseLeft();
	void ReleaseRight();

	UFUNCTION(BlueprintImplementableEvent, Category = "Actions")
	void GrabLeftAction();

	UFUNCTION(BlueprintImplementableEvent, Category = "Actions")
	void GrabRightAction();

	UFUNCTION(BlueprintImplementableEvent, Category = "Actions")
	void ReleaseLeftAction();

	UFUNCTION(BlueprintImplementableEvent, Category = "Actions")
	void ReleaseRightAction();

	FString SessionName;

	void AddDataPoint(FString Description, FVector Location, bool Success);
};
