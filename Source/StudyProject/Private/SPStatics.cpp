// Fill out your copyright notice in the Description page of Project Settings.


#include "SPStatics.h"

const UItemStaticData* USPStatics::GetItemStaticData(TSubclassOf<UItemStaticData> ItemDataClass)
{
    if (!ItemDataClass) return nullptr;

    return GetDefault<UItemStaticData>(ItemDataClass);
}
