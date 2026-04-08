#include "BillboardComponent.h"
#include "PrimitiveComponent.h"
#include "Renderer/MeshData.h"
#include "Renderer/Material.h"
#include "Renderer/MaterialManager.h"
#include "Object/Object.h"
#include "Object/Class.h"

IMPLEMENT_RTTI(UBillboardComponent, UPrimitiveComponent)

void UBillboardComponent::PostConstruct()
{
	bDrawDebugBounds = false;
	bBillboard = true;
	BillboardMesh = std::make_shared<FDynamicMesh>();

	MaterialInstance = FMaterialManager::Get().FindByName("M_Default_Texture")->CreateDynamicMaterial();
}

FBoxSphereBounds UBillboardComponent::GetWorldBounds() const
{
	const FVector Center = GetWorldLocation();
	const FVector WorldScale = GetWorldTransform().GetScaleVector();

	const float HalfW = Size.X * 0.5f * WorldScale.X;
	const float HalfH = Size.Y * 0.5f * WorldScale.Y;
	const float HalfZ = ((HalfW > HalfH) ? HalfW : HalfH);

	const FVector BoxExtent(HalfW, HalfH, HalfZ);
	return { Center, BoxExtent.Size(), BoxExtent };
}

FRenderMesh* UBillboardComponent::GetRenderMesh() const
{
	return BillboardMesh.get();
}

void UBillboardComponent::SetSpriteTexture(std::shared_ptr<FMaterialTexture> InTexture)
{
	if (MaterialInstance)
	{
		MaterialInstance->SetMaterialTexture(InTexture);
	}
}

void UBillboardComponent::CopyPropertiesFrom(const UObject* Source)
{
	UPrimitiveComponent::CopyPropertiesFrom(Source);
	const UBillboardComponent* SourceComp = static_cast<const UBillboardComponent*>(Source);

	this->Size = SourceComp->Size;
	this->Color = SourceComp->Color;
	this->bBillboard = SourceComp->bBillboard;

	this->BillboardMesh = SourceComp->BillboardMesh;
	this->MaterialInstance = SourceComp->MaterialInstance;
}
