// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AT_WallRunTick.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWallRunWallSideDeterminedDelegate, bool, bLeftSide);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWallRunFinishedDelegate);

class ACharacter;
class UCharacterMovementComponent;

UCLASS()
class STUDYPROJECT_API UAT_WallRunTick : public UAbilityTask
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable)
	FOnWallRunWallSideDeterminedDelegate OnWallSideDetermined;

	UPROPERTY(BlueprintAssignable)
    FOnWallRunFinishedDelegate OnWallRunFinished;

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidenPin = "OwneingAbility", DefaultToSelf = "OwningAbility"))
    static UAT_WallRunTick* CreateWallRunTask(UGameplayAbility* OwningAbility, ACharacter* InCharacterOwner,
        UCharacterMovementComponent* InCharacterMovement, TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes);

	virtual void Activate() override;

	virtual void OnDestroy(bool bInOwnerFinished) override;

	virtual void TickTask(float DeltaTime) override;

protected:

	UCharacterMovementComponent* CharacterMovement = nullptr;

	ACharacter* CharacterOwner = nullptr;

	TArray<TEnumAsByte<EObjectTypeQuery>> WallRun_TraceObjectTypes;

	bool FindRunnableWall(FHitResult& OnWallHit);

	bool IsWallOnTheLeft(const FHitResult& InWallHit) const;

};
