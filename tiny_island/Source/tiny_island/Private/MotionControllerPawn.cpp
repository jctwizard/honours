// Fill out your copyright notice in the Description page of Project Settings.
 
#include "tiny_island.h"
#include "MotionControllerPawn.h"

// Sets default values
AMotionControllerPawn::AMotionControllerPawn() :
	BasePlayerHeight(180.0f),
	BaseGrabRadius(15.0f),
	ControllersToUse(EControllerHand::Right),
	bCanToggleLaser(false),
	bCanToggleAdaptive(false),
	bUseAdaptive(true),
	bUseLaser(true)
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set up the origin and camera
	if (!VROrigin)
	{
		VROrigin = CreateDefaultSubobject<USceneComponent>(TEXT("VROrigin"));
		VROrigin->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);

		Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
		Camera->AttachToComponent(VROrigin, FAttachmentTransformRules::KeepWorldTransform);

		// Initialise the Data Generator
		DataGenerator = CreateDefaultSubobject<UDataGeneratorComponent>( TEXT( "DataGenerator" ) );
	}
}

// Called when the game starts or when spawned
void AMotionControllerPawn::BeginPlay()
{
	Super::BeginPlay();

	// Set the tracking origin based on the headset type
	FName HMDDeviceName = UHeadMountedDisplayFunctionLibrary::GetHMDDeviceName();

	if (HMDDeviceName == "Oculus Right" || HMDDeviceName == "Steam VR")
	{
		UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);
	}
	else if (HMDDeviceName == "PSVR")
	{
		UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Eye);
	
		VROrigin->AddLocalOffset(FVector(0.0f, 0.0f, BasePlayerHeight));
	}

	// Reset the orientation with the new tracking origin
	//UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();

	// Initialise the controllers
	if (ControllersToUse != EControllerHand::Right)
	{
		LeftController = (AMotionControllerActor*)GetWorld()->SpawnActor(AMotionControllerActor::StaticClass());
		LeftController->SetOwner(this);
		LeftController->AttachToComponent(VROrigin, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		LeftController->SetHand(EControllerHand::Left);
		LeftController->SetHandMesh(HandMeshAsset);
		LeftController->SetGrabRadius(BaseGrabRadius);
		LeftController->bAdaptive = bUseAdaptive;
		LeftController->GrabMethod = bUseLaser ? EGrabMethod::GM_Laser : EGrabMethod::GM_Sphere;
	}

	if (ControllersToUse != EControllerHand::Left)
	{
		RightController = (AMotionControllerActor*)GetWorld()->SpawnActor(AMotionControllerActor::StaticClass());
		RightController->SetOwner(this);
		RightController->AttachToComponent(VROrigin, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		RightController->SetHand(EControllerHand::Right);
		RightController->SetHandMesh(HandMeshAsset);
		RightController->SetGrabRadius(BaseGrabRadius);
		RightController->bAdaptive = bUseAdaptive;
		RightController->GrabMethod = bUseLaser ? EGrabMethod::GM_Laser : EGrabMethod::GM_Sphere;
	}
}

// Called every frame
void AMotionControllerPawn::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

// Called to bind functionality to input
void AMotionControllerPawn::SetupPlayerInputComponent(class UInputComponent* InInputComponent)
{
	Super::SetupPlayerInputComponent(InInputComponent);

	// Bind controller trigger actions
	if (ControllersToUse != EControllerHand::Right)
	{
		InInputComponent->BindAction("GrabLeft", IE_Pressed, this, &AMotionControllerPawn::GrabLeft);
		InInputComponent->BindAction("GrabLeft", IE_Released, this, &AMotionControllerPawn::ReleaseLeft);

		if( bCanToggleLaser )
		{
			InInputComponent->BindAction( "ToggleLaserLeft", IE_Pressed, this, &AMotionControllerPawn::ToggleLaserLeft );
		}

		if( bCanToggleAdaptive )
		{
			InInputComponent->BindAction( "ToggleAdaptiveLeft", IE_Pressed, this, &AMotionControllerPawn::ToggleAdaptiveLeft );
		}
	}

	if (ControllersToUse != EControllerHand::Left)
	{
		InInputComponent->BindAction("GrabRight", IE_Pressed, this, &AMotionControllerPawn::GrabRight);
		InInputComponent->BindAction("GrabRight", IE_Released, this, &AMotionControllerPawn::ReleaseRight);
		
		if( bCanToggleLaser )
		{
			InInputComponent->BindAction( "ToggleLaserRight", IE_Pressed, this, &AMotionControllerPawn::ToggleLaserRight );
		}

		if (bCanToggleAdaptive )
		{
			InInputComponent->BindAction("ToggleAdaptiveRight", IE_Pressed, this, &AMotionControllerPawn::ToggleAdaptiveRight );
		}
	}
}

void AMotionControllerPawn::GrabLeft()
{
	bool Success = LeftController->Grab();
	DataGenerator->GenerateEvent("GrabLeft", Success);
}

void AMotionControllerPawn::GrabRight()
{
	bool Success = RightController->Grab();
	DataGenerator->GenerateEvent("GrabRight", Success);
}

void AMotionControllerPawn::ReleaseLeft()
{
	bool Success = LeftController->Release();
	DataGenerator->GenerateEvent("ReleaseLeft", Success);
}

void AMotionControllerPawn::ReleaseRight()
{
	bool Success = RightController->Release();
	DataGenerator->GenerateEvent("ReleaseRight", Success);
}

void AMotionControllerPawn::ToggleLaserLeft()
{
	LeftController->ToggleLaser();
	DataGenerator->GenerateEvent("ToggleLaserLeft");
}

void AMotionControllerPawn::ToggleLaserRight()
{
	RightController->ToggleLaser();
	DataGenerator->GenerateEvent("ToggleLaserRight");
}

void AMotionControllerPawn::ToggleAdaptiveLeft()
{
	LeftController->ToggleAdaptive();
	DataGenerator->GenerateEvent("ToggleAdaptiveLeft");
}

void AMotionControllerPawn::ToggleAdaptiveRight()
{
	RightController->ToggleAdaptive();
	DataGenerator->GenerateEvent("ToggleAdaptiveRight");
}

