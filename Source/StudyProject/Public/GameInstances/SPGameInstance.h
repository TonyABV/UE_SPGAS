// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SPGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class STUDYPROJECT_API USPGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	virtual void Init() override;
};
