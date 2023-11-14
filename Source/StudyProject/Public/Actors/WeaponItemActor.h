// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/ItemActor.h"
#include "SPTypes.h"
#include "WeaponItemActor.generated.h"

/**
 * 
 */
UCLASS()
class STUDYPROJECT_API AWeaponItemActor : public AItemActor
{
	GENERATED_BODY()

    AWeaponItemActor();

    const UWeaponStaticData* GetWeaponStaticData() const;

    UFUNCTION(BlueprintCallable)
    void PlayWeaponEffects(const FHitResult& InHitResult);

public:

    UFUNCTION(BlueprintPure)
    FVector GetMuzzleLocation() const;
    
protected:

    UFUNCTION(NetMulticast, Reliable)
    void MulticastPlayWeaponEffect(const FHitResult& InHitResult);

    void PlayWeaponEffectInternal(const FHitResult& InHitResult);

    UPROPERTY()
    UMeshComponent* MeshComponent = nullptr;
    
    virtual void InitInternal() override;

};
