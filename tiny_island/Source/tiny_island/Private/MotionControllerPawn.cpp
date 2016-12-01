// Fill out your copyright notice in the Description page of Project Settings.

#include "tiny_island.h"
#include "MotionControllerPawn.h"

// Sets default values
AMotionControllerPawn::AMotionControllerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ofstream DataFile;

	SessionName = FDateTime::Now().ToString() + ".txt";

	DataFile.open(*SessionName);
	DataFile.close();

	VROrigin = CreateDefaultSubobject<USceneComponent>(TEXT("VROrigin"));
	VROrigin->AttachTo(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->AttachTo(VROrigin);
}

// Called when the game starts or when spawned
void AMotionControllerPawn::BeginPlay()
{
	Super::BeginPlay();

	IHeadMountedDisplay* HMDDevice = (IHeadMountedDisplay*)(GEngine->HMDDevice.Get());

	if (HMDDevice)
	{
		HMDDevice->ResetOrientationAndPosition();
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

	InInputComponent->BindAction("GrabLeft", IE_Pressed, this, &AMotionControllerPawn::GrabLeft);
	InInputComponent->BindAction("GrabRight", IE_Pressed, this, &AMotionControllerPawn::GrabRight);

	InInputComponent->BindAction("GrabLeft", IE_Released, this, &AMotionControllerPawn::ReleaseLeft);
	InInputComponent->BindAction("GrabRight", IE_Released, this, &AMotionControllerPawn::ReleaseRight);
}


void AMotionControllerPawn::GrabLeft()
{
	GrabLeftAction();
	AddDataPoint("Grab Left", GetActorLocation(), true);
}

void AMotionControllerPawn::GrabRight()
{
	GrabRightAction();
	AddDataPoint("Grab Right", GetActorLocation(), true);
}

void AMotionControllerPawn::ReleaseLeft()
{
	ReleaseLeftAction();
	AddDataPoint("Release Left", GetActorLocation(), true);
}

void AMotionControllerPawn::ReleaseRight()
{
	ReleaseRightAction();
	AddDataPoint("Release Right", GetActorLocation(), true);
}

void AMotionControllerPawn::AddDataPoint(FString Description, FVector Location, bool Success)
{
	ofstream DataFile;
	DataFile.open(*SessionName, ios::out | ios::app);

	if (DataFile.is_open())
	{
		DataFile << TCHAR_TO_UTF8((TEXT("%s"), *(FDateTime::Now().ToString())));
		DataFile << "\t\t";
		DataFile << TCHAR_TO_UTF8((TEXT("%s"), *Description));
		DataFile << "\r\n";
		DataFile.close();
	}
}