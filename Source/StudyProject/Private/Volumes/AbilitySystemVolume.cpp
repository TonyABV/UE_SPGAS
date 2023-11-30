// Fill out your copyright notice in the Description page of Project Settings.


#include "Volumes/AbilitySystemVolume.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

AAbilitySystemVolume::AAbilitySystemVolume()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AAbilitySystemVolume::ActorEnteredVolume(AActor* Other)
{
    Super::ActorEnteredVolume(Other);

    if(!HasAuthority()) return;

    if(UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Other))
    {
        for(auto Ability : PermanentAbilitiesToGive)
        {
            AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Ability));
        }

        EnteredActorsInfoMap.Add(Other);

        for(auto Ability : OnGoingAbilitiesToGive)
        {
            FGameplayAbilitySpecHandle AbilitySpec = AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Ability));
            EnteredActorsInfoMap[Other].AppliedAbilities.Add(AbilitySpec);            
        }

        for(auto Effect : OnGoingEffectToApply)
        {
            FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();

            EffectContext.AddInstigator(Other, Other);

            FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(Effect, 1, EffectContext);
            if(SpecHandle.IsValid())
            {
                FActiveGameplayEffectHandle EffectHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
                if(EffectHandle.WasSuccessfullyApplied())
                {
                    EnteredActorsInfoMap[Other].AppliedEffects.Add(EffectHandle);
                }
            }
        }

        for(auto EventTag : GameplayEventsToSendOnEnter)
        {
            FGameplayEventData EventPayload;
            EventPayload.EventTag = EventTag;

            UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Other, EventTag, EventPayload);
        }
    }
    
}

void AAbilitySystemVolume::ActorLeavingVolume(AActor* Other)
{
    Super::ActorLeavingVolume(Other);
    
    if(!HasAuthority()) return;

    if(UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Other))
    {
        if(EnteredActorsInfoMap.Find(Other))
        {
            for(auto GameplayEffectHandle : EnteredActorsInfoMap[Other].AppliedEffects)
            {
                AbilitySystemComponent->RemoveActiveGameplayEffect(GameplayEffectHandle);
            }
            
            for(auto GameplayAbilityHandle : EnteredActorsInfoMap[Other].AppliedAbilities)
            {
            	AbilitySystemComponent->ClearAbility(GameplayAbilityHandle);
            }

            EnteredActorsInfoMap.Remove(Other);
        }

        for(auto GameplayEffect : OnExitEffectToApply)
        {
            FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();

            EffectContext.AddInstigator(Other, Other);

            FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, 1, EffectContext);
            if(SpecHandle.IsValid())
            {
                AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
            }
        }
        
        for(auto EventTag : GameplayEventsToSendOnExit)
        {
            FGameplayEventData EventPayload;
            EventPayload.EventTag = EventTag;

            UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Other, EventTag, EventPayload);
        }
    }
}

void AAbilitySystemVolume::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if(bDrawDebug)
    {
        DrawDebugBox(GetWorld(), GetActorLocation(), GetBounds().BoxExtent, FColor::Red, false, 0,0,5);
    }
}
