// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SPTypes.h"
#include "CharacterAnimationDataAsset.generated.h"

/**
 *
 */
UCLASS()
class STUDYPROJECT_API UCharacterAnimationDataAsset : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly)
    FCharacterAnimationData CharacterAnimationData;
};
