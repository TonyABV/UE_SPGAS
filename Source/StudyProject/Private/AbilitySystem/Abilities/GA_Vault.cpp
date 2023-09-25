// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GA_Vault.h"
#include "StudyProjectCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ActorComponents/SPMotionWarpingComponent.h"

UGA_Vault::UGA_Vault()
{
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UGA_Vault::CommitCheck(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, FGameplayTagContainer* OptionalRelevantTags)
{
    if (!Super::CommitCheck(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags)) return false;

    AStudyProjectCharacter* StudyProjectCharacter = GetSPCharacter();

    if (!IsValid(StudyProjectCharacter)) return false;

    const FVector StartLocation = StudyProjectCharacter->GetActorLocation();
    const FVector ForwardVector = StudyProjectCharacter->GetActorForwardVector();
    const FVector UpVector = StudyProjectCharacter->GetActorUpVector();

    TArray<AActor*> ActorsToIgnore = {StudyProjectCharacter};

    static const auto CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("ShowDebugTraversal"));
    const bool bShowTraversal = CVar->GetInt() > 0;

    EDrawDebugTrace::Type DebugDrawType = bShowTraversal ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;
    
    int32 JumpToLocationIdx = INDEX_NONE;

    int i = 0;

    FHitResult TraceHit;
    
    float MaxJumpDistance = HorizontalTraceLength;

    for (;i<HorizontalTraceCount; ++i)
    {
        const FVector TraceStart = StartLocation + i * UpVector * HorizontalTraceStep;
        const FVector TraceEnd = TraceStart + ForwardVector * HorizontalTraceLength;

        if (UKismetSystemLibrary::SphereTraceSingleForObjects(
                GetWorld(), TraceStart, TraceEnd, HorizontalTraceRadius, TraceObjectType, true, ActorsToIgnore, DebugDrawType, TraceHit, true))
        {
            if (JumpToLocationIdx == INDEX_NONE && (i < HorizontalTraceCount - i))
            {
                JumpToLocationIdx = i;
                JumpToLocation = TraceHit.Location;
            }
            else if (JumpToLocationIdx == (i - 1))
            {
                MaxJumpDistance = FVector::Dist2D(TraceHit.Location, TraceStart);
                break;
            }
        }
        else
        {
            if (JumpToLocationIdx != INDEX_NONE) break;
        }
    }

    if (JumpToLocationIdx == INDEX_NONE) return false;

    const float DistanceToJump = FVector::Dist2D(StartLocation, JumpToLocation);

    const float MaxVerticalTraceDistance = MaxJumpDistance - DistanceToJump;

    if (MaxVerticalTraceDistance < 0) return false;

    if (HorizontalTraceCount == i) i = HorizontalTraceCount - 1;

    const float VerticalTraceLength = //
        FMath::Abs(JumpToLocation.Z - (StartLocation - i * UpVector * HorizontalTraceStep).Z);

    FVector VerticalStartLocation = JumpToLocation + UpVector * VerticalTraceLength;

    i = 0;

    const float TraceCount = MaxVerticalTraceDistance / VerticalTraceStep;

    bool bJumpOverLocationSet = false;

    for (; i < TraceCount; ++i)
    {
        const FVector TraceStart = VerticalStartLocation + i * ForwardVector * VerticalTraceStep;
        const FVector TraceEnd = TraceStart + UpVector * -1 * VerticalTraceLength;

        if (UKismetSystemLibrary::SphereTraceSingleForObjects(GetWorld(), TraceStart, TraceEnd, //
            HorizontalTraceRadius, TraceObjectType, //
            true, ActorsToIgnore, DebugDrawType, TraceHit, true))
        {
            JumpOverLocation = TraceHit.ImpactPoint;
            if (i == 0)
            {
                JumpToLocation = JumpOverLocation;
            }
        }
        else if (i != 0)
        {
            bJumpOverLocationSet = true;
            break;
        }
    }

    if (!bJumpOverLocationSet) return false;

    const FVector TraceStart = JumpOverLocation + ForwardVector * VerticalTraceStep;

    if (UKismetSystemLibrary::SphereTraceSingleForObjects(GetWorld(), TraceStart, JumpOverLocation, //
            HorizontalTraceRadius, TraceObjectType, //
            true, ActorsToIgnore, DebugDrawType, TraceHit, true))
    {
        JumpOverLocation = TraceHit.ImpactPoint;
    }

    if(bShowTraversal)
    {
        DrawDebugSphere(GetWorld(), JumpToLocation, 15, 16, FColor::White, false, 7);
        DrawDebugSphere(GetWorld(), JumpOverLocation, 15, 16, FColor::White, false, 7);
    }

    return true;
}

void UGA_Vault::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        K2_EndAbility();
        return;
    }

    AStudyProjectCharacter* Character = GetSPCharacter();

    UCharacterMovementComponent* CharacterMovement = //
        Character ? Character->GetCharacterMovement() : nullptr;

    if (CharacterMovement)
    {
        CharacterMovement->SetMovementMode(MOVE_Flying);
    }

    UCapsuleComponent* CapsuleComponent = Character ? Character->GetCapsuleComponent() : nullptr;

    if (CapsuleComponent)
    {
        for (ECollisionChannel Channel : CollisionChanelToIgnore)
        {
            CapsuleComponent->SetCollisionResponseToChannel(Channel, ECollisionResponse::ECR_Ignore);
        }
    }

    USPMotionWarpingComponent* MotionWarpingComponent = Character ? Character->GetMotionWarpingComponent() : nullptr;

    if (MotionWarpingComponent)
    {
        MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(
            TEXT("JumpToLocation"), JumpToLocation, Character->GetActorRotation());
        MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(
            TEXT("JumpOverLocation"), JumpOverLocation, Character->GetActorRotation());
    }

    MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, VaultMontage);

    MontageTask->OnBlendOut.AddDynamic(this, &UGA_Vault::K2_EndAbility);
    MontageTask->OnCompleted.AddDynamic(this, &UGA_Vault::K2_EndAbility);
    MontageTask->OnInterrupted.AddDynamic(this, &UGA_Vault::K2_EndAbility);
    MontageTask->OnCancelled.AddDynamic(this, &UGA_Vault::K2_EndAbility);
    MontageTask->ReadyForActivation();
}

void UGA_Vault::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    if (IsValid(MontageTask))
    {
        MontageTask->EndTask();
    }

    AStudyProjectCharacter* Character = GetSPCharacter();

    UCharacterMovementComponent* CharacterMovement =  //
        Character ? Character->GetCharacterMovement() : nullptr;

    if (CharacterMovement && CharacterMovement->IsFlying())
    {
        CharacterMovement->SetMovementMode(MOVE_Falling);
    }

    UCapsuleComponent* CapsuleComponent = Character ? Character->GetCapsuleComponent() : nullptr;

    if (CapsuleComponent)
    {
        for (ECollisionChannel Channel : CollisionChanelToIgnore)
        {
            CapsuleComponent->SetCollisionResponseToChannel(Channel, ECollisionResponse::ECR_Block);
        }
    }

    USPMotionWarpingComponent* MotionWarpingComponent = Character ? Character->GetMotionWarpingComponent() : nullptr;

    if (MotionWarpingComponent)
    {
        MotionWarpingComponent->RemoveWarpTarget(TEXT("JumpToLocation"));
        MotionWarpingComponent->RemoveWarpTarget(TEXT("JumpOverLocation"));
    }

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
