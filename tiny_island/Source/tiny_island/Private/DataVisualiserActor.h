// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Components/SplineComponent.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "DataVisualiserActor.generated.h"

using namespace std;

UCLASS()
class ADataVisualiserActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADataVisualiserActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session")
	FString SessionName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visualisation")
	USceneComponent* SplineRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visualisation")
	USplineComponent* LeftSpline;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visualisation")
	USplineComponent* RightSpline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session")
	USkeletalMesh* PlaceholderMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visualisation")
	USkeletalMeshComponent* PlaceholderLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visualisation")
	USkeletalMeshComponent* PlaceholderRight;

	TArray<float> LeftSplinePointTimes, RightSplinePointTimes;
	int LeftSplinePoint, RightSplinePoint;
	float CurrentSplineTime;

private:
	float ToSeconds(FDateTime TimeStamp);
};
