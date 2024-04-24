// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/ProjectileActor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "SPStatics.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

static TAutoConsoleVariable<int32> CVarShowProjectile(TEXT("ShowDebugProjectile"),  //
    0,                                                                              //
    TEXT("Draws debug info about projectiles"),                                     //
    ECVF_Cheat);

AProjectileActor::AProjectileActor()
{
    PrimaryActorTick.bCanEverTick = false;
    SetReplicatingMovement(true);
    bReplicates = true;

    ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");

    ProjectileMovementComponent->ProjectileGravityScale = 0.f;
    ProjectileMovementComponent->Velocity = FVector::ZeroVector;
    ProjectileMovementComponent->OnProjectileStop.AddDynamic(this, &AProjectileActor::OnProjectileStop);

    StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("MeshComponent");

    StaticMeshComponent->SetupAttachment(GetRootComponent());
    StaticMeshComponent->SetIsReplicated(true);
    StaticMeshComponent->SetCollisionProfileName("Projectile");
    StaticMeshComponent->bReceivesDecals = false;
}

const UProjectileStaticData* AProjectileActor::GetProjectileStaticData() const
{
    if (IsValid(ProjectileDataClass))
    {
        return GetDefault<UProjectileStaticData>(ProjectileDataClass);
    }
    return nullptr;
}

void AProjectileActor::BeginPlay()
{
    Super::BeginPlay();

    const UProjectileStaticData* ProjectileStaticData = GetProjectileStaticData();

    if (ProjectileStaticData && ProjectileMovementComponent)
    {
        if (ProjectileStaticData->StaticMesh)
        {
            StaticMeshComponent->SetStaticMesh(ProjectileStaticData->StaticMesh);
        }

        ProjectileMovementComponent->bInitialVelocityInLocalSpace = false;
        ProjectileMovementComponent->InitialSpeed = ProjectileStaticData->InitialSpeed;
        ProjectileMovementComponent->MaxSpeed = ProjectileStaticData->MaxSpeed;
        ProjectileMovementComponent->bRotationFollowsVelocity = true;
        ProjectileMovementComponent->bShouldBounce = false;
        ProjectileMovementComponent->Bounciness = 0.f;
        ProjectileMovementComponent->ProjectileGravityScale = ProjectileStaticData->GravityMultiplayer;
        ProjectileMovementComponent->Velocity = ProjectileStaticData->InitialSpeed * GetActorForwardVector();
    }

    const int32 DebugShowProjectile = CVarShowProjectile.GetValueOnAnyThread();

    if (DebugShowProjectile)
    {
        DebugDrawPath();
    }
}

void AProjectileActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    const UProjectileStaticData* ProjectileStaticData = GetProjectileStaticData();

    if (ProjectileStaticData)
    {
        UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ProjectileStaticData->OnStopSFX, GetActorLocation());
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ProjectileStaticData->OnStopVFX, GetActorLocation());
    }

    Super::EndPlay(EndPlayReason);
}

void AProjectileActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AProjectileActor, ProjectileDataClass);
}

void AProjectileActor::DebugDrawPath() const
{
    const UProjectileStaticData* ProjectileStaticData = GetProjectileStaticData();

    if (ProjectileStaticData)
    {
        FPredictProjectilePathParams PredictParams;
        PredictParams.StartLocation = GetActorLocation();
        PredictParams.LaunchVelocity = ProjectileStaticData->InitialSpeed * GetActorForwardVector();
        PredictParams.TraceChannel = ECollisionChannel::ECC_Visibility;
        PredictParams.bTraceComplex = true;
        PredictParams.bTraceWithCollision = true;
        PredictParams.DrawDebugType = EDrawDebugTrace::ForDuration;
        PredictParams.DrawDebugTime = 3.f;
        PredictParams.OverrideGravityZ =
            ProjectileStaticData->GravityMultiplayer == 0.f ? 0.0001f : ProjectileStaticData->GravityMultiplayer;

        FPredictProjectilePathResult PredictResult;
        if (UGameplayStatics::PredictProjectilePath(GetWorld(), PredictParams, PredictResult))
        {
            DrawDebugSphere(GetWorld(), PredictResult.HitResult.Location, 50.f, 16, FColor::Red);
        }
    }
}

void AProjectileActor::OnProjectileStop(const FHitResult& ImpactResult)
{
    const UProjectileStaticData* ProjectileStaticData = GetProjectileStaticData();

    if (ProjectileStaticData)
    {
        USPStatics::ApplyRadialDamage(this, GetOwner(), GetActorLocation(),  //
            ProjectileStaticData->DamageRadius, ProjectileStaticData->BaseDamage, ProjectileStaticData->Effects,
            ProjectileStaticData->RadialDamageQueryTypes, ProjectileStaticData->RadialDamageTraceType);
    }
    Destroy();
}
