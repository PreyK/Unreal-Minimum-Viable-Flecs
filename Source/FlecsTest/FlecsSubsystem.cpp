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
	GetEcsWorld()->import<flecs::stats>();
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
	GetEcsWorld()->system<FlecsCorn>("Grow System")
		.run([](flecs::iter it) {
		while (it.next())
		{
			auto fc = it.field<FlecsCorn>(0);

			float GrowthRate = 20 * it.delta_time();
			for (int i : it) {
				//if we haven't grown fully (100) then grow
				fc[i].Growth += (fc[i].Growth < 100) * GrowthRate;
			}
		}
			});

	
	//this system sets the growth value of our entities in ISM so we can access it from materials.
	GetEcsWorld()->system<FlecsCorn, FlecsISMIndex, FlecsIsmRef>("Grow Renderer System")
	.run([](flecs::iter it)-> void {
		while(it.next())
		{
			auto fw = it.field<FlecsCorn>(0);
			auto fi = it.field<FlecsISMIndex>(1);
			auto fr = it.field<FlecsIsmRef>(2);
			for (int i : it) {
				auto index = fi[i].Value;
				fr[i].Value->SetCustomDataValue(index, 0, fw[i].Growth, true);
			}
			
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
	UE_LOG(LogTemp, Warning, TEXT("Corn Spawned!"));
	return FFlecsEntityHandle{int(entity.id())};
}

void UFlecsSubsystem::SetEntityHighlight(FFlecsEntityHandle entityHandle, bool isHighlighted)
{
	int idx = GetEcsWorld()->entity(entityHandle.FlecsEntityId).get<FlecsISMIndex>().Value;
	CornRenderer->SetCustomDataValue(idx, 1, (float)isHighlighted, true);
}
float UFlecsSubsystem::GetEntityGrowthData(FFlecsEntityHandle entityHandle)
{
	return GetEcsWorld()->entity(entityHandle.FlecsEntityId).get<FlecsCorn>().Growth;
}

bool UFlecsSubsystem::Tick(float DeltaTime)
{
	if(ECSWorld) ECSWorld->progress(DeltaTime);
	return true;
}

TArray<int64> UFlecsSubsystem::ExecuteQuery(const FString& QueryString)
{
	TArray<int64> EntityIDs;

	if (!GetEcsWorld())
	{
		UE_LOG(LogTemp, Error, TEXT("Flecs world not initialized!"));
		return EntityIDs;
	}

	// Create a new query
	ecs_query_desc_t QueryDesc = {};
	QueryDesc.expr = TCHAR_TO_UTF8(*QueryString); // Define the query string

	ecs_query_t* Query = ecs_query_init(GetEcsWorld()->c_ptr(), &QueryDesc);

	// Execute the query and collect entity IDs
	ecs_iter_t It = ecs_query_iter(GetEcsWorld()->c_ptr(), Query);
	while (ecs_query_next(&It))
	{
		for (int32 i = 0; i < It.count; i++)
		{
			ecs_entity_t Entity = It.entities[i];
			EntityIDs.Add(static_cast<int64>(Entity));
		}
	}

	FString log = TEXT("Failed to create query with expr.") + FString::FromInt(It.count);
	UE_LOG(LogTemp, Log, TEXT("%s"), *log);

	return EntityIDs;
}