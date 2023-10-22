// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/SPGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemLog.h"
#include "StudyProjectCharacter.h"

void USPGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                         const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	FGameplayEffectContextHandle ContextHandle = ActorInfo->AbilitySystemComponent->MakeEffectContext();
	
	if(IsInstantiated())
	{
		for (TSubclassOf<UGameplayEffect> GameplayEffect : OngoingEffectsToRemoveOnEnd)
		{
			if (!GameplayEffect.Get()) continue;
			if (UAbilitySystemComponent* AbilitySystem = ActorInfo->AbilitySystemComponent.Get())
			{
				FGameplayEffectSpecHandle SpecHandle = ActorInfo->AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, 1, ContextHandle);
				if (SpecHandle.IsValid())
				{
					FActiveGameplayEffectHandle ActiveGEHandle = //
						ActorInfo->AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
					if (!ActiveGEHandle.WasSuccessfullyApplied())
					{
						ABILITY_LOG(Log, TEXT("Ability %s failed to apply runtime effect %s"), *GetNameSafe(GameplayEffect));
					}
					else
					{
						RemoveOnEndEffectHandle.Add(ActiveGEHandle);
					}
				}
			}
		}
	}

    for (TSubclassOf<UGameplayEffect> GameplayEffect : OngoingEffectsToJustApplyOnStart)
    {
        if (!GameplayEffect.Get()) continue;

        if (UAbilitySystemComponent* AbilitySystem = ActorInfo->AbilitySystemComponent.Get())
        {
            FGameplayEffectSpecHandle SpecHandle = AbilitySystem->MakeOutgoingSpec(GameplayEffect, 1, ContextHandle);
            if (SpecHandle.IsValid())
            {
                FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystem->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
                if (!ActiveGEHandle.WasSuccessfullyApplied())
                {
                    ABILITY_LOG(Log, TEXT("Ability %s failed to apply startup effect %s"), *GetNameSafe(GameplayEffect));
                }
            }
        }
    }
}

void USPGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if(IsInstantiated())
	{
		for(FActiveGameplayEffectHandle ActiveEffectHandle : RemoveOnEndEffectHandle)
		{
			if (UAbilitySystemComponent* AbilitySystem = ActorInfo->AbilitySystemComponent.Get())
			{
				if(ActiveEffectHandle.IsValid())
				{
					ActorInfo->AbilitySystemComponent->RemoveActiveGameplayEffect(ActiveEffectHandle);
				}
			}
		}

		RemoveOnEndEffectHandle.Empty();
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

AStudyProjectCharacter* USPGameplayAbility::GetSPCharacter() const
{
    AStudyProjectCharacter* Result = Cast<AStudyProjectCharacter>(GetAvatarActorFromActorInfo());
    return Result;
}
