// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SPCharacterMovementComponent.generated.h"

class UAbilitySystemComponent;
class UGameplayAbility;

UCLASS()
class STUDYPROJECT_API USPCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:

	bool TryTraversal(UAbilitySystemComponent* ASC);

protected:

	UPROPERTY(EditDefaultsOnly)
    TArray<TSubclassOf<UGameplayAbility>> TraversalAbilitiesOrdered;

};
