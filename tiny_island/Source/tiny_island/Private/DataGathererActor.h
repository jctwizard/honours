// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "XmlParser.h"
#include "DataGathererActor.generated.h"

UCLASS()
class ADataGathererActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADataGathererActor();
	~ADataGathererActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Whether to Visualise Data or not
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session")
	bool bVisualiseData;

	// The name of the session used when saving/loading the file
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session")
	FString SessionName;

	// The duration of a Data Tick
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session")
	float DataTickDuration;

	// The distance an actor must move before it is updated
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Session")
	float DataUpdateThreshold;

	void TrackActor(AActor* Actor);
	void UntrackActor(AActor* Actor);
	void GatherEvent(AActor* Actor, FString Label, bool Success);

private:
	void GatherActorData();
	void VisualiseActorData();
	FXmlNode* GetNode(FXmlNode* ParentNode, FString NodeName);
	bool NodeExists(FXmlNode* ParentNode, FString NodeName);
	FXmlNode* GetActorNode(AActor* TrackedActor, int NodeTick);

	// The file to store data in
	FXmlFile* XmlFile;

	// The root node
	FXmlNode* RootNode;

	// The actors being tracked
	TArray<AActor*> TrackedActors;

	TArray<FVector> TrackedActorLocations;

	// The time since data gathering began
	float ElapsedTime;

	// The time since the last data tick
	float DataTickTime;

	// The current data tick
	int32 CurrentDataTick;

};
