// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/GA_WallRun.h"

#include "AbilitySystemComponent.h"
#include "StudyProjectCharacter.h"
#include "GameFramework/Character.h"
#include "StudyProjectCharacter.h"
#include "AbilitySystem/AbilityTask/AT_WallRunTick.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_WallRun::UGA_WallRun()
{
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_WallRun::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
    Super::OnGiveAbility(ActorInfo, Spec);

    AStudyProjectCharacter* CharacterAvatar = GetSPCharacter();

    if (CharacterAvatar && CharacterAvatar->GetCapsuleComponent())
    {
        CharacterAvatar->GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &UGA_WallRun::OnCapsuleComponentHit);
    }
}

void UGA_WallRun::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
    if (!ActorInfo) return;

    AStudyProjectCharacter* CharacterAvatar = Cast<AStudyProjectCharacter>(ActorInfo->AvatarActor);

    if (CharacterAvatar && CharacterAvatar->GetCapsuleComponent())
    {
        CharacterAvatar->GetCapsuleComponent()->OnComponentHit.RemoveDynamic(this, &UGA_WallRun::OnCapsuleComponentHit);
    }

    Super::OnRemoveAbility(ActorInfo, Spec);
}

bool UGA_WallRun::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
    if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags)) return false;

    AStudyProjectCharacter* StudyProjectCharacter = GetSPCharacter();

    return StudyProjectCharacter && !StudyProjectCharacter->GetCharacterMovement()->IsMovingOnGround();
}

void UGA_WallRun::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    WallRunTickTask = UAT_WallRunTick::CreateWallRunTask(this, Cast<ACharacter>(GetAvatarActorFromActorInfo()),
        Cast<UCharacterMovementComponent>(ActorInfo->MovementComponent), WallRun_TraceObjectTypes);

    if (IsValid(WallRunTickTask))
    {
        WallRunTickTask->OnWallRunFinished.AddDynamic(this, &UGA_WallRun::K2_EndAbility);
        WallRunTickTask->OnWallSideDetermined.AddDynamic(this, &UGA_WallRun::OnWallSideDetermined);

        WallRunTickTask->ReadyForActivation();
    }
}

void UGA_WallRun::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    if (IsValid(WallRunTickTask))
    {
        WallRunTickTask->EndTask();
    }

    UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();

    if (ASC)
    {
        ASC->RemoveActiveGameplayEffectBySourceEffect(WallRunLeftSideEffectClass, ASC);
        ASC->RemoveActiveGameplayEffectBySourceEffect(WallRunRightSideEffectClass, ASC);
    }

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_WallRun::OnCapsuleComponentHit(
    UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();

    if (ASC)
    {
        ASC->TryActivateAbility(GetCurrentAbilitySpec()->Handle);
    }
}

void UGA_WallRun::OnWallSideDetermined(bool bLeftSide)
{
    AStudyProjectCharacter* SPCharc = GetSPCharacter();
    UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();

    FGameplayEffectContextHandle EffectContextHandle = ASC->MakeEffectContext();

    if (bLeftSide)
    {
        SPCharc->ApplayGameplayEffectToSelf(WallRunLeftSideEffectClass, EffectContextHandle);
    }
    else
    {
        SPCharc->ApplayGameplayEffectToSelf(WallRunRightSideEffectClass, EffectContextHandle);
    }
}
