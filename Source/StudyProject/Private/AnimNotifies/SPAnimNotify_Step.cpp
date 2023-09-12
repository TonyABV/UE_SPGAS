// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/SPAnimNotify_Step.h"
#include "StudyProjectCharacter.h"
#include "ActorComponents/SPFootstepsComponent.h"

void USPAnimNotify_Step::Notify(
    USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	AStudyProjectCharacter* Character = Cast<AStudyProjectCharacter>(MeshComp->GetOwner());
	if (!Character) return;

	if(USPFootstepsComponent* FootstepsComponent = Character->GetFootStepComponent())
	{
		FootstepsComponent->HandleFootstep(Foot);
	}
}
