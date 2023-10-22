// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponents/InventoryComponent.h"

#include "Engine/ActorChannel.h"
#include "Inventory/InventoryItemInstance.h"
#include "Net/UnrealNetwork.h"

static TAutoConsoleVariable<int32> CVarShowInventory(TEXT("ShowDebugInventory"),  //
    0,                                                                            //
    TEXT("Draws debug info about Inventory"),                                     //
    ECVF_Cheat);


UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	bWantsInitializeComponent = true;
    SetIsReplicatedByDefault(true);
}

void UInventoryComponent::InitializeComponent()
{
    Super::InitializeComponent();

    if (GetOwner()->HasAuthority())
    {
        for (const auto Item : DefaultItems)
        {
            InventoryList.AddItem(Item);
        }
    }
}

bool UInventoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
    bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (FInventoryListItem& Item : InventoryList.GetItemsRef())
	{
        UInventoryItemInstance* ItemInstance = Item.ItemInstance;

		if (IsValid(ItemInstance))
		{
            WroteSomething |= Channel->ReplicateSubobject(ItemInstance, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, InventoryList);
    DOREPLIFETIME(UInventoryComponent, CurrentEquippedItem);
}

void UInventoryComponent::AddItem(TSubclassOf<UItemStaticData> InItemStaticDataClass)
{
    InventoryList.AddItem(InItemStaticDataClass);
}

void UInventoryComponent::RemoveItem(TSubclassOf<UItemStaticData> InItemStaticDataClass)
{
    InventoryList.RemoveItem(InItemStaticDataClass);
}

void UInventoryComponent::EquipItem(TSubclassOf<UItemStaticData> InItemStaticData)
{
    if (!GetOwner()->HasAuthority()) return;

    for (FInventoryListItem& Item : InventoryList.GetItemsRef())
    {
        if (Item.ItemInstance && Item.ItemInstance->GetItemStaticDataClass() == InItemStaticData)
        {
            Item.ItemInstance->OnEquipped(GetOwner());
            CurrentEquippedItem = Item.ItemInstance;
            break;
        }
    }
}

void UInventoryComponent::UnequipItem()
{
    if (!GetOwner()->HasAuthority()) return;

    if (IsValid(CurrentEquippedItem))
    {
        CurrentEquippedItem->OnUnequipped();
        CurrentEquippedItem = nullptr;
    }
}

void UInventoryComponent::DropItem()
{
    if (!GetOwner()->HasAuthority()) return;

    if (IsValid(CurrentEquippedItem))
    {
        CurrentEquippedItem->OnDropped();
        RemoveItem(CurrentEquippedItem->GetItemStaticDataClass());
        CurrentEquippedItem = nullptr;
    }
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const bool bShowDebug = CVarShowInventory.GetValueOnGameThread() != 0 && IsValid(GEngine);

	if (bShowDebug)
    {
        for (FInventoryListItem& Item : InventoryList.GetItemsRef())
        {
            UInventoryItemInstance* ItemInstance = Item.ItemInstance;
            const UItemStaticData* ItemStatic = ItemInstance->GetItemStaticData();
            if (IsValid(ItemInstance) && IsValid(ItemStatic))
            {
                GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Blue, "Item: "  + ItemStatic->Name.ToString());
            }
        }
	}

}

