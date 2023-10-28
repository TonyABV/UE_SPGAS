// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryItemInstance.h"

#include "Actors/ItemActor.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

void UInventoryItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UInventoryItemInstance, ItemStaticDataClass);
    DOREPLIFETIME(UInventoryItemInstance, bEquipped);
    DOREPLIFETIME(UInventoryItemInstance, ItemActor);
}

void UInventoryItemInstance::Init(TSubclassOf<UItemStaticData> InItemStaticDataClass)
{
    ItemStaticDataClass = InItemStaticDataClass;
}

const UItemStaticData* UInventoryItemInstance::GetItemStaticData() const
{
    return USPStatics::GetItemStaticData(ItemStaticDataClass);
}

void UInventoryItemInstance::OnEquipped(AActor* InOwner)
{
    UWorld* World = InOwner->GetWorld();
    if (IsValid(World))
    {
        if (!GetItemStaticData()) return;

        FTransform SpawnTransform;

        ItemActor = World->SpawnActorDeferred<AItemActor>(GetItemStaticData()->ItemActorClass, SpawnTransform, InOwner);

        ItemActor->Init(this);
        ItemActor->OnEquipped();   
        ItemActor->FinishSpawning(SpawnTransform);

        ACharacter* Character = Cast<ACharacter>(InOwner);
        if (Character)
        {
            ItemActor->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, GetItemStaticData()->AttachmentSocket);
        }

    }
    bEquipped = true;
}

void UInventoryItemInstance::OnUnequipped()
{
    if (ItemActor)
    {
        ItemActor->Destroy();
        ItemActor = nullptr;
    }
    bEquipped = false;
}

void UInventoryItemInstance::OnDropped()
{
    if (ItemActor)
    {
        ItemActor->OnDropped();
    }
    bEquipped = false;
}

void UInventoryItemInstance::OnRep_Equipped()
{
}
