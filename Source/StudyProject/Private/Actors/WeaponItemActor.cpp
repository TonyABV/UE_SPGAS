// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/WeaponItemActor.h"

#include "Inventory/InventoryItemInstance.h"
#include "SPTypes.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicalMaterials/SPPhysicalMaterial.h"
#include "NiagaraFunctionLibrary.h"

AWeaponItemActor::AWeaponItemActor()
{
}

const UWeaponStaticData* AWeaponItemActor::GetWeaponStaticData() const
{
    return IsValid(ItemInstance) ? Cast<UWeaponStaticData>(ItemInstance->GetItemStaticData()) : nullptr;
}

FVector AWeaponItemActor::GetMuzzleLocation() const
{
    return MeshComponent ? MeshComponent->GetSocketLocation("sMuzzle") : GetActorLocation();
}

void AWeaponItemActor::PlayWeaponEffects(const FHitResult& InHitResult)
{
    if(HasAuthority())
    {
        MulticastPlayWeaponEffect(InHitResult);
    }
    else
    {
        PlayWeaponEffectInternal(InHitResult);
    }
}

void AWeaponItemActor::PlayWeaponEffectInternal(const FHitResult& InHitResult)
{
    if(InHitResult.PhysMaterial.Get())
    {
        if(USPPhysicalMaterial* PhysicalMaterial = Cast<USPPhysicalMaterial>(InHitResult.PhysMaterial.Get()))
        {
            UGameplayStatics::PlaySoundAtLocation(this, PhysicalMaterial->PointImpactSound, InHitResult.ImpactPoint);
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, PhysicalMaterial->PointImpactVFX, InHitResult.ImpactPoint);
        }
    }
    if(const UWeaponStaticData* WeaponStaticData = GetWeaponStaticData())
    {
        UGameplayStatics::PlaySoundAtLocation(this, WeaponStaticData->AttackSound, GetActorLocation());
    }
}

void AWeaponItemActor::MulticastPlayWeaponEffect_Implementation(const FHitResult& InHitResult)
{
    if(!Owner || Owner->GetLocalRole() != ROLE_AutonomousProxy)
    {
        PlayWeaponEffectInternal(InHitResult);
    }
}

void AWeaponItemActor::InitInternal()
{
    Super::InitInternal();

    if(const UWeaponStaticData* WeaponData = GetWeaponStaticData())
    {
        if(WeaponData->SkeletalMesh)
        {
            USkeletalMeshComponent* SkeletalMeshComponent = NewObject<USkeletalMeshComponent>(this, //
                USkeletalMeshComponent::StaticClass(),//
                "SkeletalMeshComponent");
            if(SkeletalMeshComponent)
            {
                SkeletalMeshComponent->RegisterComponent();
                SkeletalMeshComponent->SetSkeletalMesh(WeaponData->SkeletalMesh);
                SkeletalMeshComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);

                MeshComponent = SkeletalMeshComponent;
            }
        }
        else if(WeaponData->StaticMesh)
        {
            UStaticMeshComponent* StaticMeshComponent = NewObject<UStaticMeshComponent>(this, //
                UStaticMeshComponent::StaticClass(),//
                "SkeletalMeshComponent");
            if(StaticMeshComponent)
            {
                StaticMeshComponent->RegisterComponent();
                StaticMeshComponent->SetStaticMesh(WeaponData->StaticMesh);
                StaticMeshComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);

                MeshComponent = StaticMeshComponent;
            }
        }

        if(IsValid(MeshComponent))
        {
            MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        }
    }
}
