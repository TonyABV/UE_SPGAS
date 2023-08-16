// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstances/SPGameInstance.h"
#include "AbilitySystemGlobals.h"

void USPGameInstance::Init()
{
	Super::Init();

	UAbilitySystemGlobals::Get().InitGlobalData();
}
