// Fill out your copyright notice in the Description page of Project Settings.

#include "tiny_island.h"
#include "MotionControllerActor.h"


// Sets default values
AMotionControllerActor::AMotionControllerActor() :
	bWantsToGrab(false),
	GrabbedActor(nullptr)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create the motion controller component and attach it to the root
	MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionController"));
	MotionController->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);

	// Create the hand mesh and attach it to the root component
	HandMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HandMesh"));
	HandMesh->AttachToComponent(MotionController, FAttachmentTransformRules::KeepWorldTransform);

	// Create the grab sphere and attach it to the hand mesh
	GrabSphere = CreateDefaultSubobject<USphereComponent>(TEXT("GrabSphere"));
	GrabSphere->AttachToComponent(HandMesh, FAttachmentTransformRules::KeepWorldTransform);
}

// Called when the game starts or when spawned
void AMotionControllerActor::BeginPlay()
{
	Super::BeginPlay();

	// Flip the Mesh of the left hand
	if (MotionController->Hand == EControllerHand::Left)
	{
		HandMesh->SetWorldScale3D(FVector(1.0f, 1.0f, -1.0f));
	}
}

// Called every frame
void AMotionControllerActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

bool AMotionControllerActor::Grab()
{
	bWantsToGrab = true;

	AActor* NearestActor = GetNearestActor();

	// Get the closest actor overlapping the hand
	if (NearestActor->IsValidLowLevel())
	{
		IGrabbableInterface* GrabbableInterface = Cast<IGrabbableInterface>(NearestActor);

		// Check if the actor is grabbable
		if (GrabbableInterface)
		{
			GrabbedActor = NearestActor;
			GrabbableInterface->Execute_OnGrab(GrabbedActor);
			GrabbedActor->AttachToComponent(MotionController, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

			return true;
		}
	}

	return false;
}

bool AMotionControllerActor::Release()
{
	bWantsToGrab = false;

	if (GrabbedActor->IsValidLowLevel())
	{
		// Check if the actor is still being grabbed
		if (GrabbedActor->GetRootComponent()->GetAttachParent() == MotionController)
		{
			IGrabbableInterface* GrabbableInterface = Cast<IGrabbableInterface>(GrabbedActor);

			// Check if the actor is grabbable
			if (GrabbableInterface)
			{
				// Drop the actor
				GrabbableInterface->Execute_OnRelease(GrabbedActor);
				GrabbedActor->GetRootComponent()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
				GrabbedActor = nullptr;

				return true;
			}
		}
		else
		{
			GrabbedActor = nullptr;
		}
	}

	return false;
}

AActor* AMotionControllerActor::GetNearestActor()
{
	TArray<AActor*> NearbyActors;
	GrabSphere->GetOverlappingActors(NearbyActors, AActor::StaticClass());

	float NearestOverlap = 10000.0f;
	AActor* NearestOverlappingActor = nullptr;

	// Check each overlapping actor and get the nearest one
	for (auto& NearbyActor : NearbyActors)
	{
		float OverlapDistance = GetDistanceTo(NearbyActor);

		if (OverlapDistance < NearestOverlap)
		{
			NearestOverlappingActor = NearbyActor;
			NearestOverlap = OverlapDistance;
		}
	}

	return NearestOverlappingActor;
}

void AMotionControllerActor::SetHand(EControllerHand Hand)
{
	MotionController->Hand = Hand;
}