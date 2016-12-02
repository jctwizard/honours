#pragma once

#include "GameFramework/Actor.h"
#include "Runtime/HeadMountedDisplay/Public/MotionControllerComponent.h"
#include "GrabbableInterface.h"
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

	bool Grab();
	bool Release();

	void SetHand(EControllerHand Hand);
	void SetHandMesh(USkeletalMesh* HandMeshAsset);

	AActor* GetNearestActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Motion")
	UMotionControllerComponent* MotionController;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hand")
	USkeletalMeshComponent* HandMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grabbing")
	USphereComponent* GrabSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grabbing")
	AActor* GrabbedActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grabbing")
	bool bWantsToGrab;
};