#include "FlecsISMRenderer.h"
void AFlecsISMRenderer::BeginPlay(){Super::BeginPlay();}
void AFlecsISMRenderer::Tick(float DeltaTime){Super::Tick(DeltaTime);}
int32 AFlecsISMRenderer::GetInstanceCount() const {return InstancedStaticMeshComponent->GetInstanceCount();}
int32 AFlecsISMRenderer::AddInstance(){return AddInstance( FTransform(FVector::ZeroVector));}
void AFlecsISMRenderer::SetTransform(int32 instanceIndex, const FTransform& transform){transforms[instanceIndex] = transform;}


AFlecsISMRenderer::AFlecsISMRenderer()
{
	PrimaryActorTick.bCanEverTick = false;
	InstancedStaticMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstancedStaticMeshComponent"));
	SetRootComponent(InstancedStaticMeshComponent);
}

void AFlecsISMRenderer::Initialize(UStaticMesh* InMesh) const
{
	InstancedStaticMeshComponent->SetStaticMesh(InMesh);
	
	InstancedStaticMeshComponent->bUseDefaultCollision = false;
	InstancedStaticMeshComponent->SetGenerateOverlapEvents(false);
	InstancedStaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	InstancedStaticMeshComponent->SetCanEverAffectNavigation(false);
	InstancedStaticMeshComponent->NumCustomDataFloats = 1;
}

int32 AFlecsISMRenderer::AddInstance(FTransform inTransform)
{
	int32 instanceIndex;
	if(indexPool.IsEmpty())
	{
		instanceIndex = InstancedStaticMeshComponent->AddInstance(inTransform);
	}
	else
	{
		indexPool.Dequeue(instanceIndex);
		InstancedStaticMeshComponent->SetCustomDataValue(instanceIndex, 0, 0);
	}
	return instanceIndex;
}
void AFlecsISMRenderer::RemoveInstance(int32 instanceIndex)
{
	indexPool.Enqueue(instanceIndex);
	
	//Just hide an instance without actual removing
	//Soon newly added instance replace the hidden one
	InstancedStaticMeshComponent->SetCustomDataValue(instanceIndex, 0, 1);
}
void AFlecsISMRenderer::SetCustomData(int32 instanceIndex, float data) const
{
	//indexPool.Enqueue(instanceIndex);
	
	//Just hide an instance without actual removing
	//Soon newly added instance replace the hidden one
	InstancedStaticMeshComponent->SetCustomDataValue(instanceIndex, 0, data, true);
	
}
void AFlecsISMRenderer::CreateOrExpandTransformArray()
{
	if(GetInstanceCount() != transforms.Num())
	{
		transforms.AddUninitialized(GetInstanceCount() - transforms.Num());
		for (auto i = 0; i < transforms.Num(); i++)
		{
			transforms[i] = FTransform{FVector::ZeroVector};
		}
	}
}
void AFlecsISMRenderer::BatchUpdateTransform() const
{
	
	if(transforms.Num() > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Batch updating transforms!"));
		InstancedStaticMeshComponent->BatchUpdateInstancesTransforms(0, transforms, true, true);
	}
}




