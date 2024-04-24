// Fill out your copyright notice in the Description page of Project Settings.

#include "ActorComponents/SPCharacterMovementComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"

static TAutoConsoleVariable<int32> CVarShowTraversal(TEXT("ShowDebugTraversal"),  //
    0,                                                                            //
    TEXT("Draws debug info about traversal"),                                     //
    ECVF_Cheat);

bool USPCharacterMovementComponent::TryTraversal(UAbilitySystemComponent* ASC)
{
    for (TSubclassOf<UGameplayAbility> AbilityClass : TraversalAbilitiesOrdered)
    {
        if (ASC->TryActivateAbilityByClass(AbilityClass))
        {
            const FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromClass(AbilityClass);

            if (Spec && Spec->IsActive())
            {
                return true;
            }
        }
    }
    return false;
}

void USPCharacterMovementComponent::BeginPlay()
{
    Super::BeginPlay();

    HandleMovementDirection();

    if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
    {
        AbilitySystemComponent
            ->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(TEXT("Movement.Enforced.Strafe"),  //
                EGameplayTagEventType::NewOrRemoved))
            .AddUObject(this, &USPCharacterMovementComponent::OnEnforcedStrafeTagChanged);
    }
}

EMovementDirectionType USPCharacterMovementComponent::GetMovementDirectionType() const
{
    return MovementDirectionType;
}

void USPCharacterMovementComponent::SetMovementDirectionType(EMovementDirectionType InMovementDirectionType)
{
    MovementDirectionType = InMovementDirectionType;

    HandleMovementDirection();
}

void USPCharacterMovementComponent::OnEnforcedStrafeTagChanged(const FGameplayTag CallbackTag, int32 NewCound)
{
    if (NewCound)
    {
        SetMovementDirectionType(EMovementDirectionType::Strafe);
    }
    else
    {
        SetMovementDirectionType(EMovementDirectionType::OrientToMovement);
    }
}

void USPCharacterMovementComponent::HandleMovementDirection()
{
    switch (MovementDirectionType)
    {
        case EMovementDirectionType::Strafe:
            bUseControllerDesiredRotation = true;
            bOrientRotationToMovement = false;
            CharacterOwner->bUseControllerRotationYaw = true;
            break;
        default:
            bUseControllerDesiredRotation = false;
            bOrientRotationToMovement = true;
            CharacterOwner->bUseControllerRotationYaw = false;
            break;
    }
}
