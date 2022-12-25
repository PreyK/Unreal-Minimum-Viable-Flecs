#include "FlecsSubsystem.h"


flecs::world* UFlecsSubsystem::GetEcsWorld() const{return ECSWorld;}

void UFlecsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	OnTickDelegate = FTickerDelegate::CreateUObject(this, &UFlecsSubsystem::Tick);
	OnTickHandle = FTicker::GetCoreTicker().AddTicker(OnTickDelegate);
	ECSWorld = new flecs::world();
	
	//flecs explorer and monitor
	//comment this out if you not using it, it has a performance overhead
	GetEcsWorld()->import<flecs::monitor>();
	GetEcsWorld()->set<flecs::Rest>({});
	
	GetEcsWorld()->component<FlecsCorn>().member<float>("Current Growth");
	GetEcsWorld()->component<FlecsISMIndex>().member<int>("ISM Render index");
	
	UE_LOG(LogTemp, Warning, TEXT("UUnrealFlecsSubsystem has started!"));
	Super::Initialize(Collection);
}

void UFlecsSubsystem::InitFlecs(UStaticMesh* InMesh)
{
	FVector Location = FVector::ZeroVector;
	FRotator Rotation = FRotator::ZeroRotator;
	FActorSpawnParameters SpawnInfo;
	FlecsConstants.CornRenderer = Cast<AFlecsISMRenderer>(GetWorld()->SpawnActor(AFlecsISMRenderer::StaticClass(), &Location, &Rotation, SpawnInfo));
	FlecsConstants.CornRenderer->Initialize(InMesh);

	
	auto system_add_instance = GetEcsWorld()->system<FlecsISMAdd>()
	.iter([](flecs::iter it, FlecsISMAdd *add) {
		auto controller = FlecsConstants.CornRenderer;
		auto ecs = it.world();
		for (int i : it) {
			auto instanceIndex = controller->AddInstance();
			ecs.entity()
			.is_a(add->Prefab)
			.set<FlecsIsmRef>({controller})
			.set<FlecsISMIndex>({instanceIndex})
			.set<FlecsTransform>({add->Transform});
			it.entity(i).destruct();
		}
		FlecsConstants.CornRenderer->CreateOrExpandTransformArray();
	});
	auto system_copy_instance_transforms = GetEcsWorld()->system<FlecsTransform, FlecsISMIndex, FlecsIsmRef>()
	.iter([](flecs::iter it, FlecsTransform* ft, FlecsISMIndex* fi, FlecsIsmRef* fr) {
		for (int i : it) {
			auto index = fi[i].Value;
			FlecsConstants.CornRenderer->SetTransform(index, ft->Value);
		}
	});
	auto system_grow = GetEcsWorld()->system<FlecsCorn>()
	.iter([](flecs::iter it, FlecsCorn* fc) {
		float GrowthRate = 20*it.delta_time();
		for (int i : it) {
			fc[i].Growth+=(fc[i].Growth<100)*GrowthRate;
		}
	});
	auto system_copy_growth = GetEcsWorld()->system<FlecsCorn, FlecsISMIndex>()
	.iter([](flecs::iter it, FlecsCorn* fw, FlecsISMIndex* fi) {
		for (int i : it) {
			auto index = fi[i].Value;
			FlecsConstants.CornRenderer->SetCustomData(index, fw[i].Growth);
		}
	});

	//TODO: Fix prefab logic
	//https://ajmmertens.medium.com/deconstructing-flecs-prefabs-d604b5ba0fcc
	auto CornPrefab = GetEcsWorld()->prefab()
	.set<FlecsTransform>({FTransform(FVector::ZeroVector)})
	.set<FlecsCorn>({0});
	
	GetEcsWorld()->entity().set<FlecsISMAdd>({0, CornPrefab, FTransform(FVector::ZeroVector)});
	
	
	UE_LOG(LogTemp, Warning, TEXT("Flecs Corn system initialized444"));
}

void UFlecsSubsystem::Deinitialize()
{
	FTicker::GetCoreTicker().RemoveTicker(OnTickHandle);
	if(!ECSWorld) delete(ECSWorld);
	UE_LOG(LogTemp, Warning, TEXT("UUnrealFlecsSubsystem has shut down!"));
	Super::Deinitialize();
}

void UFlecsSubsystem::AddInstance(FVector location, FRotator rotation)
{
	auto CornPrefab = GetEcsWorld()->prefab()
	.set<FlecsTransform>({FTransform(rotation, location)})
	.set<FlecsCorn>({0});
	GetEcsWorld()->entity().set<FlecsISMAdd>({0, CornPrefab, FTransform(rotation, location)});
}
bool UFlecsSubsystem::Tick(float DeltaTime)
{
	FlecsConstants.CornRenderer->BatchUpdateTransform();
	
	if(ECSWorld) ECSWorld->progress(DeltaTime);
	return true;
}
