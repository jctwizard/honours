// Fill out your copyright notice in the Description page of Project Settings.

#include "tiny_island.h"
#include "InteractableActor.h"
#include "MotionControllerActor.h"

// Sets default values
AMotionControllerActor::AMotionControllerActor() :
	bWantsToGrab(false),
	bAdaptive(false),
	GrabbedActor(nullptr),
	GrabMethod(EGrabMethod::GM_Laser)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!MotionController)
	{
		// Create the motion controller component and attach it to the root
		MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionController"));
		MotionController->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);

		// Create the hand mesh and attach it to the root component
		HandMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HandMesh"));
		HandMesh->AttachToComponent(MotionController, FAttachmentTransformRules::KeepWorldTransform);
		HandMesh->SetCollisionEnabled( ECollisionEnabled::NoCollision );

		// Create the grab sphere and attach it to the hand mesh
		GrabSphere = CreateDefaultSubobject<USphereComponent>(TEXT("GrabSphere"));
		GrabSphere->AttachToComponent(HandMesh, FAttachmentTransformRules::KeepWorldTransform);

		// Initialise the Data Generator
		DataGenerator = CreateDefaultSubobject<UDataGeneratorComponent>( TEXT( "DataGenerator" ) );
	}
}

// Called when the game starts or when spawned
void AMotionControllerActor::BeginPlay()
{
	Super::BeginPlay();

	// Flip the Mesh of the left hand
	if (MotionController->Hand == EControllerHand::Left)
	{
		HandMesh->SetWorldScale3D(FVector(1.0f, -1.0f, 1.0f));
	}
}

// Called every frame
void AMotionControllerActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (GrabMethod == EGrabMethod::GM_Laser)
	{
		//LaserParticle->SetWorldTransform(GetTransform());
		//DrawLaser();
	}
}

void AMotionControllerActor::DrawLaser()
{
	FColor LaserColour = FColor(255, 0, 0);

	if (bAdaptive)
	{
		LaserColour = FColor(255, 127, 0);
	}

	// Draw a line in the direction of the controller
	DrawDebugLine(
		GetWorld(),
		GrabSphere->GetComponentLocation(),
		GrabSphere->GetComponentLocation() + GetActorForwardVector() * 1000.0f,
		LaserColour,
		false, -1, 0,
		0.5f
	);
}

bool AMotionControllerActor::Grab()
{
	bWantsToGrab = true;

	AActor* NearestActor = nullptr;
	FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules::KeepWorldTransform;

	// Get the nearest actor based on the grab method
	switch (GrabMethod)
	{
		case EGrabMethod::GM_Sphere:
		{
			// Get the closest actor overlapping the sphere
			NearestActor = GetNearestActor();

			AttachmentRules = FAttachmentTransformRules::KeepWorldTransform;
			break;
		}
		case EGrabMethod::GM_Laser:
		{
			// Get the closest actor overlapping the laser
			NearestActor = GetNearestLaserActor();

			if (bAdaptive && NearestActor == nullptr)
			{
				NearestActor = GetNearestActorAdaptive();
			}

			AttachmentRules = FAttachmentTransformRules::SnapToTargetNotIncludingScale;
			break;
		}
	}

	if (NearestActor->IsValidLowLevel())
	{
		// Check if the actor is grabbable
		if (NearestActor->GetClass()->ImplementsInterface(UGrabbableInterface::StaticClass()))
		{
			GrabbedActor = NearestActor;
			IGrabbableInterface::Execute_OnGrab(GrabbedActor);
			GrabbedActor->AttachToComponent(MotionController, AttachmentRules);
			HandMesh->SetVisibility(false);

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
	if (GrabMethod == EGrabMethod::GM_Laser)
	{
		GrabMethod = EGrabMethod::GM_Sphere;
	}
	else
	{
		GrabMethod = EGrabMethod::GM_Laser;
	}
}

void AMotionControllerActor::ToggleAdaptive()
{
	bAdaptive = !bAdaptive;
}

AActor* AMotionControllerActor::GetNearestActor()
{
	TArray<AActor*> NearbyActors;
	GrabSphere->GetOverlappingActors(NearbyActors, AActor::StaticClass());

	float NearestOverlap = 10000.0f;
	AActor* NearestOverlappingActor = nullptr;

	// Check each overlapping actor and get the nearest one
	for (AActor* NearbyActor : NearbyActors)
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

	float TraceDistance = 300.0f;

	TArray<FHitResult> HitsOut;

	GetWorld()->LineTraceMultiByChannel(
		HitsOut,
		GrabSphere->GetComponentLocation(),
		GrabSphere->GetComponentLocation() + GetActorForwardVector() * TraceDistance,
		ECC_PhysicsBody,
		TraceParams
	);

	AActor* HitActor = nullptr;

	for (FHitResult HitOut : HitsOut)
	{
		AActor* HitOutActor = HitOut.GetActor();
		
		if (HitOutActor != nullptr)
		{
			if (HitOutActor->GetClass()->ImplementsInterface(UGrabbableInterface::StaticClass()))
			{
				HitActor = HitOutActor;
				break;
			}
		}
	}

	return HitActor;
}

AActor* AMotionControllerActor::GetNearestActorAdaptive()
{
	float AdaptiveRadius = 100.0f;

	FVector LaserStart = GrabSphere->GetComponentLocation();
	FVector LaserEnd = GrabSphere->GetComponentLocation() + GetActorForwardVector();

	TArray<AActor*> InteractableActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AInteractableActor::StaticClass(), InteractableActors);

	AActor* NearestActor = nullptr;
	float NearestActorDistance = 10000.0f;

	FVector LaserNormal = LaserEnd - LaserStart;
	LaserNormal.Normalize();

	for (AActor* InteractableActor : InteractableActors)
	{
		if (InteractableActor != nullptr)
		{
			if (InteractableActor->GetClass()->ImplementsInterface(UGrabbableInterface::StaticClass()))
			{
				// The distance of the hit actor from the ray
				FVector ActorLocation = InteractableActor->GetActorLocation();
				FVector ActorToLaserStart = LaserStart - ActorLocation;
				float ActorDistance = (ActorToLaserStart - LaserNormal * (FVector::DotProduct( ActorToLaserStart, LaserNormal ))).Size();

				// Return the closest swept actor to the ray
				if (ActorDistance < NearestActorDistance && ActorDistance < AdaptiveRadius)
				{
					NearestActor = InteractableActor;
					NearestActorDistance = ActorDistance;
				}
			}
		}
	}

	return NearestActor;
}

void AMotionControllerActor::SetHand(EControllerHand Hand)
{
	MotionController->Hand = Hand;
}

void AMotionControllerActor::SetHandMesh(UStaticMesh* HandMeshAsset)
{
	if (HandMeshAsset->IsValidLowLevel())
	{
		HandMesh->SetStaticMesh(HandMeshAsset);
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