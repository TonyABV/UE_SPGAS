// Fill out your copyright notice in the Description page of Project Settings.

#include "Inventory/InventoryItemInstance.h"

#include "Actors/ItemActor.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemLog.h"
#include "AbilitySystemLog.h"
#include "Kismet/KismetMathLibrary.h"

void UInventoryItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UInventoryItemInstance, ItemStaticDataClass);
    DOREPLIFETIME(UInventoryItemInstance, bEquipped);
    DOREPLIFETIME(UInventoryItemInstance, ItemActor);
    DOREPLIFETIME(UInventoryItemInstance, Quantity);
}

void UInventoryItemInstance::Init(TSubclassOf<UItemStaticData> InItemStaticDataClass, int32 InQuantity)
{
    ItemStaticDataClass = InItemStaticDataClass;
    Quantity = InQuantity;
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
            ItemActor->AttachToComponent(
                Character->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, GetItemStaticData()->AttachmentSocket);
        }
    }

    TryGrantAbilities(InOwner);

    TryApplyEffect(InOwner);

    bEquipped = true;
}

void UInventoryItemInstance::OnUnequipped(AActor* InOwner)
{
    if (ItemActor)
    {
        ItemActor->Destroy();
        ItemActor = nullptr;
    }

    TryRemoveAbilities(InOwner);

    TryRemoveEffect(InOwner);

    bEquipped = false;
}

void UInventoryItemInstance::OnDropped(AActor* InOwner)
{
    if (ItemActor)
    {
        ItemActor->OnDropped();
    }

    TryRemoveAbilities(InOwner);

    TryRemoveEffect(InOwner);

    bEquipped = false;
}

void UInventoryItemInstance::AddItems(int32 Count)
{
    Quantity += Count;

    if (Quantity < 0)
    {
        Quantity = 0;
    }
}

void UInventoryItemInstance::OnRep_Equipped() {}

void UInventoryItemInstance::TryGrantAbilities(AActor* InOwner)
{
    if (InOwner && InOwner->HasAuthority())
    {
        if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InOwner))
        {
            if (const UItemStaticData* ItemStaticData = GetItemStaticData())
            {
                for (auto Ability : ItemStaticData->GrantedAbilities)
                {
                    GrantedAbilityHandles.Add(AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Ability)));
                }
            }
        }
    }
}

void UInventoryItemInstance::TryRemoveAbilities(AActor* InOwner)
{
    if (InOwner && InOwner->HasAuthority())
    {
        if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InOwner))
        {
            for (auto Handle : GrantedAbilityHandles)
            {
                AbilitySystemComponent->ClearAbility(Handle);
            }
            GrantedAbilityHandles.Empty();
        }
    }
}

void UInventoryItemInstance::TryApplyEffect(AActor* InOwner)
{
    if (UAbilitySystemComponent* ASComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InOwner))
    {
        const UItemStaticData* ItemStaticData = GetItemStaticData();

        FGameplayEffectContextHandle ContextHandle = ASComponent->MakeEffectContext();
        for (auto GameplayEffect : ItemStaticData->OngoingEffects)
        {
            if (!GameplayEffect.Get()) continue;

            FGameplayEffectSpecHandle SpecHandle = ASComponent->MakeOutgoingSpec(GameplayEffect, 1, ContextHandle);
            if (SpecHandle.IsValid())
            {
                FActiveGameplayEffectHandle ActiveGEHandle = ASComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
                if (!ActiveGEHandle.WasSuccessfullyApplied())
                {
                    ABILITY_LOG(Log, TEXT("Item %s failed to apply runtime effect %s"), *GetName(), *GetNameSafe(GameplayEffect));
                }
                else
                {
                    OngoingEffectHandles.Add(ActiveGEHandle);
                }
            }
        }
    }
}

void UInventoryItemInstance::TryRemoveEffect(AActor* InOwner)
{
    if (UAbilitySystemComponent* ASComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InOwner))
    {
        for (auto EffectHandle : OngoingEffectHandles)
        {
            if (EffectHandle.IsValid())
            {
                ASComponent->RemoveActiveGameplayEffect(EffectHandle);
            }
        }
    }
    OngoingEffectHandles.Empty();
}
