// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/SPAnimInstance.h"
#include "StudyProjectCharacter.h"
#include "Animation/AnimSequenceBase.h"
#include "DataAssets/CharacterAnimationDataAsset.h"

UBlendSpace* USPAnimInstance::GetLocomotionBlendspace() const
{
	AStudyProjectCharacter* Character = Cast<AStudyProjectCharacter>(GetOwningActor());
	if(Character && Character->GetCharacterData().CharacterAnimData)
	{
		return Character->GetCharacterData().CharacterAnimData->CharacterAnimationData.MovementBlendspace;
	}
	return DefaultAnimationData ? DefaultAnimationData->CharacterAnimationData.MovementBlendspace : nullptr;
}

UAnimSequenceBase* USPAnimInstance::GetIdleAnimation() const
{
	AStudyProjectCharacter* Character = Cast<AStudyProjectCharacter>(GetOwningActor());
	if(Character && Character->GetCharacterData().CharacterAnimData)
	{
		return Character->GetCharacterData().CharacterAnimData->CharacterAnimationData.IdleAnimationAsset;
	}
	return DefaultAnimationData ? DefaultAnimationData->CharacterAnimationData.IdleAnimationAsset : nullptr;
}
