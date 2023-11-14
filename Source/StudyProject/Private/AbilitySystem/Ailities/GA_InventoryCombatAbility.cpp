// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Ailities/GA_InventoryCombatAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "SPTypes.h"
#include "Actors/WeaponItemActor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "StudyProjectCharacter.h"
#include "Camera/CameraComponent.h"

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

const bool UGA_InventoryCombatAbility::GetWeaponToFocusTraceResult(float TraceDistance, ETraceTypeQuery TraceType, FHitResult& OutHitResult)
{
    AWeaponItemActor* WeaponItemActor = GetEquippedWeaponItemActor();
    
    AStudyProjectCharacter* StudyProjectCharacter = GetSPCharacter();
    
    const FTransform& CameraTransform = StudyProjectCharacter->GetFollowCamera()->GetComponentTransform();
    const FVector FocusTraceEnd = CameraTransform.GetLocation() + CameraTransform.GetRotation().Vector() * TraceDistance;
    
    TArray<AActor*> ActorsToIgnore = {GetAvatarActorFromActorInfo()};
    
    FHitResult FocusHit;
    
    UKismetSystemLibrary::LineTraceSingle(this, CameraTransform.GetLocation(), FocusTraceEnd, TraceType, false, ActorsToIgnore, //
        EDrawDebugTrace::None, FocusHit, true);

    FVector MuzzleLocation = WeaponItemActor->GetMuzzleLocation();
    
    const FVector WeaponTraceEnd = MuzzleLocation + (FocusHit.Location - MuzzleLocation).GetSafeNormal() * TraceDistance;

    UKismetSystemLibrary::LineTraceSingle(this, MuzzleLocation, WeaponTraceEnd, TraceType, //
        false, ActorsToIgnore, EDrawDebugTrace::None, OutHitResult, true);

    return OutHitResult.bBlockingHit;
    
}
