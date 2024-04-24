// Copyright Epic Games, Inc. All Rights Reserved.

#include "StudyProjectGameMode.h"
#include "StudyProjectCharacter.h"
#include "PlayerController/SPPlayerController.h"
#include "UObject/ConstructorHelpers.h"

AStudyProjectGameMode::AStudyProjectGameMode()
{
    // set default pawn class to our Blueprinted character
    static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
    if (PlayerPawnBPClass.Class != NULL)
    {
        DefaultPawnClass = PlayerPawnBPClass.Class;
    }
    PlayerControllerClass = ASPPlayerController::StaticClass();
}

void AStudyProjectGameMode::NotifyPlayerDied(ASPPlayerController* PlayerController)
{
    if (PlayerController) PlayerController->RestartPlayerIn(2.f);
}
