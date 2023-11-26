// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SPTypes.generated.h"

class AItemActor;
class UGameplayAbility;
class USkeletalMesh;
class UStaticMesh;
class UAnimMontage;
class UNiagaraSystem;

USTRUCT(BlueprintType)
struct FCharacterData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS")
	TArray<TSubclassOf<class UGameplayEffect>> Effects;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS")
	TArray<TSubclassOf<class UGameplayAbility>> Abilities;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UCharacterAnimationDataAsset* CharacterAnimData;
	
};

USTRUCT(BlueprintType)
struct FCharacterAnimationData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	class UBlendSpace* MovementBlendSpace = nullptr;

	UPROPERTY(EditDefaultsOnly)
	class UAnimSequenceBase* IdleAnimationAsset = nullptr;

	UPROPERTY(EditDefaultsOnly)
	class UBlendSpace* CrouchingBlendSpace = nullptr;

	UPROPERTY(EditDefaultsOnly)
	class UAnimSequenceBase* CrouchIdleAnimationAsset = nullptr;

};

UENUM(BlueprintType)
enum class EFoot : uint8
{
	Left UMETA(DisplayName = "Left"),
	Right UMETA(DisplayName = "Right")
};

UCLASS(BlueprintType, Blueprintable)
class UItemStaticData : public UObject
{
    GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<AItemActor> ItemActorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName AttachmentSocket = NAME_None;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    bool bCanBeEquipped = false;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FCharacterAnimationData CharacterAnimationData;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TArray<TSubclassOf<UGameplayAbility>> GrantedAbilities;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TArray<TSubclassOf<UGameplayEffect>> OngoingEffects;
    
};

UCLASS(BlueprintType, Blueprintable)
class UWeaponStaticData : public UItemStaticData
{
    GENERATED_BODY()

public:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UGameplayEffect> DamageEffect;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    USkeletalMesh* SkeletalMesh;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UStaticMesh* StaticMesh;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UAnimMontage* AttackMontage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float FireRate;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float BaseDamage;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    USoundBase* AttackSound;

};

UENUM(BlueprintType)
enum EItemState : uint8
{
    None UMETA(DisplayName = "None"),
    Equipped UMETA(DisplayName = "Equipped"),
    Dropped UMETA(DisplayName = "Dropped")
};

UENUM(BlueprintType)
enum class EMovementDirectionType : uint8
{
    None UMETA(DisplayName = "None"),
    OrientToMovement UMETA(DisplayName = "OrientToMovement"),
    Strafe UMETA(DisplayName = "Strafe")
    
};


UCLASS(BlueprintType, Blueprintable)
class UProjectileStaticData : public UObject
{
    GENERATED_BODY()

public:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float BaseDamage;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float DamageRadius;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float GravityMultiplayer;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float InitialSpeed;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float MaxSpeed;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UStaticMesh* StaticMesh;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TArray<TSubclassOf<UGameplayEffect>> Effects;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TArray<TEnumAsByte<EObjectTypeQuery>> RadialDamageQueryTypes;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TEnumAsByte<ETraceTypeQuery> RadialDamageTraceType;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UNiagaraSystem*  OnStopVFX = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    USoundBase* OnStopSFX = nullptr;

};