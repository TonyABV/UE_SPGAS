// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AnimNotify_GameplayEvent.h"

#include "AbilitySystemBlueprintLibrary.h"

void UAnimNotify_GameplayEvent::Notify(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation)
{
    Super::Notify(MeshComponent, Animation);

    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComponent->GetOwner(), Payload.EventTag, Payload);
    
}
