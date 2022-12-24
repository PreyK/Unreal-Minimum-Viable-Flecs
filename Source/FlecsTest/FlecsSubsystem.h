#pragma once
#include "flecs.h"
#include "CoreMinimal.h"
#include "FlecsISMRenderer.h"
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

struct FlecsISMAdd
{
	int32 Hash;
	flecs::entity Prefab;
	FTransform Transform;
};

struct FlecsIsmRef
{
	AFlecsISMRenderer* Value;
};

struct FlecsCorn
{
	float Growth;
};
static struct 
{  
	AFlecsISMRenderer* CornRenderer;
} FlecsConstants; 


UCLASS()
class FLECSTEST_API UFlecsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	flecs::world* GetEcsWorld() const;
	
	UFUNCTION(BlueprintCallable, Category="FLECS")
	void AddInstance(FVector location, FRotator rotation);

	
	UFUNCTION(BlueprintCallable, Category="FLECS")
	void InitFlecs(UStaticMesh* InMesh); 

protected:
	FTickerDelegate OnTickDelegate;
	FDelegateHandle OnTickHandle;
	flecs::world* ECSWorld = nullptr;
private:
	bool Tick(float DeltaTime);
	
};
