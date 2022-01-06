#include "pch.h"
#include "Mesh.h"

Mesh::Mesh(ID3D11Device* pDevice, const std::vector<IVertex>& vertices, const std::vector<unsigned int>& indices, const std::wstring& effectPath)
	: m_Effect(pDevice, effectPath)
{	
	// Create vertex layout
	HRESULT result = S_OK;
	static constexpr uint32_t numElements{ 4 };
	D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].AlignedByteOffset = 0;
	vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[1].SemanticName = "TEXCOORD";
	vertexDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	vertexDesc[1].AlignedByteOffset = 12;
	vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[2].SemanticName = "NORMAL";
	vertexDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[2].AlignedByteOffset = 20;
	vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[3].SemanticName = "TANGENT";
	vertexDesc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[3].AlignedByteOffset = 32;
	vertexDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	// Create vertex buffer
	D3D11_BUFFER_DESC bd{};
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(IVertex) * static_cast<uint32_t>(vertices.size());
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData{nullptr};
	initData.pSysMem = vertices.data();

	result = pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
	if (FAILED(result))
		return;

	// Create input layout
	D3DX11_PASS_DESC passDesc{};
	m_Effect.GetTechnique()->GetPassByIndex(0)->GetDesc(&passDesc);
	result = pDevice->CreateInputLayout(vertexDesc, numElements, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &m_pVertexLayout);
	if (FAILED(result))
		return;

	// Create index buffer
	m_AmountIndices = static_cast<uint32_t>(indices.size());
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(uint32_t) * m_AmountIndices;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	initData.pSysMem = indices.data();
	result = pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);
	if (FAILED(result))
		return;

	std::cout << "Mesh loaded successfully\n";
}

Mesh::~Mesh()
{
	m_pIndexBuffer->Release();
	m_pVertexLayout->Release();
	m_pVertexBuffer->Release();

	delete m_pDiffuse; // Todo: textureManager, if need be
	delete m_pNormalMap; // Todo: textureManager, if need be
	delete m_pSpecularMap; // Todo: textureManager, if need be
	delete m_pGlossinessMap; // Todo: textureManager, if need be
}

void Mesh::Render(ID3D11DeviceContext* pDeviceContext, Elite::Camera* pCamera)
{
	// Set vertex buffer
	UINT stride = sizeof(IVertex);
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	
	// Set index buffer
	pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set input layout
	pDeviceContext->IASetInputLayout(m_pVertexLayout);

	// Set primitive topology
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	// Set Matrices
	SetEffectMatrix("gWorldViewProj", m_pWorldViewProjVariable, pCamera->GetProjection() * pCamera->GetWorldToView());
	SetEffectMatrix("gWorldMatrix", m_pWorldMatrixVariable, Elite::FMatrix4::Identity()); // Todo: actually get a world matrix for a mesh
	SetEffectMatrix("gWorldMatrix", m_pViewInverseMatrixVariable, pCamera->GetViewToWorld());

	// Set Texture/Maps
	SetEffectShaderResource("gDiffuseMap", m_pDiffuseMapVariable, m_pDiffuse);
	SetEffectShaderResource("gNormalMap", m_pNormalMapVariable, m_pNormalMap);
	SetEffectShaderResource("gSpecularMap", m_pSpecularMapVariable, m_pSpecularMap);
	SetEffectShaderResource("gGlossinessMap", m_pGlossinessMapVariable, m_pGlossinessMap);


	// Render a triangle
	D3DX11_TECHNIQUE_DESC techDesc{}; // Todo: edited this
	m_Effect.GetTechnique()->GetDesc(&techDesc);
	for (UINT passIndex = 0; passIndex < techDesc.Passes; ++passIndex)
	{
		m_Effect.GetTechnique()->GetPassByIndex(0)->Apply(0, pDeviceContext);
		pDeviceContext->DrawIndexed(m_AmountIndices, 0, 0);
	}
}

void Mesh::SetDiffuseTexture(Texture* pDiffuseTexture)
{
	delete m_pDiffuse; // if it's not nullptr, destroy
	m_pDiffuse = pDiffuseTexture;
}
void Mesh::SetNormalMap(Texture* pNormalMap)
{
	delete m_pNormalMap; // if it's not nullptr, destroy
	m_pNormalMap = pNormalMap;
}
void Mesh::SetSpecularMap(Texture* pSpecularMap)
{
	delete m_pSpecularMap; // if it's not nullptr, destroy
	m_pSpecularMap = pSpecularMap;
}
void Mesh::SetGlossinessMap(Texture* pGlossinessMap)
{
	delete m_pGlossinessMap; // if it's not nullptr, destroy
	m_pGlossinessMap = pGlossinessMap;
}

Effect& Mesh::GetEffect()
{
	return m_Effect;
}

// Generalized functions for setting shader variables, don't make these const
void Mesh::SetEffectMatrix(const std::string& matrixVariableName, ID3DX11EffectMatrixVariable* pMeshMatrix,
	const Elite::FMatrix4& pMeshMatrixValue)
{
	// Get matrix
	pMeshMatrix = m_Effect.GetEffect()->GetVariableByName(matrixVariableName.c_str())->AsMatrix();
	if (!pMeshMatrix->IsValid())
	{
		std::cout << matrixVariableName << " is not valid\n";
		return;
	}

	// Set matrix if it's valid
	const HRESULT res = pMeshMatrix->SetMatrix(*pMeshMatrixValue.data);
	if (FAILED(res))
	{
		std::cout << "Issue setting " << matrixVariableName << std::endl;
		return;
	}
}
void Mesh::SetEffectShaderResource( const std::string& shaderVariableName, ID3DX11EffectShaderResourceVariable* pMeshShaderResource, 
	Texture* pMeshShaderResourceValue)
{
	// Get the shader resource  variable
	pMeshShaderResource = m_Effect.GetEffect()->GetVariableByName(shaderVariableName.c_str())->AsShaderResource();
	if (!pMeshShaderResource->IsValid())
	{
		std::cout << shaderVariableName << " is not valid\n";
		return;
	}

	// Set the diffuse variable
	if (pMeshShaderResource->IsValid() && pMeshShaderResourceValue != nullptr)
	{
		pMeshShaderResource->SetResource(pMeshShaderResourceValue->GetTextureResourceView());
	}
}
