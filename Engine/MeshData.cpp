#include "MeshData.h"

bool FStaticMesh::UpdateVertexAndIndexBuffer(ID3D11Device* Device)
{
	if (!bIsDirty)
		return true;

	return CreateVertexAndIndexBuffer(Device);
}

bool FStaticMesh::CreateVertexAndIndexBuffer(ID3D11Device* Device)
{
	if (VertexBuffer)
	{
		VertexBuffer->Release();
		VertexBuffer = nullptr;
	}
	if (IndexBuffer)
	{
		IndexBuffer->Release();
		IndexBuffer = nullptr;
	}

	if (Vertices.empty() || Indices.empty())
	{
		return false;
	}

	// Vertex Buffer
	D3D11_BUFFER_DESC VBDesc = {};
	VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	VBDesc.ByteWidth = static_cast<UINT>(sizeof(FStaticVertex) * Vertices.size());
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA VBData = {};
	VBData.pSysMem = Vertices.data();

	HRESULT Hr = Device->CreateBuffer(&VBDesc, &VBData, &VertexBuffer);
	if (FAILED(Hr))
	{
		printf("[FMeshData] Failed to create vertex buffer\n");
		return false;
	}

	// Index Buffer
	D3D11_BUFFER_DESC IBDesc = {};
	IBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	IBDesc.ByteWidth = static_cast<UINT>(sizeof(uint32) * Indices.size());
	IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA IBData = {};
	IBData.pSysMem = Indices.data();

	Hr = Device->CreateBuffer(&IBDesc, &IBData, &IndexBuffer);
	if (FAILED(Hr))
	{
		printf("[FMeshData] Failed to create index buffer\n");
		VertexBuffer->Release();
		VertexBuffer = nullptr;
		return false;
	}

	return true;
}

void FStaticMesh::Bind(ID3D11DeviceContext* Context)
{
	UINT Stride = sizeof(FStaticVertex);
	UINT Offset = 0;
	Context->IASetVertexBuffers(0, 1, &VertexBuffer, &Stride, &Offset);
	Context->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
}

void FStaticMesh::Release()
{
	if (IndexBuffer)
	{
		IndexBuffer->Release();
		IndexBuffer = nullptr;
	}
	if (VertexBuffer)
	{
		VertexBuffer->Release();
		VertexBuffer = nullptr;
	}
}

void FStaticMesh::UpdateLocalBound()
{
	if (Vertices.empty())
	{
		MinCoord = FVector(FLT_MAX, FLT_MAX, FLT_MAX);
		MaxCoord = FVector(-FLT_MAX, -FLT_MAX, -FLT_MAX);
		LocalBoundRadius = 0.f;
	}
	else
	{
		// TODO: Ritter's Algorithm으로 개선
		for (const FStaticVertex& Vertex : Vertices)
		{
			if (Vertex.Position.X < MinCoord.X) MinCoord.X = Vertex.Position.X;
			if (Vertex.Position.X > MaxCoord.X) MaxCoord.X = Vertex.Position.X;
			if (Vertex.Position.Y < MinCoord.Y) MinCoord.Y = Vertex.Position.Y;
			if (Vertex.Position.Y > MaxCoord.Y) MaxCoord.Y = Vertex.Position.Y;
			if (Vertex.Position.Z < MinCoord.Z) MinCoord.Z = Vertex.Position.Z;
			if (Vertex.Position.Z > MaxCoord.Z) MaxCoord.Z = Vertex.Position.Z;

			LocalBoundRadius = ((MaxCoord - MinCoord) * 0.5).Size();
		}
	}
}

const FString& UStaticMesh::GetAssetPathFileName() const
{
	if (StaticMeshAsset) return StaticMeshAsset->PathFileName;
	static FString EmptyPath = "";
	return EmptyPath;
}
