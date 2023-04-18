// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FlecsSystem.generated.h"

/**
 * 
 */



UCLASS(Blueprintable)
class UFlecsEntity : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UFlecsComponent* GetComponent(TSubclassOf<UFlecsComponent> ComponentType);
	
};

UCLASS(Blueprintable,BlueprintType)
class UFlecsComponent : public UObject
{
	GENERATED_BODY()

public:
	
};


UCLASS(Blueprintable)
class FLECSTEST_API UFlecsSystem : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent)
	void OnExecute(UFlecsEntity* CurrentEntity);

	UFUNCTION(BlueprintImplementableEvent)
	void OnInitialize();

	
	UFUNCTION(BlueprintCallable)
	void RegisterSystemComponents(TArray<TSubclassOf<UFlecsComponent>> components);
};
