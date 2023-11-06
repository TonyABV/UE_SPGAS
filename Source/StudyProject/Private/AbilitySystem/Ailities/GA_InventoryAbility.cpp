// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Ailities/GA_InventoryAbility.h"

#include "ActorComponents/InventoryComponent.h"
#include "Actors/WeaponItemActor.h"
#include "Inventory/InventoryItemInstance.h"

void UGA_InventoryAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
    Super::OnGiveAbility(ActorInfo, Spec);
    InventoryComponent = ActorInfo->OwnerActor.Get()->FindComponentByClass<UInventoryComponent>();
}

UInventoryComponent* UGA_InventoryAbility::GetInventoryComponent() const
{
    return InventoryComponent;
}

UInventoryItemInstance* UGA_InventoryAbility::GetEquippedItemInstance() const
{
    return InventoryComponent ? InventoryComponent->GetCurrentEquippedItem() : nullptr;
}

const UItemStaticData* UGA_InventoryAbility::GetEquippedItemStaticData() const
{
    UInventoryItemInstance* ItemInstance = GetEquippedItemInstance();
    return ItemInstance ? ItemInstance->GetItemStaticData() : nullptr;
}

const UWeaponStaticData* UGA_InventoryAbility::GetEquippedWeaponStaticData() const
{
    return Cast<UWeaponStaticData>(GetEquippedItemStaticData());
}

AItemActor* UGA_InventoryAbility::GetEquippedItemActor() const
{
    UInventoryItemInstance* ItemInstance = GetEquippedItemInstance();
    return ItemInstance ? ItemInstance->GetItemActor() : nullptr;
}

AWeaponItemActor* UGA_InventoryAbility::GetEquippedWeaponItemActor() const
{
    return Cast<AWeaponItemActor>(GetEquippedItemActor());
}
