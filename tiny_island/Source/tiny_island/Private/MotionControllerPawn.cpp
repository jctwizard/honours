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

	// Initialise the analytics file
	SessionName = FPaths::GameContentDir() + FDateTime::Now().ToString() + ".txt";

	// Set up the origin and camera
	if (!VROrigin)
	{
		VROrigin = CreateDefaultSubobject<USceneComponent>(TEXT("VROrigin"));
		VROrigin->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);

		Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
		Camera->AttachToComponent(VROrigin, FAttachmentTransformRules::KeepWorldTransform);
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
}

void AMotionControllerPawn::GrabLeft()
{
	bool Success = LeftController->Grab();
	AddDataPoint("Grab Left", LeftController->GetControllerLocation(), Success);
}

void AMotionControllerPawn::GrabRight()
{
	bool Success = RightController->Grab();
	AddDataPoint("Grab Right", LeftController->GetControllerLocation(), Success);
}

void AMotionControllerPawn::ReleaseLeft()
{
	bool Success = LeftController->Release();
	AddDataPoint("Release Left", RightController->GetControllerLocation(), Success);
}

void AMotionControllerPawn::ReleaseRight()
{
	bool Success = RightController->Release();
	AddDataPoint("Release Right", RightController->GetControllerLocation(), Success);
}

void AMotionControllerPawn::AddDataPoint(FString Description, FVector Location, bool Success)
{
	ofstream DataFile;
	DataFile.open(*SessionName, ios::out | ios::app);

	// Open the analytics file and save the action to it along with the timestamp, success type and location
	if (DataFile.is_open())
	{
		DataFile << TCHAR_TO_UTF8((TEXT("%s"), *(FDateTime::Now().ToString())));

		DataFile << "\t\t";

		if (Success)
		{
			DataFile << "Success";
		}
		else
		{
			DataFile << "Failed";
		}

		DataFile << "\t\t";
		DataFile << TCHAR_TO_UTF8((TEXT("%s"), *(Location.ToString())));

		DataFile << "\t\t";
		DataFile << TCHAR_TO_UTF8((TEXT("%s"), *Description));

		DataFile << "\r\n";
		DataFile.close();
	}
}