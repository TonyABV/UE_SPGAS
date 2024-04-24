// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SPTypes.h"
#include "SPFootstepsComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class STUDYPROJECT_API USPFootstepsComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    USPFootstepsComponent();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly)
    FName LeftFootSocketName = TEXT("foot_l");

    UPROPERTY(EditDefaultsOnly)
    FName RightFootSocketName = TEXT("foot_r");

public:
    void HandleFootstep(EFoot Foot);
};
