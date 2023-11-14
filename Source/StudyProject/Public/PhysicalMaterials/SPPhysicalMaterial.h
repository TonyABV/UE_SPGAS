// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "SPPhysicalMaterial.generated.h"

class USoundBase;
class UNiagaraSystem;

UCLASS()
class STUDYPROJECT_API USPPhysicalMaterial : public UPhysicalMaterial
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundBase* FootStepSound;

    UPROPERTY(EditDefaultsOnly, Category = "Sound")
    class USoundBase* PointImpactSound = nullptr;
    
    UPROPERTY(EditDefaultsOnly, Category = "VFX")
    UNiagaraSystem* PointImpactVFX = nullptr;

};
