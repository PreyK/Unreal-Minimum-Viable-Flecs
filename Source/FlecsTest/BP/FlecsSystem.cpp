// Fill out your copyright notice in the Description page of Project Settings.


#include "FlecsSystem.h"

UFlecsComponent* UFlecsEntity::GetComponent(TSubclassOf<UFlecsComponent> ComponentType)
{
	return nullptr;
}

void UFlecsSystem::RegisterSystemComponents(TArray<TSubclassOf<UFlecsComponent>> components)
{
}
