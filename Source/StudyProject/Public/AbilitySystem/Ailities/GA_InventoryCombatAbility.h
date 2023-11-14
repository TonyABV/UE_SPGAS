// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ailities/GA_InventoryAbility.h"
#include "GA_InventoryCombatAbility.generated.h"

/**
 * 
 */
UCLASS()
class STUDYPROJECT_API UGA_InventoryCombatAbility : public UGA_InventoryAbility
{
	GENERATED_BODY()

public:

    UFUNCTION(BlueprintPure)
    FGameplayEffectSpecHandle GetWeaponEffectSpec(const FHitResult& InHitResult);
    
    UFUNCTION(BlueprintPure)
    const bool GetWeaponToFocusTraceResult(float TraceDistance, ETraceTypeQuery TraceType, FHitResult& OutHitResult);
    
};
