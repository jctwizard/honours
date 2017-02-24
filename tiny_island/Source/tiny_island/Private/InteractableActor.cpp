// Fill out your copyright notice in the Description page of Project Settings.

#include "tiny_island.h"
#include "MotionControllerActor.h"
#include "InteractableActor.h"

// Sets default values
AInteractableActor::AInteractableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if( Mesh == nullptr )
	{
		Mesh = CreateDefaultSubobject<UStaticMeshComponent>( TEXT( "Mesh" ) );
		RootComponent = Mesh;

		AdaptiveMesh = CreateDefaultSubobject<UStaticMeshComponent>( TEXT( "AdaptiveMesh" ) );
		AdaptiveMesh->AttachToComponent( RootComponent, FAttachmentTransformRules::KeepWorldTransform );

		// Initialise the Data Generator
		DataGenerator = CreateDefaultSubobject<UDataGeneratorComponent>( TEXT( "DataGenerator" ) );
	}
}

// Called when the game starts or when spawned
void AInteractableActor::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::GetAllActorsOfClass( GetWorld(), AMotionControllerActor::StaticClass(), ControllerActors );
}

// Called every frame
void AInteractableActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	// Scale actor based on proximity to controller laser
	float NearestActorDistance = 100000.0f;

	for( AActor* ControllerActor : ControllerActors )
	{
		if( ControllerActor != nullptr )
		{
			FVector ControllerForward = ControllerActor->GetActorForwardVector();

			// The distance of the hit actor from the ray
			FVector ActorLocation = GetActorLocation();
			FVector ActorToController = ControllerActor->GetActorLocation() - ActorLocation;
			float ActorDistance = (ActorToController - ControllerForward * (FVector::DotProduct( ActorToController, ControllerForward ))).Size();

			// Return the closest swept actor to the ray
			if( ActorDistance < NearestActorDistance )
			{
				NearestActorDistance = ActorDistance;
			}
		}
	}

	if( bAdaptiveScaling )
	{
		float NormalisedDistance = FMath::Clamp( NearestActorDistance / 20.0f * (3.14159265359f * 0.5f), 0.0f, (3.14159265359f * 0.5f) );
		float ScaleFactor = FMath::Cos( NormalisedDistance );

		AdaptiveMesh->SetRelativeScale3D( FVector( 1, 1, 1 ) * (1.0f + 1.5f * ScaleFactor) );
	}
	else
	{
		if( NearestActorDistance < 10.0f )
		{
			AdaptiveMesh->SetRelativeScale3D( FVector( 1, 1, 1 ) * 1.1f );
		}
		else
		{
			AdaptiveMesh->SetRelativeScale3D( FVector( 1, 1, 1 ) * 0.9f );
		}
	}
}