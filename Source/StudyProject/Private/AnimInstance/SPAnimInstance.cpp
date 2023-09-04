// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/SPAnimInstance.h"
#include "StudyProjectCharacter.h"
#include "Animation/AnimSequenceBase.h"
#include "DataAssets/CharacterAnimationDataAsset.h"

UBlendSpace* USPAnimInstance::GetLocomotionBlendSpace() const
{
	const AStudyProjectCharacter* Character = Cast<AStudyProjectCharacter>(GetOwningActor());
	if(Character && Character->GetCharacterData().CharacterAnimData)
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
    return DefaultAnimationData ? DefaultAnimationData->CharacterAnimationData.MovementBlendSpace : nullptr;
}

UAnimSequenceBase* USPAnimInstance::GetIdleAnimation() const
{
	const AStudyProjectCharacter* Character = Cast<AStudyProjectCharacter>(GetOwningActor());
	if(Character && Character->GetCharacterData().CharacterAnimData)
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
	return DefaultAnimationData ? DefaultAnimationData->CharacterAnimationData.IdleAnimationAsset : nullptr;
}
