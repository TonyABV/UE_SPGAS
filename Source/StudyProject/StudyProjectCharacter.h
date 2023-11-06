// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystemInterface.h"
#include "SPTypes.h"
#include "DataAssets/CharacterDataAsset.h"
#include "ActorComponents/SPMotionWarpingComponent.h"
#include "StudyProjectCharacter.generated.h"

class USP_AbilitySystemComponentBase;
class USPAttributeSetBase;

class UGameplayEffect;
class UGameplayAbility;
class USPFootstepsComponent;
class USPMotionWarpingComponent;
class USPCharacterMovementComponent;
class UInventoryComponent;

UCLASS(config=Game)
class AStudyProjectCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/** Crouch Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* CrouchAction;

	/** Crouch Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SprintAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* EquipNextInputAction;
    
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* DropItemInputAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* UnequipInputAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    class UInputAction* AttackInputAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MotionWarp, meta = (AllowPrivateAccess = "true"))
    USPMotionWarpingComponent* MotionWarpingComponent;

	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    UInventoryComponent* InventoryComponent;

	USPCharacterMovementComponent* SPMovementComponent = nullptr;

public:

	AStudyProjectCharacter(const FObjectInitializer& ObjectInitializer);
	
	bool ApplayGameplayEffectToSelf(TSubclassOf<UGameplayEffect> Effect, FGameplayEffectContextHandle InEffectContext);

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	FCharacterData GetCharacterData() const { return CharacterData; }

	UFUNCTION(BlueprintCallable)
	void SetCharacterData(const FCharacterData& InCharacterData);

	virtual void PostInitializeComponents() override;

	USPFootstepsComponent* GetFootStepComponent() const { return FootstepsComponent; }

	virtual void Landed(const FHitResult& Hit) override;

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
    virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	UPROPERTY(EditDefaultsOnly)
	UCharacterDataAsset* CharacterDataAsset;

	UPROPERTY(BlueprintReadOnly)
	USPFootstepsComponent* FootstepsComponent;

	UFUNCTION(BlueprintCallable)
    USPMotionWarpingComponent* GetMotionWarpingComponent() const { return MotionWarpingComponent; }

    
    UInventoryComponent* GetInventoryComponent() const { return  InventoryComponent; }
    
protected:
	
	void GiveAbilities();
	void ApplyStartupEffects();

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	UPROPERTY(EditDefaultsOnly)
	USP_AbilitySystemComponentBase* AbilitySystemComponent;

	UPROPERTY(Transient)
	USPAttributeSetBase* AttributeSet;

	UPROPERTY(ReplicatedUsing = OnRep_CharacterData)
	FCharacterData CharacterData;

	UFUNCTION()
	void OnRep_CharacterData();

	void OnJump(const FInputActionValue& InputActionValue);

	void OnStopJumping(const FInputActionValue& InputActionValue);

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

    void OnCrouch(const FInputActionValue& InputActionValue);
	
    void OuStopCrouch(const FInputActionValue& InputActionValue);
	
    void OnSprint(const FInputActionValue& InputActionValue);
	
    void OuStopSprint(const FInputActionValue& InputActionValue);

    void OuDropItemTriggered(const FInputActionValue& InputActionValue);
    	
    void OnEquipNextTriggered(const FInputActionValue& InputActionValue);
    	
    void OuUnequipTriggered(const FInputActionValue& InputActionValue);

    void OuAttackActionStarted(const FInputActionValue& InputActionValue);

    void OuAttackActionEnded(const FInputActionValue& InputActionValue);
    
	virtual void InitFromCharacterData(const FCharacterData& InCharacterData, bool bFromReplication = false);

protected:// APawn interface

    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	
protected: // Gameplay Events

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag JumpEventTag;
    
    UPROPERTY(EditDefaultsOnly)
    FGameplayTag StartAttackEventTag;
    
    UPROPERTY(EditDefaultsOnly)
    FGameplayTag EndAttackEventTag;
	
protected: // Gameplay Tags

	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer InAirTags;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer CrouchTags;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer SprintTags;

protected: // Gameplay Effects

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> CrouchStateEffect;

protected: //Delegates

	FDelegateHandle MaxMoveSpeedChangedDelegateHandle;

private:

	void OnMaxMovementSpeedChanged(const FOnAttributeChangeData& Data);

};

