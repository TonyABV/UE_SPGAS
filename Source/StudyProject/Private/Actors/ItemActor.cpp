// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/ItemActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AudioDevice.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "ActorComponents/InventoryComponent.h"
#include "Engine/ActorChannel.h"
#include "Inventory/InventoryItemInstance.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"

AItemActor::AItemActor()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;

    SetReplicateMovement(true);

    SphereComponent = CreateDefaultSubobject<USphereComponent>("USphereComponent");
    SphereComponent->SetupAttachment(RootComponent);
    SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AItemActor::OnSphereBeginOverlap);
}

void AItemActor::OnEquipped()
{
    ItemState = EItemState::Equipped;
    SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SphereComponent->SetGenerateOverlapEvents(false);
}

void AItemActor::OnUnequipped()
{
    ItemState = EItemState::None;
    SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SphereComponent->SetGenerateOverlapEvents(false);
}

void AItemActor::OnDropped()
{
    ItemState = EItemState::Dropped;

    GetRootComponent()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

    if (AActor* ItemOwner = GetOwner())
    {
        FVector TraceStart = GetActorLocation() + ItemOwner->GetActorForwardVector() * 100.f;
        FVector TraceEnd = TraceStart - FVector::UpVector * 1000.f;

        FHitResult HitResult;

        FVector TargetLocation = TraceEnd;

        if (UKismetSystemLibrary::LineTraceSingleByProfile(
                GetWorld(), TraceStart, TraceEnd, FName("WorldStatic"), true, {this}, EDrawDebugTrace::ForDuration, HitResult, true))
        {
            if (HitResult.bBlockingHit)
            {
                TargetLocation = HitResult.Location;
            }
        }

        SetActorLocation(TargetLocation);
    }

    SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    SphereComponent->SetGenerateOverlapEvents(true);
}

bool AItemActor::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
    bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
    WroteSomething |= Channel->ReplicateSubobject(ItemInstance, *Bunch, *RepFlags);
    return WroteSomething;
}

void AItemActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AItemActor, ItemInstance);
    DOREPLIFETIME(AItemActor, ItemState);
    DOREPLIFETIME(AItemActor, Quantity);
}

void AItemActor::Init(UInventoryItemInstance* InInstance)
{
    ItemInstance = InInstance;
    InitInternal();
}

void AItemActor::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (HasAuthority())
    {
        FGameplayEventData EventPayload;
        EventPayload.OptionalObject = ItemInstance;
        EventPayload.Instigator = this;
        EventPayload.EventTag = UInventoryComponent::EquipItemTag;

        UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OtherActor, UInventoryComponent::EquipItemTag, EventPayload);
    }
}

void AItemActor::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        if (!IsValid(ItemInstance) && IsValid(StaticDataClass))
        {
            ItemInstance = NewObject<UInventoryItemInstance>();
            ItemInstance->Init(StaticDataClass, Quantity);

            SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
            SphereComponent->SetGenerateOverlapEvents(true);

            InitInternal();
        }
    }
}

void AItemActor::OnRep_ItemInstance(UInventoryItemInstance* OldItemInstance)
{
    if (!IsValid(OldItemInstance) && IsValid(ItemInstance))
    {
        InitInternal();
    }
}

void AItemActor::OnRep_ItemState()
{
    switch (ItemState)
    {
        case EItemState::Equipped:
            SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            SphereComponent->SetGenerateOverlapEvents(false);
            break;
        default:
            SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
            SphereComponent->SetGenerateOverlapEvents(true);
            break;
    }
}

void AItemActor::InitInternal() {}

void AItemActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}
