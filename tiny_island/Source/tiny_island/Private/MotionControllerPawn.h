// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "GameFramework/Pawn.h"
#include <iostream>
#include <fstream>
#include "Runtime/HeadMountedDisplay/Public/HeadMountedDisplay.h"
#include "Runtime/HeadMountedDisplay/Public/MotionControllerComponent.h"
#include "MotionControllerActor.h"
#include "DataGeneratorComponent.h"
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

	void ToggleLaserLeft();
	void ToggleLaserRight();

	void ToggleAdaptiveLeft();
	void ToggleAdaptiveRight();

	UPROPERTY( EditAnywhere, Category = "Grabbing")
	EControllerHand ControllersToUse;

	UPROPERTY( EditAnywhere, Category = "Grabbing" )
	bool bUseLaser;

	UPROPERTY( EditAnywhere, Category = "Grabbing" )
	bool bUseAdaptive;

	UPROPERTY( EditAnywhere, Category = "Grabbing" )
	bool bCanToggleLaser;

	UPROPERTY( EditAnywhere, Category = "Grabbing" )
	bool bCanToggleAdaptive;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* VROrigin;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	AMotionControllerActor* LeftController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	AMotionControllerActor* RightController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties")
	float BasePlayerHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grabbing")
	float BaseGrabRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMesh* HandMeshAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UParticleSystem* LaserParticle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UDataGeneratorComponent* DataGenerator;
};
