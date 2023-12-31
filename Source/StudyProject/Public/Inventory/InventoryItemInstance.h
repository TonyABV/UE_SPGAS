// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SPStatics.h"
#include "SPTypes.h"
#include "GameplayAbilitySpecHandle.h"
#include "ActiveGameplayEffectHandle.h"
#include "InventoryItemInstance.generated.h"

class UItemStaticData;

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class STUDYPROJECT_API UInventoryItemInstance: public UObject
{

	GENERATED_BODY()

public:

    virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

    virtual void Init(TSubclassOf<UItemStaticData> InItemStaticDataClass);

    TSubclassOf<UItemStaticData> GetItemStaticDataClass() const { return ItemStaticDataClass; }

    UFUNCTION(BlueprintCallable, BlueprintPure)
    const UItemStaticData* GetItemStaticData() const;

	virtual void OnEquipped(AActor* InOwner);
    virtual void OnUnequipped(AActor* InOwner);
    virtual void OnDropped(AActor* InOwner);

    AItemActor* GetItemActor() const { return  ItemActor; }
    
private:

	virtual bool IsSupportedForNetworking() const override { return true; }

	UPROPERTY(Replicated)
    TSubclassOf<UItemStaticData> ItemStaticDataClass;
	
	UPROPERTY(ReplicatedUsing = OnRep_Equipped)
    bool bEquipped = false;

	UFUNCTION()
    void OnRep_Equipped();

    UPROPERTY(Replicated)
    AItemActor* ItemActor = nullptr;

    void TryGrantAbilities(AActor* InOwner);

    void TryRemoveAbilities(AActor* InOwner);

    void TryApplyEffect(AActor* InOwner);

    void TryRemoveEffect(AActor* InOwner);

    UPROPERTY()
    TArray<FGameplayAbilitySpecHandle> GrantedAbilityHandles;

    TArray<FActiveGameplayEffectHandle> OngoingEffectHandles;
    
};
