// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "StudyProjectGameMode.generated.h"

class ASPPlayerController;

UCLASS(minimalapi)
class AStudyProjectGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AStudyProjectGameMode();

    void NotifyPlayerDied(ASPPlayerController* PlayerController);
};
