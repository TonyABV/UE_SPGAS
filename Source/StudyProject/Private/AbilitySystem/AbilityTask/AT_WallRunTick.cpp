// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTask/AT_WallRunTick.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

UAT_WallRunTick* UAT_WallRunTick::CreateWallRunTask(UGameplayAbility* OwningAbility, ACharacter* InCharacterOwner,
    UCharacterMovementComponent* InCharacterMovement, TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes)
{
    UAT_WallRunTick* WallRunTask = NewAbilityTask<UAT_WallRunTick>(OwningAbility);

    WallRunTask->CharacterOwner = InCharacterOwner;
    WallRunTask->CharacterMovement = InCharacterMovement;
    WallRunTask->WallRun_TraceObjectTypes = TraceObjectTypes;
    WallRunTask->bTickingTask = true;

    return WallRunTask;
}

void UAT_WallRunTick::Activate()
{
    Super::Activate();

    FHitResult OnWallHit;

    //const FVector CurrentAcceleration = CharacterMovement->GetCurrentAcceleration();

    if (!FindRunnableWall(OnWallHit))
    {
        if (ShouldBroadcastAbilityTaskDelegates()) OnWallRunFinished.Broadcast();

        EndTask();

        return;
    }

    OnWallSideDetermined.Broadcast(IsWallOnTheLeft(OnWallHit));

    CharacterOwner->Landed(OnWallHit);

    CharacterOwner->SetActorLocation(OnWallHit.ImpactPoint + OnWallHit.ImpactNormal * 60.f);

    CharacterMovement->SetMovementMode(MOVE_Flying);
}

void UAT_WallRunTick::OnDestroy(bool bInOwnerFinished)
{
    CharacterMovement->SetPlaneConstraintEnabled(false);
    CharacterMovement->SetMovementMode(MOVE_Falling);
    
    Super::OnDestroy(bInOwnerFinished);
}

void UAT_WallRunTick::TickTask(float DeltaTime)
{
    Super::TickTask(DeltaTime);

    FHitResult OnWallHit;

    //const FVector CurrentAcceleration = CharacterMovement->GetCurrentAcceleration();

    if (!FindRunnableWall(OnWallHit))
    {
        if (ShouldBroadcastAbilityTaskDelegates()) OnWallRunFinished.Broadcast();

        EndTask();
        return;
    }

    const FRotator DirectionRotator = IsWallOnTheLeft(OnWallHit) ?  //
                                          FRotator(0, -90, 0)
                                                        : FRotator(0, 90, 0);

    const FVector WallRunDirection = DirectionRotator.RotateVector(OnWallHit.ImpactNormal);

    CharacterMovement->Velocity = WallRunDirection * CharacterMovement->MaxWalkSpeed;

    CharacterMovement->Velocity.Z = CharacterMovement->GetGravityZ() * DeltaTime;

    CharacterOwner->SetActorRotation(WallRunDirection.Rotation());

    CharacterMovement->SetPlaneConstraintEnabled(true);
    CharacterMovement->SetPlaneConstraintOrigin(OnWallHit.ImpactPoint);
    CharacterMovement->SetPlaneConstraintNormal(OnWallHit.ImpactNormal);
}

bool UAT_WallRunTick::FindRunnableWall(FHitResult& OnWallHit)
{
    const FVector CharacterLocation = CharacterOwner->GetActorLocation();

    const FVector RightVector = CharacterOwner->GetActorRightVector();
    const FVector ForwardVector = CharacterOwner->GetActorForwardVector();

    const float TraceLength = CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius() + 30.f;

    TArray<AActor*> ActorsToIgnore = {CharacterOwner};

    static const auto CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("ShowDebugTraversal"));
    const bool bShowTraversal = CVar->GetInt() > 0;

    EDrawDebugTrace::Type DebugDrawType = bShowTraversal ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;
    
    if (UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), CharacterLocation, CharacterLocation + ForwardVector * TraceLength,
            WallRun_TraceObjectTypes, true, ActorsToIgnore, DebugDrawType, OnWallHit, true))
    {
        return false;
    }

    //LeftTrace
    if (UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), CharacterLocation, CharacterLocation - RightVector * TraceLength,
            WallRun_TraceObjectTypes, true, ActorsToIgnore, DebugDrawType, OnWallHit, true))
    {
        if (FVector::DotProduct(OnWallHit.ImpactNormal, RightVector) > 0.3f) return true;
    }

    //RightTrace
    if (UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), CharacterLocation, CharacterLocation + RightVector * TraceLength,
            WallRun_TraceObjectTypes, true, ActorsToIgnore, DebugDrawType, OnWallHit, true))
    {
        if (FVector::DotProduct(OnWallHit.ImpactNormal, -RightVector) > 0.3f) return true;
    }

    return false;
}

bool UAT_WallRunTick::IsWallOnTheLeft(const FHitResult& InWallHit) const
{
    return FVector::DotProduct(CharacterOwner->GetActorRightVector(), InWallHit.ImpactNormal) > 0.f;
}
