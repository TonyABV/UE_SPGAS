// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SPTypes.h"
#include "GameplayTagContainer.h"
#include "SPCharacterMovementComponent.generated.h"

class UAbilitySystemComponent;
class UGameplayAbility;

UCLASS()
class STUDYPROJECT_API USPCharacterMovementComponent : public UCharacterMovementComponent
{
    GENERATED_BODY()

public:
    bool TryTraversal(UAbilitySystemComponent* ASC);

    virtual void BeginPlay() override;

    UFUNCTION(BlueprintPure)
    EMovementDirectionType GetMovementDirectionType() const;

    UFUNCTION(BlueprintCallable)
    void SetMovementDirectionType(EMovementDirectionType InMovementDirectionType);

    UFUNCTION()
    void OnEnforcedStrafeTagChanged(const FGameplayTag CallbackTag, int32 NewCound);

protected:
    UPROPERTY(EditDefaultsOnly)
    TArray<TSubclassOf<UGameplayAbility>> TraversalAbilitiesOrdered;

    UPROPERTY(EditAnywhere)
    EMovementDirectionType MovementDirectionType;

    void HandleMovementDirection();
};
