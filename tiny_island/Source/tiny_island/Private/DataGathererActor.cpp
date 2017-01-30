// Fill out your copyright notice in the Description page of Project Settings.

#include "tiny_island.h"
#include "DataGathererActor.h"


// Sets default values
ADataGathererActor::ADataGathererActor() :
	ElapsedTime(0.0f),
	DataTickTime(0.0f),
	DataTickDuration(0.1f),
	CurrentDataTick(0),
	DataUpdateThreshold(10.0f),
	bVisualiseData(false)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

ADataGathererActor::~ADataGathererActor()
{
	if (XmlFile != nullptr)
	{
		delete XmlFile;
	}
}

// Called when the game starts or when spawned
void ADataGathererActor::BeginPlay()
{
	Super::BeginPlay();

	if (bVisualiseData)
	{
		XmlFile = new FXmlFile();
		XmlFile->LoadFile(FPaths::GameContentDir() + SessionName);
		RootNode = XmlFile->GetRootNode();

		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), TrackedActors);
	}
	else
	{
		// Initialise the analytics file
		SessionName = FPaths::GameContentDir() + FDateTime::Now().ToString() + ".xml";

		const FString FileTemplate = "<?xml version=\"1.0\" encoding=\"UTF - 8\"?>\n<Session>\n</Session>";
		XmlFile = new FXmlFile( FileTemplate, EConstructMethod::ConstructFromBuffer );

		RootNode = XmlFile->GetRootNode();
	}
}

// Called every frame
void ADataGathererActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	ElapsedTime += DeltaTime;

	DataTickTime += DeltaTime;

	if (DataTickTime >= DataTickDuration)
	{
		DataTickTime = DataTickTime - DataTickDuration;

		CurrentDataTick += 1;

		if (bVisualiseData)
		{
			VisualiseActorData();
		}
		else
		{
			GatherActorData();
		}
	}
}

void ADataGathererActor::GatherActorData()
{
	// Add each tracked actor to the tick node if it has moved enough
	for (int TrackedActorIndex = 0; TrackedActorIndex < TrackedActors.Num(); TrackedActorIndex++)
	{
		// Add transform data of actor to timeline
		AActor* TrackedActor = TrackedActors[TrackedActorIndex];

		// Only update if the actor has moved more than the threshold
		if ((TrackedActor->GetActorLocation() - TrackedActorLocations[TrackedActorIndex]).Size() > DataUpdateThreshold)
		{
			GetActorNode(TrackedActor, CurrentDataTick)->SetContent(TrackedActor->GetTransform().ToString());

			TrackedActorLocations[TrackedActorIndex] = TrackedActor->GetActorLocation();
		}
	}

	XmlFile->Save(SessionName);
}

void ADataGathererActor::VisualiseActorData()
{
	if (NodeExists(RootNode, FString::FromInt(CurrentDataTick)))
	{
		FXmlNode* TickNode = GetNode(RootNode, FString::FromInt(CurrentDataTick));

		// Go through each actor getting its transform from the xml file
		for (int TrackedActorIndex = 0; TrackedActorIndex < TrackedActors.Num(); TrackedActorIndex++)
		{
			// Update actor based on data
			AActor* TrackedActor = TrackedActors[TrackedActorIndex];

			if (TrackedActor != nullptr)
			{
				FString ActorName = TrackedActor->GetName();

				if (NodeExists(TickNode, ActorName))
				{
					FXmlNode* ActorNode = GetNode( TickNode, ActorName );
					FTransform TickTransform;
					TickTransform.InitFromString( ActorNode->GetContent());

					// Set the actors transform from the node
					TrackedActor->SetActorTransform(TickTransform);

					// Check for any event nodes
					TArray<FXmlNode*> EventNodes = ActorNode->GetChildrenNodes();

					for( FXmlNode* EventNode : EventNodes )
					{
						FString Success = "successful";
						
						if (!FCString::Atoi(*EventNode->GetContent()))
						{
							Success = "unsuccessful";
						}

						UE_LOG( LogTemp, Warning, TEXT( "A %s %s occurred at tick %d" ), *Success, *EventNode->GetTag(), int(CurrentDataTick) );
					}
				}
			}
		}
	}
}

bool ADataGathererActor::NodeExists(FXmlNode* ParentNode, FString NodeName)
{
	if (ParentNode->FindChildNode(NodeName) == nullptr)
	{
		return false;
	}
	else
	{
		return true;
	}
}

FXmlNode* ADataGathererActor::GetNode(FXmlNode* ParentNode, FString NodeName)
{
	if (NodeExists(ParentNode, NodeName) == false)
	{
		ParentNode->AppendChildNode(NodeName, "");
	}

	return ParentNode->FindChildNode(NodeName);
}

FXmlNode* ADataGathererActor::GetActorNode(AActor* TrackedActor, int NodeTick)
{
	// Add new tick node
	FString ActorName = TrackedActor->GetName();
	FXmlNode* TickNode = GetNode(RootNode, FString::FromInt(NodeTick));
	return GetNode(TickNode, ActorName);
}

void ADataGathererActor::GatherEvent(AActor* Actor, FString Label, bool Success)
{
	if (bVisualiseData == false)
	{
		FXmlNode* ActorNode = GetActorNode(Actor, CurrentDataTick);

		ActorNode->AppendChildNode(Label, FString::FromInt(Success));
	}
}

void ADataGathererActor::TrackActor(AActor* Actor)
{
	if (bVisualiseData == false)
	{
		// Add the reference to the actor and store its location
		TrackedActors.Add(Actor);
		TrackedActorLocations.Add(Actor->GetActorLocation());
	}
}