// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "SPTypes.h"
#include "ItemActor.generated.h"

class UInventoryItemInstance;
class USphereComponent;

UCLASS()
class STUDYPROJECT_API AItemActor : public AActor
{
	GENERATED_BODY()
	
public:

	AItemActor();

    virtual void OnEquipped();
    virtual void OnUnequipped();
    virtual void OnDropped();

	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void Init(UInventoryItemInstance* InInstance);

    UFUNCTION()
    void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
    USphereComponent* SphereComponent;

	UPROPERTY(Replicated)
    UInventoryItemInstance* ItemInstance = nullptr;

	UPROPERTY(Replicated)
    TEnumAsByte<EItemState> ItemState = EItemState::None;

    FGameplayTag OverlappedEventTag;

public:	

	virtual void Tick(float DeltaTime) override;

};
