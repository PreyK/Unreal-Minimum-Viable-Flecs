#include "FlecsSubsystem.h"
flecs::world* UFlecsSubsystem::GetEcsWorld() const{return ECSWorld;}
void UFlecsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	OnTickDelegate = FTickerDelegate::CreateUObject(this, &UFlecsSubsystem::Tick);
	OnTickHandle = FTSTicker::GetCoreTicker().AddTicker(OnTickDelegate);
	
	//sets title in Flecs Explorer
  char name[] = { "Minimum Viable Flecs" };
	char* argv = name;
	ECSWorld = new flecs::world(1, &argv);
	
	//flecs explorer and monitor
	//comment this out if you not using it, it has some performance overhead
	//go to https://www.flecs.dev/explorer/ when the project is running to inspect active entities and values
	GetEcsWorld()->import<flecs::monitor>();
	GetEcsWorld()->set<flecs::Rest>({});
	
	//expose values with names to Flecs Explorer for easier inspection & debugging
	GetEcsWorld()->component<FlecsCorn>().member<float>("Current Growth");
	GetEcsWorld()->component<FlecsISMIndex>().member<int>("ISM Render index");	
	
	UE_LOG(LogTemp, Warning, TEXT("UUnrealFlecsSubsystem has started!"));
	Super::Initialize(Collection);
}

void UFlecsSubsystem::InitFlecs(UStaticMesh* InMesh)
{
	//Spawn an actor and add an Instanced Static Mesh component to it.
	//This will render our entities.
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	CornRenderer = Cast<UInstancedStaticMeshComponent>((GetWorld()->SpawnActor(AActor::StaticClass(), &FVector::ZeroVector, &FRotator::ZeroRotator, SpawnInfo))->AddComponentByClass(UInstancedStaticMeshComponent::StaticClass(), false, FTransform(FVector::ZeroVector), false));
	CornRenderer->SetStaticMesh(InMesh);
	CornRenderer->bUseDefaultCollision = false;
	CornRenderer->SetGenerateOverlapEvents(false);
	CornRenderer->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CornRenderer->SetCanEverAffectNavigation(false);
	CornRenderer->NumCustomDataFloats = 2;
	
	//this system processes the growth of our entities
	auto system_grow = GetEcsWorld()->system<FlecsCorn>("Grow System")
	.iter([](flecs::iter it, FlecsCorn* fc) {
		float GrowthRate = 20*it.delta_time();
		for (int i : it) {
			//if we haven't grown fully (100) then grow
			fc[i].Growth+=(fc[i].Growth<100)*GrowthRate;
		}
	});
	
	//this system sets the growth value of our entities in ISM so we can access it from materials.
	auto system_copy_growth = GetEcsWorld()->system<FlecsCorn, FlecsISMIndex, FlecsIsmRef>("Grow Renderer System")
	.iter([](flecs::iter it, FlecsCorn* fw, FlecsISMIndex* fi, FlecsIsmRef* fr) {
		for (int i : it) {
			auto index = fi[i].Value;
			fr[i].Value->SetCustomDataValue(index, 0, fw[i].Growth, true);
		}
	});
	
	UE_LOG(LogTemp, Warning, TEXT("Flecs Corn system initialized!"));
}

void UFlecsSubsystem::Deinitialize()
{
	FTSTicker::GetCoreTicker().RemoveTicker(OnTickHandle);
	
	if (ECSWorld)
	{
		delete ECSWorld;
		ECSWorld = nullptr;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("UUnrealFlecsSubsystem has shut down!"));
	Super::Deinitialize();
}

FFlecsEntityHandle UFlecsSubsystem::SpawnCornEntity(FVector location, FRotator rotation)
{
	auto IsmID = CornRenderer->AddInstance(FTransform(rotation, location));
	auto entity = GetEcsWorld()->entity()
	.set<FlecsIsmRef>({CornRenderer})
	.set<FlecsISMIndex>({IsmID})
	.set<FlecsCorn>({0})
	.child_of<Corns>()
	.set_name(StringCast<ANSICHAR>(*FString::Printf(TEXT("Corn%d"), IsmID)).Get());
	return FFlecsEntityHandle{int(entity.id())};
}

void UFlecsSubsystem::SetEntityHighlight(FFlecsEntityHandle entityHandle, bool isHighlighted)
{
	int idx = GetEcsWorld()->entity(entityHandle.FlecsEntityId).get<FlecsISMIndex>()->Value;
	CornRenderer->SetCustomDataValue(idx, 1, (float)isHighlighted, true);
}
float UFlecsSubsystem::GetEntityGrowthData(FFlecsEntityHandle entityHandle)
{
	return GetEcsWorld()->entity(entityHandle.FlecsEntityId).get<FlecsCorn>()->Growth;
}

bool UFlecsSubsystem::Tick(float DeltaTime)
{
	if(ECSWorld) ECSWorld->progress(DeltaTime);
	return true;
}
