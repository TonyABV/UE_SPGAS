// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SPTypes.h"
#include "SPStatics.generated.h"

/**
 * 
 */
UCLASS()
class STUDYPROJECT_API USPStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintPure)
    static const UItemStaticData* GetItemStaticData(TSubclassOf<UItemStaticData> ItemDataClass);

};
