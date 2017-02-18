// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "InteractableActor.generated.h"

UCLASS()
class AInteractableActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractableActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Adaptive" )
	bool bAdaptiveScaling;

	UPROPERTY( EditAnywhere, Category = "Meshes", BlueprintReadWrite )
	UStaticMeshComponent* Mesh;

	UPROPERTY( EditAnywhere, Category = "Meshes", BlueprintReadWrite )
	UStaticMeshComponent* AdaptiveMesh;

	TArray<AActor*> ControllerActors;
};
