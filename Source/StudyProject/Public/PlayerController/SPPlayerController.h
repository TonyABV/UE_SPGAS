// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "SPPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class STUDYPROJECT_API ASPPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

    void RestartPlayerIn(float InTime);

protected:

    virtual void OnPossess(APawn* InPawn) override;
    virtual void OnUnPossess() override;

    UFUNCTION()
    void OnPawnDeathStateChanged(const FGameplayTag CallbackTag, int32 NewCount);

    void RestartPlayer();

    FTimerHandle RestartPlayerTimerHandle;

    FDelegateHandle DeathStateTagDelegate;
	
};
