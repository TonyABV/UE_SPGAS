// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/InventoryList.h"
#include "InventoryComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STUDYPROJECT_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UInventoryComponent();

	virtual void InitializeComponent() override;

	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
    void AddItem(TSubclassOf<UItemStaticData> InItemStaticDataClass);
    UFUNCTION(BlueprintCallable)
    void RemoveItem(TSubclassOf<UItemStaticData> InItemStaticDataClass);

    UFUNCTION(BlueprintCallable)
    void EquipItem(TSubclassOf<UItemStaticData> InItemStaticData);

    UFUNCTION(BlueprintCallable)
    void UnequipItem();

    UFUNCTION(BlueprintCallable)
    void DropItem();

    UFUNCTION(BlueprintCallable)
    UInventoryItemInstance* GetCurrentEquippedItem() const { return CurrentEquippedItem; }

protected:

	virtual void BeginPlay() override;

	UPROPERTY(Replicated)
    FInventoryList InventoryList;

	UPROPERTY(EditDefaultsOnly)
    TArray<TSubclassOf<UItemStaticData>> DefaultItems;

    UPROPERTY(Replicated)
    UInventoryItemInstance* CurrentEquippedItem = nullptr;

public:	

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};
