// Fill out your copyright notice in the Description page of Project Settings.

#include "ActorComponents/SPFootstepsComponent.h"

#include "SPTypes.h"
#include "GameFramework/Character.h"
#include "PhysicalMaterials/SPPhysicalMaterial.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

static TAutoConsoleVariable<int32> CVarShowFootsteps(
    TEXT("ShowDebugFootsteps"), 0, TEXT("Draws debug info about footsteps/n  0: off/n  1: on/n"), ECVF_Cheat);

USPFootstepsComponent::USPFootstepsComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void USPFootstepsComponent::BeginPlay()
{
    Super::BeginPlay();
}

void USPFootstepsComponent::HandleFootstep(EFoot Foot)
{
    ACharacter* Character = GetOwner<ACharacter>();
    if (Character)
    {
        const int8 DebugShowFootsteps = CVarShowFootsteps.GetValueOnAnyThread();

        if (USkeletalMeshComponent* SkeletalMesh = Character->GetMesh())
        {
            FHitResult HitResult;
            const FVector StartTracePoint =  //
                SkeletalMesh->GetSocketLocation(Foot == EFoot::Left ? LeftFootSocketName : RightFootSocketName);
            const FVector EndTracePoint = StartTracePoint + FVector::UpVector * -50.f;

            FCollisionQueryParams QueryParams;
            QueryParams.AddIgnoredActor(Character);
            QueryParams.bReturnPhysicalMaterial = true;

            GetWorld()->LineTraceSingleByChannel(
                HitResult, StartTracePoint, EndTracePoint, ECollisionChannel::ECC_WorldStatic, QueryParams);

            if (HitResult.bBlockingHit)
            {
                if (HitResult.PhysMaterial.Get())
                {
                    USPPhysicalMaterial* PhysicalMaterial = Cast<USPPhysicalMaterial>(HitResult.PhysMaterial.Get());
                    if (PhysicalMaterial)
                    {
                        UGameplayStatics::SpawnSoundAtLocation(GetWorld(), PhysicalMaterial->FootStepSound, StartTracePoint);
                    }
                    if (DebugShowFootsteps == 1)
                    {
                        DrawDebugString(GetWorld(), HitResult.ImpactPoint, GetNameSafe(PhysicalMaterial), nullptr, FColor::White, 4.);
                    }
                }
                if (DebugShowFootsteps == 1)
                {
                    DrawDebugLine(GetWorld(), StartTracePoint, EndTracePoint, FColor::Red, false, 4.f);
                }
            }
            else
            {
                if (DebugShowFootsteps == 1)
                {
                    DrawDebugLine(GetWorld(), StartTracePoint, EndTracePoint, FColor::Green, false, 4.f);
                }
            }
        }
    }
}
