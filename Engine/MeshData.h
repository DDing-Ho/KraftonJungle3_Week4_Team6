#pragma once
#include "CoreMinimal.h"
#include "StaticVertex.h"
#include "Component/PrimitiveComponent.h"
#include "Renderer/TextMeshBuilder.h"

enum class EMeshTopology
{
	EMT_Undefined = 0, // = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED
	EMT_Point = 1,	// =  D3D11_PRIMITIVE_TOPOLOGY_POINTLIST
	EMT_LineList = 2, // = D3D11_PRIMITIVE_TOPOLOGY_LINELIST
	EMT_LineStrip = 3,	// = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP
	EMT_TriangleList = 4, // = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	EMT_TriangleStrip = 5, // = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP
};

struct ENGINE_API FStaticMesh
{
	FStaticMesh() : SortId(NextSortId++) {}
	~FStaticMesh() { Release(); }

	uint32 GetSortId() const { return SortId; }
	bool bIsDirty = true;	// 최초 1회 초기화 보장


	int32 GetNumSection() const { return static_cast<int32>(Sections.size()); }

	bool UpdateVertexAndIndexBuffer(ID3D11Device* Device);
	bool CreateVertexAndIndexBuffer(ID3D11Device* Device);
	void Bind(ID3D11DeviceContext* Context);
	void Release();

	// 토폴로지 옵션
	EMeshTopology Topology = EMeshTopology::EMT_Undefined;

	// CPU 데이터
	TArray<FStaticVertex> Vertices;
	TArray<uint32> Indices;
	TArray<FMeshSection> Sections;

	// .obj파일 원래 경로
	FString PathFileName;

	// GPU 버퍼
	ID3D11Buffer* VertexBuffer = nullptr;
	ID3D11Buffer* IndexBuffer = nullptr;

	/** AABB Box Extent 및 Local Bound Radius 갱신 */
	void UpdateLocalBound();
	float GetLocalBoundRadius() const { return LocalBoundRadius; }

	FVector GetMinCoord() const { return MinCoord; }
	FVector GetMaxCoord() const { return MaxCoord; }
	FVector GetCenterCoord() const { return (MaxCoord - MinCoord) * 0.5 + MinCoord; }

private:
	uint32 SortId = 0;
	static inline uint32 NextSortId = 0;

	FVector MinCoord = FVector(FLT_MAX, FLT_MAX, FLT_MAX);
	FVector MaxCoord = FVector(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	float LocalBoundRadius = 0.f;
};

class ENGINE_API UStaticMesh : public UObject
{
public:
	DECLARE_RTTI(UStaticMesh, UObject)

	FBoxSphereBounds LocalBounds;

	const FString& GetAssetPathFileName() const;
	void SetStaticMeshAsset(FStaticMesh* InStaticMesh) { StaticMeshAsset = InStaticMesh; }
	FStaticMesh* GetRenderData() const { return StaticMeshAsset; }

	int32 GetNumSections() const { return StaticMeshAsset ? StaticMeshAsset->GetNumSection() : 0; }

private:
	FStaticMesh* StaticMeshAsset = nullptr;
};