// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/SPGAJump.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

USPGAJump::USPGAJump()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;
}

bool USPGAJump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags)) return false;

	ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get(), ECastCheckedType::NullAllowed);
	if (!Character) return false;

	const bool bMovementAllowsJump = Character->GetCharacterMovement()->IsJumpAllowed();

	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Character);
    const bool bIsWallRunning = ASC && ASC->HasMatchingGameplayTag(WallRunStatTag);

	return Character->CanJump() || (bMovementAllowsJump && bIsWallRunning);
}

void USPGAJump::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
    {
        if (!CommitAbility(Handle, ActorInfo, ActivationInfo)) return;

        Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

        ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());

        if (!IsValid(Character)) return;

        UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Character);
        const bool bIsWallRunning = ASC && ASC->HasMatchingGameplayTag(WallRunStatTag);

        if (bIsWallRunning)
        {
            FGameplayTagContainer WallRunTags(WallRunStatTag);

            ASC->CancelAbilities(&WallRunTags);

            FVector JumpOffVector = Character->GetCharacterMovement()->GetCurrentAcceleration().GetSafeNormal() + FVector::UpVector;

            Character->LaunchCharacter(JumpOffVector * OffWallJumpStrength, true, true);
        }
        else
        {
            Character->Jump();
        }
	}
}
