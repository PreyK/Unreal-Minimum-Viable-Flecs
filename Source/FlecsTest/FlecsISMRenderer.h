// Copyright 2021 Red J
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "FlecsISMRenderer.generated.h"

UCLASS()
class FLECSTEST_API AFlecsISMRenderer : public AActor
{
	GENERATED_BODY()
public:	
	AFlecsISMRenderer();

public:
	UPROPERTY(EditAnywhere)
	UInstancedStaticMeshComponent* InstancedStaticMeshComponent = nullptr;

	
public:
	void Initialize(UStaticMesh* InMesh) const;
	int32 GetInstanceCount() const;
	void CreateOrExpandTransformArray();
	int32 AddInstance(FTransform inTransform);
	int32 AddInstance();
	void SetTransform(int32 instanceIndex, const FTransform& transform);
	void RemoveInstance(int32 instanceIndex);
	void BatchUpdateTransform() const;
	void SetCustomData(int32 instanceIndex, float data) const;
	

private:
	TArray<FTransform> transforms;
	TQueue<int> indexPool;


protected:
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;
};
