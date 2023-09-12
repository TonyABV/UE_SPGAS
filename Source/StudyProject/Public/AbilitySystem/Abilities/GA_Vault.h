// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SPGameplayAbility.h"
#include "GA_Vault.generated.h"

class UAnimMontage;
class UAbilityTask_PlayMontageAndWait;

UCLASS()
class STUDYPROJECT_API UGA_Vault : public USPGameplayAbility
{
    GENERATED_BODY()

    UGA_Vault();

    virtual bool CommitCheck(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, FGameplayTagContainer* OptionalRelevantTags) override;

    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
    UPROPERTY(EditDefaultsOnly, Category = HorizontalTrace)
    float HorizontalTraceRadius = 30.f;
    UPROPERTY(EditDefaultsOnly, Category = HorizontalTrace)
    float HorizontalTraceLength = 500.f;
    UPROPERTY(EditDefaultsOnly, Category = HorizontalTrace)
    float HorizontalTraceCount = 5.f;
    UPROPERTY(EditDefaultsOnly, Category = HorizontalTrace)
    float HorizontalTraceStep = 30.f;

    UPROPERTY(EditDefaultsOnly, Category = HorizontalTrace)
    float VerticalTraceRadius = 30.f;
    UPROPERTY(EditDefaultsOnly, Category = HorizontalTrace)
    float VerticalTraceStep = 30.f;

    UPROPERTY(EditDefaultsOnly, Category = HorizontalTrace)
    TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectType;

    UPROPERTY(EditDefaultsOnly)
    TArray<TEnumAsByte<ECollisionChannel>> CollisionChanelToIgnore;

    UPROPERTY(EditDefaultsOnly)
    UAnimMontage* VaultMontage = nullptr;

    UPROPERTY()
    UAbilityTask_PlayMontageAndWait* MontageTask = nullptr;

    FVector JumpToLocation;
    FVector JumpOverLocation;

};
