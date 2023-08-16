// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SPAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class STUDYPROJECT_API USPAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:

	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	class UBlendSpace* GetLocomotionBlendspace() const;

	UFUNCTION(BlueprintCallable, meta = (BlueprintThreadSafe))
	class UAnimSequenceBase* GetIdleAnimation() const;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Animation")
	class UCharacterAnimationDataAsset* DefaultAnimationData;
};
