// Copyright Epic Games, Inc. All Rights Reserved.

#include "StudyProjectCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemLog.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystem/AttributeSet/SPAttributeSetBase.h"
#include "AbilitySystem/Components/SP_AbilitySystemComponentBase.h"
#include "ActorComponents/InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "ActorComponents/SPCharacterMovementComponent.h"
#include "ActorComponents/SPFootstepsComponent.h"
#include "ActorComponents/SPMotionWarpingComponent.h"


//////////////////////////////////////////////////////////////////////////
// AStudyProjectCharacter

AStudyProjectCharacter::AStudyProjectCharacter(const FObjectInitializer& ObjectInitializer):
Super(ObjectInitializer.SetDefaultSubobjectClass<USPCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	SPMovementComponent = Cast<USPCharacterMovementComponent>(GetCharacterMovement());

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	//AbilitySystem

	AbilitySystemComponent = CreateDefaultSubobject<USP_AbilitySystemComponentBase>(TEXT("AbilitySystem"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxMovementSpeedAttribute())
        .AddUObject(this, &AStudyProjectCharacter::OnMaxMovementSpeedChanged);
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute())//
        .AddUObject(this, &AStudyProjectCharacter::OnHealthAttributeChanged);
    
    AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(TEXT("State.Ragdoll")), //
        EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AStudyProjectCharacter::OnRagdollStateChanged);
    
	AttributeSet = CreateDefaultSubobject<USPAttributeSetBase>(TEXT("AttributeSet"));

	FootstepsComponent = CreateDefaultSubobject<USPFootstepsComponent>(TEXT("FootstepsComponent"));

	//MotionWarping

	MotionWarpingComponent = CreateDefaultSubobject<USPMotionWarpingComponent>("MotionWarping");

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>("Inventory");
    InventoryComponent->SetIsReplicated(true);

}

bool AStudyProjectCharacter::ApplayGameplayEffectToSelf(TSubclassOf<UGameplayEffect> Effect,
	FGameplayEffectContextHandle InEffectContext)
{
	if (!Effect.Get()) return false;

	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(Effect, 1, InEffectContext);
	if(SpecHandle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		return ActiveGEHandle.WasSuccessfullyApplied();
	}

	return false;
}

UAbilitySystemComponent* AStudyProjectCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AStudyProjectCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(AStudyProjectCharacter, CharacterData, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME(AStudyProjectCharacter, InventoryComponent);
}

void AStudyProjectCharacter::SetCharacterData(const FCharacterData& InCharacterData)
{
	CharacterData = InCharacterData;
	InitFromCharacterData(InCharacterData);
}

void AStudyProjectCharacter::PostLoad()
{
	Super::PostLoad();

	if(IsValid(CharacterDataAsset))
	{
		SetCharacterData(CharacterDataAsset->CharacterData);
	}
}

void AStudyProjectCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if(AbilitySystemComponent)
	{
		AbilitySystemComponent->RemoveActiveEffectsWithTags(InAirTags);
	}
}

void AStudyProjectCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
    Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	if (!CrouchStateEffect.Get()) return;;

    if (AbilitySystemComponent)
    {
        FGameplayEffectSpecHandle SpecHandle =
            AbilitySystemComponent->MakeOutgoingSpec(CrouchStateEffect, 1, AbilitySystemComponent->MakeEffectContext());

        if (SpecHandle.IsValid())
        {
            FActiveGameplayEffectHandle ActiveGEHandle = //
				AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
            if (!ActiveGEHandle.WasSuccessfullyApplied())
            {
                ABILITY_LOG(Log, TEXT("Ability %s failed to apply croush effect %s"), *GetNameSafe(CrouchStateEffect));
            }
        }
    }
}

void AStudyProjectCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->RemoveActiveGameplayEffectBySourceEffect(CrouchStateEffect, AbilitySystemComponent);
    }
    Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

void AStudyProjectCharacter::StartRagdoll()
{
    USkeletalMeshComponent* SKMesh = GetMesh();
    if(IsValid(SKMesh) && !SKMesh->IsSimulatingPhysics())
    {
        SKMesh->SetCollisionProfileName(TEXT("Ragdoll"));
        SKMesh->SetSimulatePhysics(true);
        SKMesh->SetAllPhysicsLinearVelocity(FVector());
        SKMesh->SetAllPhysicsAngularVelocityInRadians(FVector());
        SKMesh->WakeAllRigidBodies();
        GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);        
    }
}

void AStudyProjectCharacter::GiveAbilities()
{
	if (HasAuthority() && AbilitySystemComponent)
	{
		for(const auto DefaultAbility : CharacterData.Abilities)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(DefaultAbility));
		}
	}
}

void AStudyProjectCharacter::ApplyStartupEffects()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		EffectContext.AddSourceObject(this);

		for(const auto CharacterEffect : CharacterData.Effects)
		{
			ApplayGameplayEffectToSelf(CharacterEffect, EffectContext);
		}
	}
}

void AStudyProjectCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Init AS on server
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	
	GiveAbilities();
	ApplyStartupEffects();
}

void AStudyProjectCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Init AS on client
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

void AStudyProjectCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AStudyProjectCharacter::OnMaxMovementSpeedChanged(const FOnAttributeChangeData& Data)
{
    if (GetCharacterMovement())
    {
        GetCharacterMovement()->MaxWalkSpeed = Data.NewValue;
    }
}

