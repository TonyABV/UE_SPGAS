// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SPTypes.generated.h"

class AItemActor;

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
	
};

UENUM(BlueprintType)
enum EItemState : uint8
{
    None UMETA(DisplayName = "None"),
    Equipped UMETA(DisplayName = "Equipped"),
    Dropped UMETA(DisplayName = "Dropped")
};
