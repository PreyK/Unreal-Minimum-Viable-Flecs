#pragma once
#include "flecs.h"
#include "CoreMinimal.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "FlecsSubsystem.generated.h"

struct FlecsTransform
{
	FTransform Value;
};
struct FlecsISMIndex
{
	int Value;
};
struct FlecsIsmRef
{
	UInstancedStaticMeshComponent* Value;
};
struct FlecsCorn
{
	float Growth;
};
struct Corns {};

USTRUCT(BlueprintType)
struct FFlecsEntityHandle
{
	GENERATED_USTRUCT_BODY()
	FFlecsEntityHandle()  {}
	FFlecsEntityHandle(int inId)
	{
		FlecsEntityId = inId;
	}
	UPROPERTY(BlueprintReadWrite)
	int FlecsEntityId;
};

UCLASS()
class FLECSTEST_API UFlecsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	flecs::world* GetEcsWorld() const;
	
	UPROPERTY(EditAnywhere)
	UInstancedStaticMeshComponent* CornRenderer = nullptr;

	
	UFUNCTION(BlueprintCallable, Category="FLECS")
	void InitFlecs(UStaticMesh* InMesh);
	UFUNCTION(BlueprintCallable, Category="FLECS")
	FFlecsEntityHandle SpawnCornEntity(FVector location, FRotator rotation);
	UFUNCTION(BlueprintCallable, Category="FLECS")
	void SetEntityHighlight(FFlecsEntityHandle entityHandle, bool isHighlighted);
	UFUNCTION(BlueprintCallable, Category="FLECS")
	float GetEntityGrowthData(FFlecsEntityHandle entityHandle);

protected:
	FTickerDelegate OnTickDelegate;
	FTSTicker::FDelegateHandle OnTickHandle;
	flecs::world* ECSWorld = nullptr;
private:
	bool Tick(float DeltaTime);
	
};
