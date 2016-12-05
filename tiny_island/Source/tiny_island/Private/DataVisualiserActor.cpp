// Fill out your copyright notice in the Description page of Project Settings.

#include "tiny_island.h"
#include "DataVisualiserActor.h"


// Sets default values
ADataVisualiserActor::ADataVisualiserActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create splines
	SplineRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SplineRoot"));
	SplineRoot->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);

	LeftSpline = CreateDefaultSubobject<USplineComponent>(TEXT("LeftSpline"));
	LeftSpline->AttachToComponent(SplineRoot, FAttachmentTransformRules::KeepWorldTransform);

	PlaceholderLeft = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LeftPlaceholder"));
	PlaceholderLeft->AttachToComponent(LeftSpline, FAttachmentTransformRules::KeepWorldTransform);

	RightSpline = CreateDefaultSubobject<USplineComponent>(TEXT("RightSpline"));
	RightSpline->AttachToComponent(SplineRoot, FAttachmentTransformRules::KeepWorldTransform);

	PlaceholderRight = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RightPlaceholder"));
	PlaceholderRight->AttachToComponent(RightSpline, FAttachmentTransformRules::KeepWorldTransform);
}

// Called when the game starts or when spawned
void ADataVisualiserActor::BeginPlay()
{
	Super::BeginPlay();

	if (PlaceholderMesh)
	{
		PlaceholderLeft->SetSkeletalMesh(PlaceholderMesh);
		PlaceholderLeft->SetWorldScale3D(FVector(1.0f, 1.0f, -1.0f));

		PlaceholderRight->SetSkeletalMesh(PlaceholderMesh);
	}

	// Initialise the analytics file
	SessionName = FPaths::GameContentDir() + SessionName;

	ifstream DataFile;
	DataFile.open(*SessionName);

	// Open the analytics file and save the action to it along with the timestamp, success type and location
	if (DataFile.is_open())
	{
		std::string DataPoint;
		std::vector<std::string> DataLine;

		FVector PreviousDataLocation;

		while (std::getline(DataFile, DataPoint))
		{
			for (size_t p = 0, q = 0; p != DataPoint.npos; p = q)
			{
				DataLine.push_back(DataPoint.substr(p + (p != 0), (q = DataPoint.find("\t\t", p + 1)) - p - (p != 0)));
			}

			FVector DataLocation;
			DataLocation.InitFromString(FString(DataLine.at(2).c_str()));

			//Process the data
			if (DataLine.at(3).find("Left") != std::string::npos)
			{
				LeftSpline->AddSplineWorldPoint(DataLocation);
			}
			else
			{
				RightSpline->AddSplineWorldPoint(DataLocation);
			}

			DataLine.clear();

			PreviousDataLocation = DataLocation;
		}

		DataFile.close();
	}

	LeftSpline->Duration = 1.0f * LeftSpline->GetNumberOfSplinePoints();
	RightSpline->Duration = 1.0f * RightSpline->GetNumberOfSplinePoints();
}

// Called every frame
void ADataVisualiserActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	ElapsedTime += DeltaTime;

	if (PlaceholderLeft)
	{
		PlaceholderLeft->SetRelativeLocation(LeftSpline->GetLocationAtTime(ElapsedTime, ESplineCoordinateSpace::Local));
	}

	if (PlaceholderRight)
	{
		PlaceholderRight->SetRelativeLocation(RightSpline->GetLocationAtTime(ElapsedTime, ESplineCoordinateSpace::Local));
	}
}