void AStudyProjectCharacter::OnHealthAttributeChanged(const FOnAttributeChangeData& Data)
{
    if(Data.NewValue <= 0 && Data.OldValue > 0)
    {
        AStudyProjectCharacter* OtherCharacter = nullptr;

        if(Data.GEModData)
        {
            const FGameplayEffectContextHandle& EffectContext = Data.GEModData->EffectSpec.GetEffectContext();
            OtherCharacter = Cast<AStudyProjectCharacter>(EffectContext.GetInstigator());
        }

        FGameplayEventData EventPayload;
        EventPayload.EventTag = ZeroHealthEventTag;

        UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, ZeroHealthEventTag, EventPayload);
    }
}

//////////////////////////////////////////////////////////////////////////
// Input

void AStudyProjectCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Jumping
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AStudyProjectCharacter::OnJump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AStudyProjectCharacter::OnStopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AStudyProjectCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AStudyProjectCharacter::Look);

		//Crouch
        EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AStudyProjectCharacter::OnCrouch);
        EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AStudyProjectCharacter::OuStopCrouch);

		//Sprint
        EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AStudyProjectCharacter::OnSprint);
        EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AStudyProjectCharacter::OnStopSprint);

	    //Equip, Drop, Unequip item.
        EnhancedInputComponent->BindAction(EquipNextInputAction, ETriggerEvent::Started, this, &AStudyProjectCharacter::OnEquipNextTriggered);
	    EnhancedInputComponent->BindAction(DropItemInputAction, ETriggerEvent::Started, this, &AStudyProjectCharacter::OnDropItemTriggered);
	    EnhancedInputComponent->BindAction(UnequipInputAction, ETriggerEvent::Started, this, &AStudyProjectCharacter::OnUnequipTriggered);

	    //Attack
	    EnhancedInputComponent->BindAction(AttackInputAction, ETriggerEvent::Started, this, &AStudyProjectCharacter::OnAttackActionStarted);
	    EnhancedInputComponent->BindAction(AttackInputAction, ETriggerEvent::Completed, this, &AStudyProjectCharacter::OnAttackActionEnded);

	    EnhancedInputComponent->BindAction(AimInputAction, ETriggerEvent::Started, this, &AStudyProjectCharacter::OnAimActionStarted);
	    EnhancedInputComponent->BindAction(AimInputAction, ETriggerEvent::Completed, this, &AStudyProjectCharacter::OnAimActionEnded);
   
	}

}

void AStudyProjectCharacter::OnRep_CharacterData()
{
	InitFromCharacterData(CharacterData, true);
}

void AStudyProjectCharacter::OnJump(const FInputActionValue& InputActionValue)
{
	/*FGameplayEventData Payload;

	Payload.Instigator = this;
	Payload.EventTag = JumpEventTag;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, JumpEventTag, Payload);*/

	SPMovementComponent->TryTraversal(AbilitySystemComponent);
}

void AStudyProjectCharacter::OnStopJumping(const FInputActionValue& InputActionValue)
{
}

void AStudyProjectCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AStudyProjectCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AStudyProjectCharacter::InitFromCharacterData(const FCharacterData& InCharacterData, bool bFromReplication)
{

}

void AStudyProjectCharacter::OnRagdollStateChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
     if(NewCount > 0)
     {
         StartRagdoll();
     }
}

void AStudyProjectCharacter::OnCrouch(const FInputActionValue& InputActionValue)
{
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->TryActivateAbilitiesByTag(CrouchTags, true);
    }
}

void AStudyProjectCharacter::OuStopCrouch(const FInputActionValue& InputActionValue)
{
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->CancelAbilities(&CrouchTags);
    }
}

void AStudyProjectCharacter::OnSprint(const FInputActionValue& InputActionValue)
{
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->TryActivateAbilitiesByTag(SprintTags, true);
    }
}

void AStudyProjectCharacter::OnStopSprint(const FInputActionValue& InputActionValue)
{
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->CancelAbilities(&SprintTags);
    }
}

void AStudyProjectCharacter::OnDropItemTriggered(const FInputActionValue& InputActionValue)
{
    FGameplayEventData EventPayload;
    EventPayload.EventTag = UInventoryComponent::DropItemTag;

    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, UInventoryComponent::DropItemTag, EventPayload);
}

void AStudyProjectCharacter::OnEquipNextTriggered(const FInputActionValue& InputActionValue)
{
    FGameplayEventData EventPayload;
    EventPayload.EventTag = UInventoryComponent::EquipNextItemTag;

    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, UInventoryComponent::EquipNextItemTag, EventPayload);
}

void AStudyProjectCharacter::OnUnequipTriggered(const FInputActionValue& InputActionValue)
{
    FGameplayEventData EventPayload;
    EventPayload.EventTag = UInventoryComponent::UnequipItemTag;

    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, UInventoryComponent::UnequipItemTag, EventPayload);
}

void AStudyProjectCharacter::OnAttackActionStarted(const FInputActionValue& InputActionValue)
{
    FGameplayEventData EventPayload;
    EventPayload.EventTag = StartAttackEventTag;

    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, StartAttackEventTag, EventPayload);
}

void AStudyProjectCharacter::OnAttackActionEnded(const FInputActionValue& InputActionValue)
{
    FGameplayEventData EventPayload;
    EventPayload.EventTag = EndAttackEventTag;

    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, EndAttackEventTag, EventPayload);
}

void AStudyProjectCharacter::OnAimActionStarted(const FInputActionValue& InputActionValue)
{
    FGameplayEventData EventPayload;
    EventPayload.EventTag = StartAimEventTag;

    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, StartAimEventTag, EventPayload);
}

void AStudyProjectCharacter::OnAimActionEnded(const FInputActionValue& InputActionValue)
{
    FGameplayEventData EventPayload;
    EventPayload.EventTag = EndAimEventTag;

    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, EndAimEventTag, EventPayload);
}
