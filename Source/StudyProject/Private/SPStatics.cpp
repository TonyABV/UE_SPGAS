// Fill out your copyright notice in the Description page of Project Settings.


#include "SPStatics.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Actors/ProjectileActor.h"
#include "Kismet/KismetSystemLibrary.h"

static TAutoConsoleVariable<int32> CVarShowRadialDamage(
    TEXT("ShowRadialDamage"), //
    0, //
    TEXT("Draws debug info about radial damage/n 0: off/n 1: on"), //
    ECVF_Cheat);

const UItemStaticData* USPStatics::GetItemStaticData(TSubclassOf<UItemStaticData> ItemDataClass)
{
    if (!ItemDataClass) return nullptr;

    return GetDefault<UItemStaticData>(ItemDataClass);
}

void USPStatics::ApplyRadialDamage(UObject* WorldContextObject, AActor* DamageCauser, FVector Location, float Radius, float DamageAmount,
    TArray<TSubclassOf<UGameplayEffect>> DamageEffects, const TArray<TEnumAsByte<EObjectTypeQuery>>& ObjectTypes, ETraceTypeQuery TraceType)
{
    TArray<AActor*> OutActors;
    TArray<AActor*> ActorsToIgnore = {DamageCauser};

    UKismetSystemLibrary::SphereOverlapActors(WorldContextObject, Location, Radius, ObjectTypes, nullptr, ActorsToIgnore, OutActors);

    const bool bDebug = static_cast<bool>(CVarShowRadialDamage.GetValueOnAnyThread());

    for(AActor* Actor : OutActors)
    {
        FHitResult HitResult;
        if(UKismetSystemLibrary::LineTraceSingle(WorldContextObject, Location, Actor->GetActorLocation(), TraceType, true, //
            ActorsToIgnore, EDrawDebugTrace::None, HitResult, true))
        {
            AActor* Target = HitResult.GetActor();
            
            if(Actor == Target)
            {
                bool bWasApplied = false;
                
                if(UAbilitySystemComponent* ASComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target))
                {
                    FGameplayEffectContextHandle EffectConstants = ASComponent->MakeEffectContext();
                    EffectConstants.AddInstigator(DamageCauser, DamageCauser);

                    for(auto Effect : DamageEffects)
                    {
                        FGameplayEffectSpecHandle SpecHandle = ASComponent->MakeOutgoingSpec(Effect, 1, EffectConstants);
                        if(SpecHandle.IsValid())
                        {
                            UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, //
                                FGameplayTag::RequestGameplayTag(TEXT("Attribute.Health")), -DamageAmount);

                            FActiveGameplayEffectHandle ActiveGEHandle = ASComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
                            if(ActiveGEHandle.WasSuccessfullyApplied())
                            {
                                bWasApplied = true;
                            }
                        }
                        
                    }
                    
                }

                if(bDebug)
                {
                    DrawDebugLine(WorldContextObject->GetWorld(), Location, Actor->GetActorLocation(), //
                        bWasApplied ? FColor::Green :FColor::Red, false, 4.f, 0,1);
                    DrawDebugSphere(WorldContextObject->GetWorld(), HitResult.Location, 16, 16, //
                        bWasApplied ? FColor::Green :FColor::Red, false, 4.f, 0,1);
                    DrawDebugString(WorldContextObject->GetWorld(), HitResult.Location, *GetNameSafe(Target), //
                        nullptr, FColor::White, 0, false, 1.f);
                }
               
            }
            else
             {
                if(bDebug)
                {
                    DrawDebugLine(WorldContextObject->GetWorld(), Location, Actor->GetActorLocation(), //
                        FColor::Red, false, 4.f, 0,1);
                    DrawDebugSphere(WorldContextObject->GetWorld(), HitResult.Location, 16, 16, //
                        FColor::Red, false, 4.f, 0,1);
                    DrawDebugString(WorldContextObject->GetWorld(), HitResult.Location, *GetNameSafe(Target), //
                        nullptr, FColor::Red, 0, false, 1.f);
                }   
             }
        }
        else
        {
            if(bDebug)
            {
                DrawDebugLine(WorldContextObject->GetWorld(), Location, Actor->GetActorLocation(), //
                    FColor::Red, false, 4.f, 0,1);
                DrawDebugSphere(WorldContextObject->GetWorld(), HitResult.Location, 16, 16, //
                    FColor::Red, false, 4.f, 0,1);
                DrawDebugString(WorldContextObject->GetWorld(), HitResult.Location, *GetNameSafe(HitResult.GetActor()), //
                    nullptr, FColor::Red, 0, false, 1.f);
            }  
        }
    }

    if(bDebug)
    {
        DrawDebugSphere(WorldContextObject->GetWorld(), Location, Radius, 16, FColor::White, false, 4.f, 0, 1.f);
    }
}

AProjectileActor* USPStatics::LaunchProjectile(UObject* WorldContextObject, TSubclassOf<UProjectileStaticData> ProjectileDataClass,
    FTransform Transform, AActor* Owner, APawn* Instigator)
{
    UWorld* World = WorldContextObject ? WorldContextObject->GetWorld() : nullptr;

    if(World && !World->IsNetMode(NM_Client))
    {
        if(AProjectileActor* ProjectileActor = World->SpawnActorDeferred<AProjectileActor>(AProjectileActor::StaticClass(), Transform, //
            Owner, Instigator, ESpawnActorCollisionHandlingMethod::AlwaysSpawn))
        {
            ProjectileActor->ProjectileDataClass = ProjectileDataClass;
            ProjectileActor->FinishSpawning(Transform);

            return ProjectileActor;
        }
    }
    return nullptr;
}
