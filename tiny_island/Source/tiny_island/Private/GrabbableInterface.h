// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GrabbableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UGrabbableInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class IGrabbableInterface
{
	GENERATED_IINTERFACE_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grabbing")
	void OnGrab();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Grabbing")
	void OnRelease();
};
