// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FastArrayTagCounter.h"
#include "Components/ActorComponent.h"
#include "Inventory/InventoryList.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "InventoryComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class STUDYPROJECT_API UInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UInventoryComponent();

    void AddInventoryTags();

    virtual void InitializeComponent() override;

    virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION(BlueprintCallable)
    void AddItem(TSubclassOf<UItemStaticData> InItemStaticDataClass);
    UFUNCTION(BlueprintCallable)
    void AddItemInstance(UInventoryItemInstance* InItemInstance);
    UFUNCTION(BlueprintCallable)
    void RemoveItem(TSubclassOf<UItemStaticData> InItemStaticDataClass);

    UFUNCTION(BlueprintCallable)
    void RemoveItemInstance(UInventoryItemInstance* InItemInstance);

    UFUNCTION(BlueprintCallable)
    void RemoveItemInstanceWithInventoryTag(FGameplayTag InventoryTag, int32 Count = 1);

    UFUNCTION(BlueprintCallable)
    void EquipItem(TSubclassOf<UItemStaticData> InItemStaticData);
    UFUNCTION(BlueprintCallable)
    void EquipItemInstance(UInventoryItemInstance* InItemInstance);

    UFUNCTION(BlueprintCallable)
    void EquipNext();

    UFUNCTION(BlueprintCallable)
    void UnequipItem();

    UFUNCTION(BlueprintCallable)
    void DropItem();

    UFUNCTION(BlueprintCallable)
    UInventoryItemInstance* GetCurrentEquippedItem() const { return CurrentEquippedItem; }

    virtual void GameplayEvenCallback(const FGameplayEventData* Payload);

    static FGameplayTag EquipItemTag;
    static FGameplayTag DropItemTag;
    static FGameplayTag EquipNextItemTag;
    static FGameplayTag UnequipItemTag;

    UFUNCTION(BlueprintCallable)
    int32 GetInventoryTagCount(FGameplayTag Tag) const;

    UFUNCTION(BlueprintCallable)
    void AddInventoryTagCount(FGameplayTag InTag, int32 CountDelta);

protected:
    virtual void BeginPlay() override;

    UPROPERTY(Replicated)
    FInventoryList InventoryList;

    UPROPERTY(EditDefaultsOnly)
    TArray<TSubclassOf<UItemStaticData>> DefaultItems;

    UPROPERTY(Replicated)
    UInventoryItemInstance* CurrentEquippedItem = nullptr;

    UPROPERTY(Replicated)
    FFastArrayTagCounter InventoryTags;

    TArray<UInventoryItemInstance*> GetAllInstancesWithTag(FGameplayTag Tag);

    FDelegateHandle TagDelegateHandle;

    void HandelGameplayEventInternal(FGameplayEventData Payload);

    UFUNCTION(Server, Reliable)
    void ServerHandleGameplayEvent(FGameplayEventData Payload);

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
