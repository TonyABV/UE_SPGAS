// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "SPTypes.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "ItemActor.generated.h"

class UInventoryItemInstance;
class USphereComponent;
class UItemStaticData;

UCLASS()
class STUDYPROJECT_API AItemActor : public AActor
{
    GENERATED_BODY()

public:
    AItemActor();

    virtual void OnEquipped();
    virtual void OnUnequipped();
    virtual void OnDropped();

    virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    void Init(UInventoryItemInstance* InInstance);

    UFUNCTION()
    void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USphereComponent* SphereComponent;

    UPROPERTY(ReplicatedUsing = OnRep_ItemInstance)
    UInventoryItemInstance* ItemInstance = nullptr;

    UFUNCTION()
    void OnRep_ItemInstance(UInventoryItemInstance* OldItemInstance);

    UPROPERTY(ReplicatedUsing = OnRep_ItemState)
    TEnumAsByte<EItemState> ItemState = EItemState::None;

    UPROPERTY(EditAnywhere)
    TSubclassOf<UItemStaticData> StaticDataClass;

    UFUNCTION()
    void OnRep_ItemState();

    virtual void InitInternal();

    UPROPERTY(EditAnywhere, Replicated)
    int32 Quantity = 1;

public:
    virtual void Tick(float DeltaTime) override;
};
