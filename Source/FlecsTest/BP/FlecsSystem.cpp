// Fill out your copyright notice in the Description page of Project Settings.


#include "FlecsSystem.h"
/*
void UFlecsSystem::Native_Implementation()
{
}

void UFlecsSystem::Callable()
{
}

bool UFlecsSystem::Pure()
{
	return true;
}
*/

UFlecsComponent* UFlecsEntity::GetComponent(TSubclassOf<UFlecsComponent> ComponentType)
{
	return nullptr;
}

void UFlecsSystem::RegisterSystemComponents(TArray<TSubclassOf<UFlecsComponent>> components)
{
}
