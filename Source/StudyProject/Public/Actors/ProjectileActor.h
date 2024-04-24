// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SPTypes.h"
#include "ProjectileActor.generated.h"

UCLASS()
class STUDYPROJECT_API AProjectileActor : public AActor
{
    GENERATED_BODY()

public:
    AProjectileActor();

    const UProjectileStaticData* GetProjectileStaticData() const;

    UPROPERTY(BlueprintReadOnly, Replicated)
    TSubclassOf<UProjectileStaticData> ProjectileDataClass;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

    UPROPERTY()
    class UProjectileMovementComponent* ProjectileMovementComponent = nullptr;

    void DebugDrawPath() const;

    UPROPERTY()
    class UStaticMeshComponent* StaticMeshComponent = nullptr;

    UFUNCTION()
    void OnProjectileStop(const FHitResult& ImpactResult);
};
