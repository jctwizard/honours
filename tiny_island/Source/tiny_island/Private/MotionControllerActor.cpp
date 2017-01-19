// Fill out your copyright notice in the Description page of Project Settings.

#include "tiny_island.h"
#include "MotionControllerActor.h"

// Sets default values
AMotionControllerActor::AMotionControllerActor() :
	bWantsToGrab(false),
	bUseLaser(false),
	GrabbedActor(nullptr)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!MotionController)
	{
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

	if (bUseLaser)
	{
		DrawLaser();
	}
}

void AMotionControllerActor::DrawLaser()
{
	// Set up the trace parameters to find where the laser ends
	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("Laser Trace")), true, this);
	TraceParams.bTraceComplex = true;
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = false;

	float TraceDistance = 1000.0f;

	FHitResult HitOut(ForceInit);

	// Trace for an actor in the direction of the controller
	GetWorld()->LineTraceSingleByChannel(
		HitOut,
		GrabSphere->GetComponentLocation(),
		GrabSphere->GetComponentLocation() + GetActorForwardVector() * TraceDistance,
		ECC_WorldDynamic,
		TraceParams
	);

	// Check if an actor is being hit by the laser
	if (HitOut.GetActor() != NULL)
	{
		TraceDistance = HitOut.Distance;
	}

	// Draw a line to the actor
	DrawDebugLine(
		GetWorld(),
		GrabSphere->GetComponentLocation(),
		GrabSphere->GetComponentLocation() + GetActorForwardVector() * TraceDistance,
		FColor(255, 0, 0),
		false, -1, 0,
		12.333
	);
}

bool AMotionControllerActor::Grab()
{
	bWantsToGrab = true;

	// Get the closest actor overlapping the hand
	AActor* NearestActor = GetNearestActor();

	if (NearestActor->IsValidLowLevel())
	{
		// Check if the actor is grabbable
		if (NearestActor->GetClass()->ImplementsInterface(UGrabbableInterface::StaticClass()))
		{
			GrabbedActor = NearestActor;
			IGrabbableInterface::Execute_OnGrab(GrabbedActor);
			GrabbedActor->AttachToComponent(MotionController, FAttachmentTransformRules::KeepWorldTransform);
			HandMesh->SetVisibility(false);

			return true;
		}
	}
	else if (bUseLaser)
	{
		// Get the closest actor overlapping the laser
		AActor* NearestLaserActor = GetNearestLaserActor();
		
		if (NearestLaserActor->IsValidLowLevel())
		{
			// Check if the actor is grabbable
			if (NearestLaserActor->GetClass()->ImplementsInterface(UGrabbableInterface::StaticClass()))
			{
				GrabbedActor = NearestLaserActor;
				IGrabbableInterface::Execute_OnGrab(GrabbedActor);
				GrabbedActor->AttachToComponent(MotionController, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				HandMesh->SetVisibility(false);

				return true;
			}
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
			// Check if the actor is grabbable
			if (GrabbedActor->GetClass()->ImplementsInterface(UGrabbableInterface::StaticClass()))
			{
				// Drop the actor
				IGrabbableInterface::Execute_OnRelease(GrabbedActor);
				GrabbedActor->GetRootComponent()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
				GrabbedActor = nullptr;
				HandMesh->SetVisibility(true);

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

void AMotionControllerActor::ToggleLaser()
{
	// Toggle the laser on and off
	bUseLaser = !bUseLaser;
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

AActor* AMotionControllerActor::GetNearestLaserActor()
{
	// Trace for an actor in the direction of the controller
	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("Laser Trace")), true, this);
	TraceParams.bTraceComplex = true;
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = false;

	float TraceDistance = 1000.0f;

	FHitResult HitOut(ForceInit);

	GetWorld()->LineTraceSingleByChannel(
		HitOut,
		GrabSphere->GetComponentLocation(),
		GrabSphere->GetComponentLocation() + GetActorForwardVector() * TraceDistance,
		ECC_WorldDynamic,
		TraceParams
	);

	AActor* HitActor = nullptr;

	if (HitOut.Actor.IsValid())
	{
		HitActor = HitOut.GetActor();
	}

	return HitActor;
}

void AMotionControllerActor::SetHand(EControllerHand Hand)
{
	MotionController->Hand = Hand;
}

void AMotionControllerActor::SetHandMesh(USkeletalMesh* HandMeshAsset)
{
	if (HandMeshAsset->IsValidLowLevel())
	{
		HandMesh->SetSkeletalMesh(HandMeshAsset);
	}
}

void AMotionControllerActor::SetGrabRadius(float GrabRadius)
{
	GrabSphere->SetSphereRadius(GrabRadius);
}

FVector AMotionControllerActor::GetControllerLocation()
{
	return MotionController->GetComponentLocation();
}