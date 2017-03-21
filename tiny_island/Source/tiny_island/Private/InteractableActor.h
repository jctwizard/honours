// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "DataGeneratorComponent.h"
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

	UPROPERTY(EditAnywhere, Category = "Scaling", BlueprintReadWrite)
	float HighlightDistance;

	UPROPERTY(EditAnywhere, Category = "Scaling", BlueprintReadWrite)
	float ScaleDistance;

	UPROPERTY(EditAnywhere, Category = "Scaling", BlueprintReadWrite)
	float MaxScale;

	UPROPERTY( EditAnywhere, Category = "DataGenerator", BlueprintReadWrite )
	UDataGeneratorComponent* DataGenerator;

private:
	TArray<AActor*> ControllerActors;
};
