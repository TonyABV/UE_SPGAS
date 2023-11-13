// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Ailities/GA_InventoryCombatAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "SPTypes.h"

FGameplayEffectSpecHandle UGA_InventoryCombatAbility::GetWeaponEffectSpec(const FHitResult& InHitResult)
{
    if (UAbilitySystemComponent* AbilityComponent = GetAbilitySystemComponentFromActorInfo())
    {
        if (const UWeaponStaticData* WeaponStaticData = GetEquippedWeaponStaticData())
        {
            FGameplayEffectContextHandle EffectContext = AbilityComponent->MakeEffectContext();

            FGameplayEffectSpecHandle OutSpec = AbilityComponent->MakeOutgoingSpec(WeaponStaticData->DamageEffect, 1, EffectContext);

            UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(OutSpec, FGameplayTag::RequestGameplayTag(TEXT("Attribute.Health")), -WeaponStaticData->BaseDamage);

            return OutSpec;
        }
    }

    return FGameplayEffectSpecHandle();
}
