// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "AnimNotify_GameplayEvent.generated.h"

UCLASS()
class STUDYPROJECT_API UAnimNotify_GameplayEvent : public UAnimNotify
{
    GENERATED_BODY()

public:
    virtual void Notify(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation) override;

protected:
    UPROPERTY(EditAnywhere)
    FGameplayEventData Payload;
};
