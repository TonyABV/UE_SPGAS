// Fill out your copyright notice in the Description page of Project Settings.

#include "ActorComponents/SPCharacterMovementComponent.h"

#include "AbilitySystemComponent.h"

static TAutoConsoleVariable<int32> CVarShowTraversal(
    TEXT("ShowDebugTraversal"), //
    0, //
    TEXT("Draws debug info about traversal"), //
    ECVF_Cheat);

bool USPCharacterMovementComponent::TryTraversal(UAbilitySystemComponent* ASC)
{
    for (TSubclassOf<UGameplayAbility> AbilityClass : TraversalAbilitiesOrdered)
    {
        if (ASC->TryActivateAbilityByClass(AbilityClass))
        {
            const FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromClass(AbilityClass);

            if (Spec && Spec->IsActive())
            {
                return true;
            }
        }
    }
    return false;
}
