// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/SPGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemLog.h"
#include "StudyProjectCharacter.h"
#include "AbilitySystemLog.h"

void USPGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    FGameplayEffectContextHandle EffectContext = ActorInfo->AbilitySystemComponent->MakeEffectContext();

    for (auto GameplayEffect : OngoingEffectsToJustApplyOnStart)
    {
        if (!GameplayEffect.Get()) continue;

        if (UAbilitySystemComponent* AbilityComponent = ActorInfo->AbilitySystemComponent.Get())
        {
            FGameplayEffectSpecHandle SpecHandle = AbilityComponent->MakeOutgoingSpec(GameplayEffect, 1, EffectContext);
            if (SpecHandle.IsValid())
            {
                FActiveGameplayEffectHandle ActiveGEHandle = AbilityComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
                if (!ActiveGEHandle.WasSuccessfullyApplied())
                {
                    ABILITY_LOG(Log, TEXT("Ability %s failed to apply startup effect %s"), *GetName(), *GetNameSafe(GameplayEffect));
                }
            }
        }
    }

    if (IsInstantiated())
    {
        for (auto GameplayEffect : OngoingEffectsToRemoveOnEnd)
        {
            if (!GameplayEffect.Get()) continue;

            if (UAbilitySystemComponent* AbilityComponent = ActorInfo->AbilitySystemComponent.Get())
            {
                FGameplayEffectSpecHandle SpecHandle = AbilityComponent->MakeOutgoingSpec(GameplayEffect, 1, EffectContext);
                if (SpecHandle.IsValid())
                {
                    FActiveGameplayEffectHandle ActiveGEHandle = AbilityComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
                    if (!ActiveGEHandle.WasSuccessfullyApplied())
                    {
                        ABILITY_LOG(Log, TEXT("Ability %s failed to apply runtime effect %s"), *GetName(), *GetNameSafe(GameplayEffect));
                    }
                    else
                    {
                        RemoveOnEndEffectHandles.Add(ActiveGEHandle);
                    }
                }
            }
        }
    }
}

void USPGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    if (IsInstantiated())
    {
        for (FActiveGameplayEffectHandle ActiveEffectHandle : RemoveOnEndEffectHandles)
        {
            if (ActiveEffectHandle.IsValid())
            {
                ActorInfo->AbilitySystemComponent->RemoveActiveGameplayEffect(ActiveEffectHandle);
            }
        }

        RemoveOnEndEffectHandles.Empty();
    }

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

AStudyProjectCharacter* USPGameplayAbility::GetSPCharacter() const
{
    AStudyProjectCharacter* Result = Cast<AStudyProjectCharacter>(GetAvatarActorFromActorInfo());
    return Result;
}
