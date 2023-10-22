// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/ItemActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Engine/ActorChannel.h"
#include "Inventory/InventoryItemInstance.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"

AItemActor::AItemActor()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;

    SphereComponent = CreateDefaultSubobject<USphereComponent>("USphereComponent");
    SphereComponent->SetupAttachment(RootComponent);
    SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AItemActor::OnSphereBeginOverlap);
}

void AItemActor::OnEquipped()
{
    ItemState = EItemState::Equipped;
    SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AItemActor::OnUnequipped()
{
    ItemState = EItemState::None;
    SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AItemActor::OnDropped()
{
    ItemState = EItemState::Dropped;

    SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

    GetRootComponent()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

    FVector StartPoint = GetActorLocation() + GetActorForwardVector() * 100.f;
    FVector TraceEnd = StartPoint - FVector::UpVector * 1000.f;

    FHitResult HitResult;
    if (UKismetSystemLibrary::LineTraceSingleByProfile(
            GetWorld(), StartPoint, TraceEnd, FName("WorldStatic"), true, {this}, EDrawDebugTrace::ForDuration, HitResult, true))
    {
        if (HitResult.bBlockingHit)
        {
            SetActorLocation(HitResult.Location);
            return;
        }
    }
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
}

void AItemActor::Init(UInventoryItemInstance* InInstance)
{
}

void AItemActor::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    FGameplayEventData GameplayEvent;
    GameplayEvent.OptionalObject = this;

    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OtherActor, OverlappedEventTag, GameplayEvent);
}

// Called when the game starts or when spawned
void AItemActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AItemActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

