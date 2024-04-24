// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "SPTypes.h"
#include "SPAnimNotify_Step.generated.h"

/**
 *
 */
UCLASS()
class STUDYPROJECT_API USPAnimNotify_Step : public UAnimNotify
{
    GENERATED_BODY()

    virtual void Notify(
        USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

public:
    UPROPERTY(EditAnywhere)
    EFoot Foot;
};
