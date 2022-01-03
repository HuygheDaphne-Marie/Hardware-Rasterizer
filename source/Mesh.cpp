#include "pch.h"
#include "Mesh.h"

Mesh::Mesh(ID3D11Device* pDevice, const std::vector<IVertex>& vertices, const std::vector<unsigned int>& indices, const std::wstring& effectPath)
	: m_Effect(pDevice, effectPath)
{	
	// Create vertex layout
	HRESULT result = S_OK;
	static constexpr uint32_t numElements{ 3 };
	D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].AlignedByteOffset = 0;
	vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[1].SemanticName = "TEXCOORD";
	vertexDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	vertexDesc[1].AlignedByteOffset = 12;
	vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[2].SemanticName = "COLOR";
	vertexDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[2].AlignedByteOffset = 20;
	vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

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

	// Matrix
	{
		// Set WorldViewProjection matrix
		m_pMatWorldViewProjVariable = m_Effect.GetEffect()->GetVariableByName("gWorldViewProj")->AsMatrix();
		if (!m_pMatWorldViewProjVariable->IsValid())
		{
			std::cout << "m_pMatWorldViewProjVariable is not valid\n";
			return;
		}

		// Set matrix if it's valid
		const Elite::FMatrix4 worldViewProjection{ pCamera->GetProjection() * pCamera->GetWorldToView() };
		const HRESULT res = m_pMatWorldViewProjVariable->SetMatrix(*worldViewProjection.data);
		if (FAILED(res))
		{
			std::cout << "Issue setting matrix\n";
			return;
		}
	}

	// Diffuse
	{
		// Get the shader diffuse map variable
		m_pDiffuseMapVariable = m_Effect.GetEffect()->GetVariableByName("gDiffuseMap")->AsShaderResource();
		if (!m_pDiffuseMapVariable->IsValid())
		{
			std::cout << "m_pDiffuseMapVariable is not valid\n";
			return;
		}

		// Set the diffuse variable
		if (m_pDiffuseMapVariable->IsValid() && m_pDiffuse != nullptr)
		{
			m_pDiffuseMapVariable->SetResource(m_pDiffuse->GetTextureResourceView());
		}
	}


	// Render a triangle
	D3DX11_TECHNIQUE_DESC techDesc;
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

Effect& Mesh::GetEffect()
{
	return m_Effect;
}
