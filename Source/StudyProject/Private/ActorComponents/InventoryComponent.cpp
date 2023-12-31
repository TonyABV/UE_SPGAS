// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponents/InventoryComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagsManager.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Engine/ActorChannel.h"
#include "Inventory/InventoryItemInstance.h"
#include "Net/UnrealNetwork.h"

static TAutoConsoleVariable<int32> CVarShowInventory(TEXT("ShowDebugInventory"),  //
    0,                                                                            //
    TEXT("Draws debug info about Inventory"),                                     //
    ECVF_Cheat);

FGameplayTag UInventoryComponent::EquipItemTag;
FGameplayTag UInventoryComponent::DropItemTag;
FGameplayTag UInventoryComponent::EquipNextItemTag;
FGameplayTag UInventoryComponent::UnequipItemTag;

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	bWantsInitializeComponent = true;
    SetIsReplicatedByDefault(true);

    static bool bHandledAddingTags = false;
    if(!bHandledAddingTags)
    {
        UGameplayTagsManager::Get().OnLastChanceToAddNativeTags().AddUObject(this, &UInventoryComponent::AddInventoryTags);
        bHandledAddingTags = true;
    }    
}

void UInventoryComponent::AddInventoryTags()
{
    UGameplayTagsManager& TagsManager = UGameplayTagsManager::Get();

     UInventoryComponent::EquipItemTag = TagsManager. //
     AddNativeGameplayTag(TEXT("Event.Inventory.EquipItemActor"), TEXT("Equip item from item actor event"));
     UInventoryComponent::DropItemTag = TagsManager. //
     AddNativeGameplayTag(TEXT("Event.Inventory.DropItem"), TEXT("Drop equipped item"));
     UInventoryComponent::EquipItemTag = TagsManager. //
     AddNativeGameplayTag(TEXT("Event.Inventory.EquipNext"), TEXT("Try equip next item"));
     UInventoryComponent::UnequipItemTag = TagsManager. //
     AddNativeGameplayTag(TEXT("Event.Inventory.EquipItemActor"), TEXT("Unequip current item"));
    
     TagsManager.OnLastChanceToAddNativeTags().RemoveAll(this);
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

    if(UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
    {
        ASC->GenericGameplayEventCallbacks.FindOrAdd(UInventoryComponent::EquipItemTag).AddUObject(this, &UInventoryComponent::GameplayEvenCallback);
        ASC->GenericGameplayEventCallbacks.FindOrAdd(UInventoryComponent::DropItemTag).AddUObject(this, &UInventoryComponent::GameplayEvenCallback);
        ASC->GenericGameplayEventCallbacks.FindOrAdd(UInventoryComponent::UnequipItemTag).AddUObject(this, &UInventoryComponent::GameplayEvenCallback);
        ASC->GenericGameplayEventCallbacks.FindOrAdd(UInventoryComponent::EquipNextItemTag).AddUObject(this, &UInventoryComponent::GameplayEvenCallback);
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
    if (!GetOwner()->HasAuthority()) return;

    InventoryList.AddItem(InItemStaticDataClass);
}

void UInventoryComponent::AddItemInstance(UInventoryItemInstance* InItemInstance)
{
    if(GetOwner()->HasAuthority())
    {
        InventoryList.AddItem(InItemInstance);
    }
}

void UInventoryComponent::RemoveItem(TSubclassOf<UItemStaticData> InItemStaticDataClass)
{
    if (!GetOwner()->HasAuthority()) return;

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

void UInventoryComponent::EquipItemInstance(UInventoryItemInstance* InItemInstance)
{
    if (!GetOwner()->HasAuthority()) return;

    for (FInventoryListItem& Item : InventoryList.GetItemsRef())
    {
        if (Item.ItemInstance == InItemInstance)
        {
            Item.ItemInstance->OnEquipped(GetOwner());
            CurrentEquippedItem = Item.ItemInstance;
            break;
        }
    }
}

void UInventoryComponent::EquipNext()
{
    if (!GetOwner()->HasAuthority()) return;

    TArray<FInventoryListItem>& Items = InventoryList.GetItemsRef();

    const bool bNoItems = Items.Num() == 0;
    const bool bOneAndEquipped =  Items.Num() == 1 && CurrentEquippedItem;

    if(bNoItems || bOneAndEquipped) return;

    UInventoryItemInstance* TargetItem = CurrentEquippedItem;
    
    for(auto Item : Items)
    {
        if(Item.ItemInstance->GetItemStaticData()->bCanBeEquipped)
        {
            if(Item.ItemInstance != CurrentEquippedItem)
            {
                TargetItem = Item.ItemInstance;
                break;
            }
        }
    }

    if(CurrentEquippedItem)
    {
        if(TargetItem == CurrentEquippedItem)
        {
            return;
        }
        UnequipItem();
    }
    
    EquipItemInstance(TargetItem);
}

void UInventoryComponent::UnequipItem()
{
    if (!GetOwner()->HasAuthority()) return;

    if (IsValid(CurrentEquippedItem))
    {
        CurrentEquippedItem->OnUnequipped(GetOwner());
        CurrentEquippedItem = nullptr;
    }
}

void UInventoryComponent::DropItem()
{
    if (!GetOwner()->HasAuthority()) return;

    if (IsValid(CurrentEquippedItem))
    {
        CurrentEquippedItem->OnDropped(GetOwner());
        RemoveItem(CurrentEquippedItem->GetItemStaticDataClass());
        CurrentEquippedItem = nullptr;
    }
}

void UInventoryComponent::GameplayEvenCallback(const FGameplayEventData* Payload)
{
    ENetRole NetRole = GetOwnerRole();

    if(NetRole == ROLE_Authority)
    {
        HandelGameplayEventInternal(*Payload);
    }
    else if(NetRole == ROLE_AutonomousProxy)
    {
        ServerHandleGameplayEvent(*Payload);
    }
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInventoryComponent::HandelGameplayEventInternal(FGameplayEventData Payload)
{
    ENetRole NetRole = GetOwnerRole();

    if(NetRole == ROLE_Authority)
    {
        FGameplayTag EventTag = Payload.EventTag;

        if(EventTag == UInventoryComponent::EquipItemTag)
        {
            if(const UInventoryItemInstance* ItemInstance = Cast<UInventoryItemInstance>(Payload.OptionalObject))
            {
                AddItemInstance(const_cast<UInventoryItemInstance*>(ItemInstance));

                if(Payload.Instigator && Payload.Instigator.Get())
                {
                    const_cast<AActor*>(Payload.Instigator.Get())->Destroy();
                }
            }
        }
        else if(EventTag == UInventoryComponent::EquipNextItemTag)
        {
            EquipNext();
        }
        else if(EventTag == UInventoryComponent::DropItemTag)
        {
            DropItem();
        }
        else if(EventTag == UInventoryComponent::UnequipItemTag)
        {
            UnequipItem();
        }
    }
}

void UInventoryComponent::ServerHandleGameplayEvent_Implementation(FGameplayEventData Payload)
{
    HandelGameplayEventInternal(Payload);
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

