// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/SPPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "StudyProjectGameMode.h"

void ASPPlayerController::RestartPlayerIn(float InTime)
{
    ChangeState(NAME_Spectating);

    GetWorld()->GetTimerManager().SetTimer(RestartPlayerTimerHandle, this, &ASPPlayerController::RestartPlayer, InTime, false);
}

void ASPPlayerController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    if(UAbilitySystemComponent* ASComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InPawn))
    {
        DeathStateTagDelegate = ASComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(TEXT("State.Dead")), //
            EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ASPPlayerController::OnPawnDeathStateChanged);
    }
}

void ASPPlayerController::OnUnPossess()
{
    if(DeathStateTagDelegate.IsValid())
    {
        if(UAbilitySystemComponent* ASComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()))
        {
            ASComponent->UnregisterGameplayTagEvent(DeathStateTagDelegate, FGameplayTag::RequestGameplayTag(TEXT("State.Dead")), //
                EGameplayTagEventType::NewOrRemoved);
        }
    }
    
    Super::OnUnPossess();
}

void ASPPlayerController::OnPawnDeathStateChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
    if(NewCount > 0)
    {
        UWorld* World = GetWorld();

        AStudyProjectGameMode* SPGameMode = World ? World->GetAuthGameMode<AStudyProjectGameMode>() : nullptr;

        if(SPGameMode)
        {
            SPGameMode->NotifyPlayerDied(this);
        }

        if(DeathStateTagDelegate.IsValid())
        {
            if(UAbilitySystemComponent* ASComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()))
            {
                ASComponent->UnregisterGameplayTagEvent(DeathStateTagDelegate, FGameplayTag::RequestGameplayTag(TEXT("State.Dead")), //
                    EGameplayTagEventType::NewOrRemoved);
            }
        }
    }
}

void ASPPlayerController::RestartPlayer()
{
    UWorld* World = GetWorld();

    AStudyProjectGameMode* SPGameMode = World ? World->GetAuthGameMode<AStudyProjectGameMode>() : nullptr;

    if(SPGameMode)
    {
        SPGameMode->RestartPlayer(this);
    }
}
