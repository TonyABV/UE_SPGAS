// Fill out your copyright notice in the Description page of Project Settings.

#include "Inventory/InventoryList.h"

#include "GameplayTagContainer.h"
#include "Inventory/InventoryItemInstance.h"

void FInventoryList::AddItem(TSubclassOf<UItemStaticData> InItemStaticDataClass)
{
    FInventoryListItem& Item = Items.AddDefaulted_GetRef();
    Item.ItemInstance = NewObject<UInventoryItemInstance>();
    Item.ItemInstance->Init(InItemStaticDataClass, USPStatics::GetItemStaticData(InItemStaticDataClass)->MaxStackCount);
    MarkItemDirty(Item);
}

void FInventoryList::AddItem(UInventoryItemInstance* InventoryItemInstance)
{
    FInventoryListItem& Item = Items.AddDefaulted_GetRef();
    Item.ItemInstance = InventoryItemInstance;
    MarkItemDirty(Item);
}

void FInventoryList::RemoveItem(TSubclassOf<UItemStaticData> InItemStaticDataClass)
{
    for (auto ItemIter = Items.CreateIterator(); ItemIter; ++ItemIter)
    {
        FInventoryListItem& Item = *ItemIter;
        if (Item.ItemInstance && Item.ItemInstance->GetItemStaticData()->IsA(InItemStaticDataClass))
        {
            ItemIter.RemoveCurrent();
            MarkArrayDirty();
            break;
        }
    }
}

void FInventoryList::RemoveItem(UInventoryItemInstance* InItemInstance)
{
    for (auto ItemIter = Items.CreateIterator(); ItemIter; ++ItemIter)
    {
        FInventoryListItem& Item = *ItemIter;
        if (Item.ItemInstance && Item.ItemInstance == InItemInstance)
        {
            ItemIter.RemoveCurrent();
            MarkArrayDirty();
            break;
        }
    }
}

TArray<UInventoryItemInstance*> FInventoryList::GetAllItemInstancesWithTag(FGameplayTag InTag)
{
    TArray<UInventoryItemInstance*> OutInstances;

    for (auto ItemIter = Items.CreateIterator(); ItemIter; ++ItemIter)
    {
        FInventoryListItem& Item = *ItemIter;
        if (Item.ItemInstance->GetItemStaticData()->InventoryTags.Contains(InTag))
        {
            OutInstances.Add(Item.ItemInstance);
        }
    }

    return OutInstances;
}

TArray<UInventoryItemInstance*> FInventoryList::GetAllAvailableInstancesOfType(TSubclassOf<UItemStaticData> InItemStaticDataClass)
{
    TArray<UInventoryItemInstance*> OutInstances;

    for (auto ItemIter = Items.CreateIterator(); ItemIter; ++ItemIter)
    {
        FInventoryListItem& Item = *ItemIter;

        const UItemStaticData* ItemStaticData = Item.ItemInstance->GetItemStaticData();

        const bool bSameType = ItemStaticData->IsA(InItemStaticDataClass);
        const bool bHasCapacity = ItemStaticData->MaxStackCount > Item.ItemInstance->GetQuantity();

        if (bSameType && bHasCapacity)
        {
            OutInstances.Add(Item.ItemInstance);
        }
    }

    return OutInstances;
}
