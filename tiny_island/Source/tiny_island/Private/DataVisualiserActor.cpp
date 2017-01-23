#include "tiny_island.h"
#include "DataVisualiserActor.h"

ADataVisualiserActor::ADataVisualiserActor():
	LeftSplinePoint(0),
	RightSplinePoint(0),
	CurrentSplineTime(0.0f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create splines
	SplineRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SplineRoot"));
	SplineRoot->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);

	LeftSpline = CreateDefaultSubobject<USplineComponent>(TEXT("LeftSpline"));
	LeftSpline->AttachToComponent(SplineRoot, FAttachmentTransformRules::KeepWorldTransform);
	LeftSpline->bVisible = true;

	PlaceholderLeft = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LeftPlaceholder"));
	PlaceholderLeft->AttachToComponent(LeftSpline, FAttachmentTransformRules::KeepWorldTransform);

	RightSpline = CreateDefaultSubobject<USplineComponent>(TEXT("RightSpline"));
	RightSpline->AttachToComponent(SplineRoot, FAttachmentTransformRules::KeepWorldTransform);
	RightSpline->bVisible = true;

	PlaceholderRight = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RightPlaceholder"));
	PlaceholderRight->AttachToComponent(RightSpline, FAttachmentTransformRules::KeepWorldTransform);
}

void ADataVisualiserActor::BeginPlay()
{
	Super::BeginPlay();

	if (PlaceholderMesh)
	{
		PlaceholderLeft->SetSkeletalMesh(PlaceholderMesh);
		PlaceholderLeft->SetWorldScale3D(FVector(1.0f, -1.0f, 1.0f));

		PlaceholderRight->SetSkeletalMesh(PlaceholderMesh);
	}

	// Initialise the analytics file
	SessionName = FPaths::GameContentDir() + SessionName;

	ifstream DataFile;
	DataFile.open(*SessionName);
	float DataStartSeconds = 0.0f;
	float LastDataSeconds;

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

			FDateTime DataTime;
			FDateTime::Parse(FString(DataLine.at(0).c_str()), DataTime);

			float DataSeconds = ToSeconds(DataTime);

			if (DataStartSeconds == 0.0f)
			{
				DataStartSeconds = DataSeconds;
			}

			FVector DataLocation;
			DataLocation.InitFromString(FString(DataLine.at(2).c_str()));

			//Process the data
			if (DataLine.at(3).find("Left") != std::string::npos)
			{
				LeftSpline->AddSplineWorldPoint(DataLocation);
				LeftSplinePointTimes.Add(DataSeconds - DataStartSeconds);
			}
			else
			{
				RightSpline->AddSplineWorldPoint(DataLocation);
				RightSplinePointTimes.Add(DataSeconds - DataStartSeconds);
			}

			DataLine.clear();

			PreviousDataLocation = DataLocation;
			LastDataSeconds = DataSeconds;
		}

		DataFile.close();
	}

	LeftSpline->Duration = (LastDataSeconds - DataStartSeconds);
	RightSpline->Duration = (LastDataSeconds - DataStartSeconds);
}

// Called every frame
void ADataVisualiserActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	CurrentSplineTime += DeltaTime;

	if (LeftSplinePoint + 1 >= LeftSplinePointTimes.Num() && RightSplinePoint + 1 >= RightSplinePointTimes.Num())
	{
		CurrentSplineTime = 0.0f;

		LeftSplinePoint = 0;
		RightSplinePoint = 0;
	} 

	if (PlaceholderLeft && LeftSplinePoint < LeftSplinePointTimes.Num())
	{
		float LastDataSeconds = LeftSplinePointTimes[LeftSplinePoint];
		float NextDataSeconds = LeftSplinePointTimes[LeftSplinePoint + 1];
		
		float LastSplinePointDistance = LeftSpline->GetDistanceAlongSplineAtSplinePoint(LeftSplinePoint);
		float NextSplinePointDistance = LeftSpline->GetDistanceAlongSplineAtSplinePoint(LeftSplinePoint + 1);
		float CurrentDistance = LastSplinePointDistance + (CurrentSplineTime - LastDataSeconds) / (NextDataSeconds - LastDataSeconds) * (NextSplinePointDistance - LastSplinePointDistance);

		PlaceholderLeft->SetRelativeLocation(LeftSpline->GetLocationAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::Local), ESplineCoordinateSpace::Local);

		if (CurrentSplineTime > LeftSplinePointTimes[LeftSplinePoint + 1])
		{
			LeftSplinePoint += 1;
		}
	}
	
	if (PlaceholderRight && RightSplinePoint < RightSplinePointTimes.Num())
	{
		float LastDataSeconds = RightSplinePointTimes[RightSplinePoint];
		float NextDataSeconds = RightSplinePointTimes[RightSplinePoint + 1];

		float LastSplinePointDistance = RightSpline->GetDistanceAlongSplineAtSplinePoint(RightSplinePoint);
		float NextSplinePointDistance = RightSpline->GetDistanceAlongSplineAtSplinePoint(RightSplinePoint + 1);
		float CurrentDistance = LastSplinePointDistance + (CurrentSplineTime - LastDataSeconds) / (NextDataSeconds - LastDataSeconds) * (NextSplinePointDistance - LastSplinePointDistance);

		PlaceholderRight->SetRelativeLocation(RightSpline->GetLocationAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::Local), ESplineCoordinateSpace::Local);

		if (CurrentSplineTime > RightSplinePointTimes[RightSplinePoint + 1])
		{
			RightSplinePoint += 1;
		}
	}
}

float ADataVisualiserActor::ToSeconds(FDateTime TimeStamp)
{
	return (TimeStamp.GetHour() * 60 * 60) + (TimeStamp.GetMinute() * 60) + TimeStamp.GetSecond();
}