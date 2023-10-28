// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/SPAnimInstance.h"
#include "StudyProjectCharacter.h"
#include "Animation/AnimSequenceBase.h"
#include "DataAssets/CharacterAnimationDataAsset.h"
#include "ActorComponents/InventoryComponent.h"
#include "Inventory/InventoryItemInstance.h"

const UItemStaticData* USPAnimInstance::GetEquippedItemData() const
{
    AStudyProjectCharacter* Character = Cast<AStudyProjectCharacter>(GetOwningActor());
    UInventoryComponent* InventoryComponent = IsValid(Character) ? Character->GetInventoryComponent() : nullptr;
    UInventoryItemInstance* ItemInstance = IsValid(InventoryComponent) ? InventoryComponent->GetCurrentEquippedItem() : nullptr;

    return IsValid(ItemInstance) ? ItemInstance->GetItemStaticData() : nullptr;
}

UBlendSpace* USPAnimInstance::GetLocomotionBlendSpace() const
{
    const AStudyProjectCharacter* Character = Cast<AStudyProjectCharacter>(GetOwningActor());

    if(IsValid(Character))
    {
        if(const UItemStaticData* ItemStaticData = GetEquippedItemData())
        {
            if (Character->bIsCrouched)
            {
                return ItemStaticData->CharacterAnimationData.CrouchingBlendSpace;
            }
            else
            {
                return ItemStaticData->CharacterAnimationData.MovementBlendSpace;
            }
        }
    
        if(Character->GetCharacterData().CharacterAnimData)
        {
            if (Character->bIsCrouched)
            {
                return Character->GetCharacterData().CharacterAnimData->CharacterAnimationData.CrouchingBlendSpace;
            }
            else
            {
                return Character->GetCharacterData().CharacterAnimData->CharacterAnimationData.MovementBlendSpace;
            }
        }
    }
    
    return DefaultAnimationData ? DefaultAnimationData->CharacterAnimationData.MovementBlendSpace : nullptr;
}

UAnimSequenceBase* USPAnimInstance::GetIdleAnimation() const
{
	
	
	const AStudyProjectCharacter* Character = Cast<AStudyProjectCharacter>(GetOwningActor());

    if(IsValid(Character))
    {
        if(const UItemStaticData* ItemStaticData = GetEquippedItemData())
        {
            if (Character->bIsCrouched)
            {
                return ItemStaticData->CharacterAnimationData.CrouchIdleAnimationAsset;
            }
            else
            {
                return ItemStaticData->CharacterAnimationData.IdleAnimationAsset;
            }
        }
	
        if(Character->GetCharacterData().CharacterAnimData)
        {
            if (Character->bIsCrouched)
            {
                return Character->GetCharacterData().CharacterAnimData->CharacterAnimationData.CrouchIdleAnimationAsset;
            }
            else
            {
                return Character->GetCharacterData().CharacterAnimData->CharacterAnimationData.IdleAnimationAsset;
            }
        }
    }
	return DefaultAnimationData ? DefaultAnimationData->CharacterAnimationData.IdleAnimationAsset : nullptr;
}
