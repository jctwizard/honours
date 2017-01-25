#pragma once

#include "GameFramework/Actor.h"
#include "Runtime/HeadMountedDisplay/Public/MotionControllerComponent.h"
#include "GrabbableInterface.h"
#include "DataGeneratorComponent.h"
#include "MotionControllerActor.generated.h"

UENUM(BlueprintType)
enum class EGrabMethod : uint8
{
	GM_Sphere 	UMETA(DisplayName = "Sphere"),
	GM_Laser 	UMETA(DisplayName = "Laser")
};

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

	void DrawLaser();

	bool Grab();
	bool Release();
	void ToggleLaser();

	void SetHand(EControllerHand Hand);
	void SetHandMesh(USkeletalMesh* HandMeshAsset);
	void SetGrabRadius(float GrabRadius);
	FVector GetControllerLocation();

	AActor* GetNearestActor();
	AActor* GetNearestLaserActor();

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grabbing")
	EGrabMethod GrabMethod;

private:
	UDataGeneratorComponent* DataGenerator;
};