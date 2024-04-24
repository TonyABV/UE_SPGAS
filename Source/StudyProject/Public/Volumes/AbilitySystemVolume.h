// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PhysicsVolume.h"
#include "GameplayAbilitySpec.h"
#include "GameplayTagContainer.h"
#include "AbilitySystemVolume.generated.h"

class UGameplayEffect;
class UGameplayAbility;

USTRUCT(BlueprintType)
struct FAbilityVolumeEnteredActorInfo
{
    GENERATED_BODY()

    TArray<FGameplayAbilitySpecHandle> AppliedAbilities;

    TArray<FActiveGameplayEffectHandle> AppliedEffects;
};

UCLASS()
class STUDYPROJECT_API AAbilitySystemVolume : public APhysicsVolume
{
    GENERATED_BODY()

public:
    AAbilitySystemVolume();

    virtual void ActorEnteredVolume(AActor* Other) override;

    virtual void ActorLeavingVolume(AActor* Other) override;

    virtual void Tick(float DeltaSeconds) override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<TSubclassOf<UGameplayEffect>> OnGoingEffectToApply;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<TSubclassOf<UGameplayEffect>> OnExitEffectToApply;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bDrawDebug = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FGameplayTag> GameplayEventsToSendOnEnter;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FGameplayTag> GameplayEventsToSendOnExit;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<TSubclassOf<UGameplayAbility>> OnGoingAbilitiesToGive;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<TSubclassOf<UGameplayAbility>> PermanentAbilitiesToGive;

    TMap<AActor*, FAbilityVolumeEnteredActorInfo> EnteredActorsInfoMap;
};
