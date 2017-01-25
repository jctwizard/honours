// Fill out your copyright notice in the Description page of Project Settings.

#include "tiny_island.h"
#include "MotionControllerPawn.h"

// Sets default values
AMotionControllerPawn::AMotionControllerPawn() :
	BasePlayerHeight(180.0f),
	BaseGrabRadius(15.0f)
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
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();

	// Initialise the controllers
	LeftController = (AMotionControllerActor*) GetWorld()->SpawnActor(AMotionControllerActor::StaticClass());
	LeftController->SetOwner(this);
	LeftController->AttachToComponent(VROrigin, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	LeftController->SetHand(EControllerHand::Left);
	LeftController->SetHandMesh(HandMeshAsset);
	LeftController->SetGrabRadius(BaseGrabRadius);

	RightController = (AMotionControllerActor*) GetWorld()->SpawnActor(AMotionControllerActor::StaticClass());
	RightController->SetOwner(this);
	RightController->AttachToComponent(VROrigin, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	RightController->SetHand(EControllerHand::Right);
	RightController->SetHandMesh(HandMeshAsset);
	RightController->SetGrabRadius(BaseGrabRadius);
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
	InInputComponent->BindAction("GrabLeft", IE_Pressed, this, &AMotionControllerPawn::GrabLeft);
	InInputComponent->BindAction("GrabRight", IE_Pressed, this, &AMotionControllerPawn::GrabRight);

	InInputComponent->BindAction("GrabLeft", IE_Released, this, &AMotionControllerPawn::ReleaseLeft);
	InInputComponent->BindAction("GrabRight", IE_Released, this, &AMotionControllerPawn::ReleaseRight);

	InInputComponent->BindAction("ToggleLaserLeft", IE_Pressed, this, &AMotionControllerPawn::ToggleLaserLeft);
	InInputComponent->BindAction("ToggleLaserRight", IE_Pressed, this, &AMotionControllerPawn::ToggleLaserRight);
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
	DataGenerator->GenerateEvent("ToggleLaseLeft");
}

void AMotionControllerPawn::ToggleLaserRight()
{
	RightController->ToggleLaser();
	DataGenerator->GenerateEvent("ToggleLaserRight");
}
