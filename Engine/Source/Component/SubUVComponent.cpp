#include "SubUVComponent.h"
#include "Object/Class.h"
#include "Renderer/MeshData.h"

IMPLEMENT_RTTI(USubUVComponent, UPrimitiveComponent)

void USubUVComponent::PostConstruct()
{
	// SubUV 렌더링용 메시 객체 생성
	bDrawDebugBounds = false;
	SubUVMesh = std::make_shared<FDynamicMesh>();
}

FRenderMesh* USubUVComponent::GetRenderMesh() const { return SubUVMesh.get(); }

void USubUVComponent::CopyPropertiesFrom(const UObject* Source)
{
	UPrimitiveComponent::CopyPropertiesFrom(Source);
	const USubUVComponent* SourceComp = static_cast<const USubUVComponent*>(Source);

	this->Size = SourceComp->Size;
	this->Color = SourceComp->Color;
	this->Columns = SourceComp->Columns;
	this->Rows = SourceComp->Rows;
	this->TotalFrames = SourceComp->TotalFrames;
	this->FirstFrame = SourceComp->FirstFrame;
	this->LastFrame = SourceComp->LastFrame;
	this->FPS = SourceComp->FPS;
	this->bLoop = SourceComp->bLoop;
	this->bBillboard = SourceComp->bBillboard;

	this->SubUVMesh = SourceComp->SubUVMesh;
}

FBoxSphereBounds USubUVComponent::GetWorldBounds() const
{
	const FVector Center = GetWorldLocation();
	const FVector WorldScale = GetWorldTransform().GetScaleVector();

	const float HalfW = Size.X * 0.5f * WorldScale.X;
	const float HalfH = Size.Y * 0.5f * WorldScale.Y;
	const float HalfZ = ((HalfW > HalfH) ? HalfW : HalfH);

	const FVector BoxExtent(HalfW, HalfH, HalfZ);
	return { Center, BoxExtent.Size(), BoxExtent };
}
