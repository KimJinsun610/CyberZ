//-----------------------------------------------------------------------------
// File: CGameObject.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Object.h"
#include "Shader.h"
#include "Scene.h"
//#include "Animation.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CTexture::CTexture(int nTextures, UINT nTextureType, int nSamplers, int nRootParameters)
{
	m_nTextureType = nTextureType;

	m_nTextures = nTextures;
	if (m_nTextures > 0)
	{
		m_ppd3dTextures = new ID3D12Resource * [m_nTextures];
		for (int i = 0; i < m_nTextures; i++)m_ppd3dTextures[i] = NULL;

		m_ppd3dTextureUploadBuffers = new ID3D12Resource * [m_nTextures];
		for (int i = 0; i < m_nTextures; i++)m_ppd3dTextureUploadBuffers[i] = NULL;

		m_pd3dSrvGpuDescriptorHandles = new D3D12_GPU_DESCRIPTOR_HANDLE[m_nTextures];
		for (int i = 0; i < m_nTextures; i++)m_pd3dSrvGpuDescriptorHandles[i].ptr = NULL;

		m_pnResourceTypes = new UINT[m_nTextures];
		for (int i = 0; i < m_nTextures; i++)m_pnResourceTypes[i] = 0;

		m_pdxgiBufferFormats = new DXGI_FORMAT[m_nTextures];
		for (int i = 0; i < m_nTextures; i++)m_pnResourceTypes[i] = DXGI_FORMAT_UNKNOWN;
		m_pnBufferElements = new int[m_nTextures];
		for (int i = 0; i < m_nTextures; i++)m_pnBufferElements[i] = 0;
	}
	m_nRootParameters = nRootParameters;
	if (nRootParameters > 0)m_pnRootParameterIndices = new int[nRootParameters];
	for (int i = 0; i < m_nRootParameters; i++)m_pnRootParameterIndices[i] = -1;

	m_nSamplers = nSamplers;
	if (m_nSamplers > 0)m_pd3dSamplerGpuDescriptorHandles = new D3D12_GPU_DESCRIPTOR_HANDLE[m_nSamplers];
}

CTexture::~CTexture()
{
	if (m_ppd3dTextures)
	{
		for (int i = 0; i < m_nTextures; i++) if (m_ppd3dTextures[i]) m_ppd3dTextures[i]->Release();
		delete[] m_ppd3dTextures;
	}
	if (m_pnResourceTypes) delete[] m_pnResourceTypes;
	if (m_pdxgiBufferFormats) delete[] m_pdxgiBufferFormats;
	if (m_pnBufferElements) delete[] m_pnBufferElements;

	if (m_pnRootParameterIndices) delete[] m_pnRootParameterIndices;
	if (m_pd3dSrvGpuDescriptorHandles) delete[] m_pd3dSrvGpuDescriptorHandles;

	if (m_pd3dSamplerGpuDescriptorHandles) delete[] m_pd3dSamplerGpuDescriptorHandles;
}

void CTexture::SetRootParameterIndex(int nIndex, UINT nRootParameterIndex)
{
	m_pnRootParameterIndices[nIndex] = nRootParameterIndex;
}

void CTexture::SetGpuDescriptorHandle(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGpuDescriptorHandle)
{
	m_pd3dSrvGpuDescriptorHandles[nIndex] = d3dSrvGpuDescriptorHandle;
}

void CTexture::SetSampler(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSamplerGpuDescriptorHandle)
{
	m_pd3dSamplerGpuDescriptorHandles[nIndex] = d3dSamplerGpuDescriptorHandle;
}

void CTexture::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_nRootParameters == m_nTextures)
	{
		for (int i = 0; i < m_nRootParameters; i++)
		{
			pd3dCommandList->SetGraphicsRootDescriptorTable(m_pnRootParameterIndices[i], m_pd3dSrvGpuDescriptorHandles[i]);
		}
	}
	else
	{
		pd3dCommandList->SetGraphicsRootDescriptorTable(m_pnRootParameterIndices[0], m_pd3dSrvGpuDescriptorHandles[0]);
	}
}

void CTexture::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, int nParameterIndex, int nTextureIndex)
{
	pd3dCommandList->SetGraphicsRootDescriptorTable(m_pnRootParameterIndices[nParameterIndex], m_pd3dSrvGpuDescriptorHandles[nTextureIndex]);
}

void CTexture::ReleaseShaderVariables()
{
}

void CTexture::ReleaseUploadBuffers()
{
	if (m_ppd3dTextureUploadBuffers)
	{
		for (int i = 0; i < m_nTextures; i++) if (m_ppd3dTextureUploadBuffers[i]) m_ppd3dTextureUploadBuffers[i]->Release();
		delete[] m_ppd3dTextureUploadBuffers;
		m_ppd3dTextureUploadBuffers = NULL;
	}
}

//void CTexture::LoadTextureFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, wchar_t* pszFileName, UINT nIndex)
//{
//	m_ppd3dTextures[nIndex] = ::CreateTextureResourceFromDDSFile(pd3dDevice, pd3dCommandList, pszFileName, &m_ppd3dTextureUploadBuffers[nIndex], D3D12_RESOURCE_STATE_GENERIC_READ);
//}

void CTexture::LoadTextureFromDDSFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, wchar_t* pszFileName, UINT nResourceType, UINT nIndex)
{
	m_pnResourceTypes[nIndex] = nResourceType;
	m_ppd3dTextures[nIndex] = ::CreateTextureResourceFromDDSFile(pd3dDevice, pd3dCommandList, pszFileName, &m_ppd3dTextureUploadBuffers[nIndex], D3D12_RESOURCE_STATE_GENERIC_READ/*D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE*/);
}

void CTexture::LoadBuffer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pData, UINT nElements, UINT nStride, DXGI_FORMAT ndxgiFormat, UINT nIndex)
{
	m_pnResourceTypes[nIndex] = RESOURCE_BUFFER;
	m_pdxgiBufferFormats[nIndex] = ndxgiFormat;
	m_pnBufferElements[nIndex] = nElements;
	m_ppd3dTextures[nIndex] = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pData, nElements * nStride, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_GENERIC_READ, &m_ppd3dTextureUploadBuffers[nIndex]);
}

ID3D12Resource* CTexture::CreateTexture(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, UINT nIndex, UINT nResourceType, UINT nWidth, UINT nHeight, UINT nElements, UINT nMipLevels, DXGI_FORMAT dxgiFormat, D3D12_RESOURCE_FLAGS d3dResourceFlags, D3D12_RESOURCE_STATES d3dResourceStates, D3D12_CLEAR_VALUE* pd3dClearValue)
{
	m_pnResourceTypes[nIndex] = nResourceType;
	m_ppd3dTextures[nIndex] = ::CreateTexture2DResource(pd3dDevice, pd3dCommandList, nWidth, nHeight, nElements, nMipLevels, dxgiFormat, d3dResourceFlags, d3dResourceStates, pd3dClearValue);
	return(m_ppd3dTextures[nIndex]);
}

D3D12_SHADER_RESOURCE_VIEW_DESC CTexture::GetShaderResourceViewDesc(int nIndex)
{
	ID3D12Resource* pShaderResource = GetResource(nIndex);
	D3D12_RESOURCE_DESC d3dResourceDesc = pShaderResource->GetDesc();

	D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc;
	d3dShaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	int nTextureType = GetTextureType(nIndex);
	switch (nTextureType)
	{
	case RESOURCE_TEXTURE2D: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 1)
	case RESOURCE_TEXTURE2D_ARRAY: //[]
		d3dShaderResourceViewDesc.Format = d3dResourceDesc.Format;
		if (d3dResourceDesc.Format == DXGI_FORMAT_D32_FLOAT) d3dShaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		d3dShaderResourceViewDesc.Texture2D.MipLevels = -1;
		d3dShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.Texture2D.PlaneSlice = 0;
		d3dShaderResourceViewDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		break;
	case RESOURCE_TEXTURE2DARRAY: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize != 1)
		d3dShaderResourceViewDesc.Format = d3dResourceDesc.Format;
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
		d3dShaderResourceViewDesc.Texture2DArray.MipLevels = -1;
		d3dShaderResourceViewDesc.Texture2DArray.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.Texture2DArray.PlaneSlice = 0;
		d3dShaderResourceViewDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;
		d3dShaderResourceViewDesc.Texture2DArray.FirstArraySlice = 0;
		d3dShaderResourceViewDesc.Texture2DArray.ArraySize = d3dResourceDesc.DepthOrArraySize;
		break;
	case RESOURCE_TEXTURE_CUBE: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 6)
		d3dShaderResourceViewDesc.Format = d3dResourceDesc.Format;
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		d3dShaderResourceViewDesc.TextureCube.MipLevels = -1;
		d3dShaderResourceViewDesc.TextureCube.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.TextureCube.ResourceMinLODClamp = 0.0f;
		break;
	case RESOURCE_BUFFER: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
		d3dShaderResourceViewDesc.Format = m_pdxgiBufferFormats[nIndex];
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		d3dShaderResourceViewDesc.Buffer.FirstElement = 0;
		d3dShaderResourceViewDesc.Buffer.NumElements = m_pnBufferElements[nIndex];
		d3dShaderResourceViewDesc.Buffer.StructureByteStride = 0;
		d3dShaderResourceViewDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		break;
	}
	return(d3dShaderResourceViewDesc);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CMaterial::CMaterial(int nTextures)
{
	m_nTextures = nTextures;

	m_ppTextures = new CTexture * [m_nTextures];
	m_ppstrTextureNames = new _TCHAR[m_nTextures][64];
	for (int i = 0; i < m_nTextures; i++) m_ppTextures[i] = NULL;
	for (int i = 0; i < m_nTextures; i++) m_ppstrTextureNames[i][0] = '\0';
}

CMaterial::~CMaterial()
{
	if (m_pShader) m_pShader->Release();

	if (m_nTextures > 0)
	{
		for (int i = 0; i < m_nTextures; i++) if (m_ppTextures[i]) m_ppTextures[i]->Release();
		delete[] m_ppTextures;

		if (m_ppstrTextureNames) delete[] m_ppstrTextureNames;
	}
}

void CMaterial::SetShader(CShader* pShader)
{
	if (m_pShader) m_pShader->Release();
	m_pShader = pShader;
	if (m_pShader) m_pShader->AddRef();
}

void CMaterial::SetTexture(CTexture* pTexture, UINT nTexture)
{
	if (m_ppTextures[nTexture]) m_ppTextures[nTexture]->Release();
	m_ppTextures[nTexture] = pTexture;
	if (m_ppTextures[nTexture]) m_ppTextures[nTexture]->AddRef();
}

void CMaterial::ReleaseUploadBuffers()
{
	for (int i = 0; i < m_nTextures; i++)
	{
		if (m_ppTextures[i]) m_ppTextures[i]->ReleaseUploadBuffers();
	}
}

CShader* CMaterial::m_pSkinnedAnimationShader = NULL;
CShader* CMaterial::m_pStandardShader = NULL;
CShader* CMaterial::m_pBoundingBoxShader = NULL;


void CMaterial::PrepareShaders(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	DXGI_FORMAT pdxgiRtvFormats[5] = { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R16G16B16A16_UNORM, DXGI_FORMAT_R16G16B16A16_UNORM, DXGI_FORMAT_R16G16B16A16_UNORM, DXGI_FORMAT_R32_FLOAT };


	m_pStandardShader = new CStandardShader();
	m_pStandardShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 5, pdxgiRtvFormats, DXGI_FORMAT_R32_FLOAT);
	m_pStandardShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	m_pSkinnedAnimationShader = new CSkinnedAnimationStandardShader();
	m_pSkinnedAnimationShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 5, pdxgiRtvFormats, DXGI_FORMAT_R32_FLOAT);
	m_pSkinnedAnimationShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	m_pBoundingBoxShader = new CBoundingBoxShader();
	m_pBoundingBoxShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_R32_FLOAT);

}

void CMaterial::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 4, &m_xmf4AmbientColor, 16);
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 4, &m_xmf4AlbedoColor, 20);
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 4, &m_xmf4SpecularColor, 24);
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 4, &m_xmf4EmissiveColor, 28);

	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 1, &m_nType, 32);

	for (int i = 0; i < m_nTextures; i++)
	{
		if (m_ppTextures[i]) m_ppTextures[i]->UpdateShaderVariables(pd3dCommandList);
		//if (m_ppTextures[i]) m_ppTextures[i]->UpdateShaderVariable(pd3dCommandList, 0, 0);
	}
}

void CMaterial::LoadTextureFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, UINT nType, UINT nRootParameter, _TCHAR* pwstrTextureName, CTexture** ppTexture, CGameObject* pParent, FILE* pInFile, CShader* pShader)
{
	char pstrTextureName[64] = { '\0' };

	BYTE nStrLength = 64;
	UINT nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
	nReads = (UINT)::fread(pstrTextureName, sizeof(char), nStrLength, pInFile);
	pstrTextureName[nStrLength] = '\0';

	bool bDuplicated = false;
	if (strcmp(pstrTextureName, "null"))
	{
		SetMaterialType(nType);

		char pstrFilePath[64] = { '\0' };
		strcpy_s(pstrFilePath, 64, "Model/Textures/");

		bDuplicated = (pstrTextureName[0] == '@');
		strcpy_s(pstrFilePath + 15, 64 - 15, (bDuplicated) ? (pstrTextureName + 1) : pstrTextureName);
		strcpy_s(pstrFilePath + 15 + ((bDuplicated) ? (nStrLength - 1) : nStrLength), 64 - 15 - ((bDuplicated) ? (nStrLength - 1) : nStrLength), ".dds");

		size_t nConverted = 0;
		mbstowcs_s(&nConverted, pwstrTextureName, 64, pstrFilePath, _TRUNCATE);

		//#define _WITH_DISPLAY_TEXTURE_NAME

#ifdef _WITH_DISPLAY_TEXTURE_NAME
		static int nTextures = 0, nRepeatedTextures = 0;
		TCHAR pstrDebug[256] = { 0 };
		_stprintf_s(pstrDebug, 256, _T("Texture Name: %d %c %s\n"), (pstrTextureName[0] == '@') ? nRepeatedTextures++ : nTextures++, (pstrTextureName[0] == '@') ? '@' : ' ', pwstrTextureName);
		OutputDebugString(pstrDebug);
#endif
		if (!bDuplicated)
		{
			*ppTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
			(*ppTexture)->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, pwstrTextureName, RESOURCE_TEXTURE2D, 0);
			if (*ppTexture) (*ppTexture)->AddRef();

			CScene::CreateShaderResourceViews(pd3dDevice, *ppTexture, 0, nRootParameter);
		}
		else
		{
			if (pParent)
			{
				while (pParent)
				{
					if (!pParent->m_pParent) break;
					pParent = pParent->m_pParent;
				}
				CGameObject* pRootGameObject = pParent;
				*ppTexture = pRootGameObject->FindReplicatedTexture(pwstrTextureName);
				if (*ppTexture) (*ppTexture)->AddRef();
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CGameObject::CGameObject()
{
	m_xmf4x4ToParent = Matrix4x4::Identity();
	m_xmf4x4World = Matrix4x4::Identity();
}

CGameObject::CGameObject(int nMaterials) : CGameObject()
{
	m_nMaterials = nMaterials;
	if (m_nMaterials > 0)
	{
		m_ppMaterials = new CMaterial * [m_nMaterials];
		for (int i = 0; i < m_nMaterials; i++) m_ppMaterials[i] = NULL;
	}
}

CGameObject::~CGameObject()
{
	if (m_pMesh) m_pMesh->Release();

	if (m_nMaterials > 0&&m_ppMaterials)
	{
		for (int i = 0; i < m_nMaterials; i++)
		{
			if (m_ppMaterials[i]) m_ppMaterials[i]->Release();
		}
	}
	if (m_ppMaterials) delete[] m_ppMaterials;

	if (m_pSkinnedAnimationController) delete m_pSkinnedAnimationController;
}

void CGameObject::AddRef()
{
	m_nReferences++;

	if (m_pSibling) m_pSibling->AddRef();
	if (m_pChild) m_pChild->AddRef();
}

void CGameObject::Release()
{
	if (m_pChild) m_pChild->Release();
	if (m_pSibling) m_pSibling->Release();

	if (--m_nReferences <= 0) delete this;
}

void CGameObject::SetChild(CGameObject* pChild, bool bReferenceUpdate)
{
	if (pChild)
	{
		pChild->m_pParent = this;
		if (bReferenceUpdate) pChild->AddRef();
	}
	if (m_pChild)
	{
		if (pChild) pChild->m_pSibling = m_pChild->m_pSibling;
		m_pChild->m_pSibling = pChild;
	}
	else
	{
		m_pChild = pChild;
	}
}

void CGameObject::SetBoundingBoxMesh(CBoundingBoxMesh* pMesh)
{
	if (m_pBoundingBoxMesh) m_pBoundingBoxMesh->Release();
	m_pBoundingBoxMesh = pMesh;
	if (pMesh) pMesh->AddRef();
}

void CGameObject::UpdateBoundingBox(XMFLOAT3 xmf3NextPos)
{
	m_bCheckBB = true;
	if (m_pMesh)
	{
		m_xmBoundingBox = m_pMesh->m_xmBoundingBox;
		XMFLOAT4X4 xmf4x4World = m_xmf4x4World;
		xmf4x4World._41 += xmf3NextPos.x * 0.1;
		xmf4x4World._43 += xmf3NextPos.z * 0.1;
		m_pMesh->m_xmBoundingBox.Transform(m_xmBoundingBox, XMLoadFloat4x4(&xmf4x4World));
		//XMStoreFloat4(&m_xmBoundingBox.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmBoundingBox.Orientation)));
		MoveBBToParent(this);
		return;
	}
	if (m_pSibling)m_pSibling->UpdateBoundingBox(xmf3NextPos);
	if (m_pChild)m_pChild->UpdateBoundingBox(xmf3NextPos);

}

void CGameObject::RenderBoundingBox(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_pBoundingBoxMesh)
	{
		m_pBoundingBoxMesh->UpdateVertexPosition(&m_xmBoundingBox);
		m_pBoundingBoxMesh->Render(pd3dCommandList);
	}
	if (m_pSibling) m_pSibling->RenderBoundingBox(pd3dCommandList);
	if (m_pChild) m_pChild->RenderBoundingBox(pd3dCommandList);
}

void CGameObject::MoveBBToParent(CGameObject* pTargetLv)
{
	if (pTargetLv->m_pParent) {
		pTargetLv->m_pParent->m_xmBoundingBox = pTargetLv->m_xmBoundingBox;
		MoveBBToParent(pTargetLv->m_pParent);
	}
	else return;
}

void CGameObject::SetMesh(CMesh* pMesh)
{
	if (m_pMesh) m_pMesh->Release();
	m_pMesh = pMesh;
	if (m_pMesh) m_pMesh->AddRef();
}

void CGameObject::SetShader(CShader* pShader)
{
	m_nMaterials = 1;
	m_ppMaterials = new CMaterial * [m_nMaterials];
	m_ppMaterials[0] = new CMaterial(0);
	m_ppMaterials[0]->SetShader(pShader);
}

void CGameObject::SetShader(int nMaterial, CShader* pShader)
{
	if (m_ppMaterials[nMaterial]) m_ppMaterials[nMaterial]->SetShader(pShader);
}

void CGameObject::SetMaterial(int nMaterial, CMaterial* pMaterial)
{
	if (m_ppMaterials[nMaterial]) m_ppMaterials[nMaterial]->Release();
	m_ppMaterials[nMaterial] = pMaterial;
	if (m_ppMaterials[nMaterial]) m_ppMaterials[nMaterial]->AddRef();
}

CSkinnedMesh* CGameObject::FindSkinnedMesh(char* pstrSkinnedMeshName)
{
	CSkinnedMesh* pSkinnedMesh = NULL;
	if (m_pMesh && (m_pMesh->GetType() & VERTEXT_BONE_INDEX_WEIGHT))
	{
		pSkinnedMesh = (CSkinnedMesh*)m_pMesh;
		if (!strncmp(pSkinnedMesh->m_pstrMeshName, pstrSkinnedMeshName, strlen(pstrSkinnedMeshName))) return(pSkinnedMesh);
	}

	if (m_pSibling) if (pSkinnedMesh = m_pSibling->FindSkinnedMesh(pstrSkinnedMeshName)) return(pSkinnedMesh);
	if (m_pChild) if (pSkinnedMesh = m_pChild->FindSkinnedMesh(pstrSkinnedMeshName)) return(pSkinnedMesh);

	return(NULL);
}

void CGameObject::FindAndSetSkinnedMesh(CSkinnedMesh** ppSkinnedMeshes, int* pnSkinnedMesh)
{
	if (m_pMesh && (m_pMesh->GetType() & VERTEXT_BONE_INDEX_WEIGHT)) ppSkinnedMeshes[(*pnSkinnedMesh)++] = (CSkinnedMesh*)m_pMesh;

	if (m_pSibling) m_pSibling->FindAndSetSkinnedMesh(ppSkinnedMeshes, pnSkinnedMesh);
	if (m_pChild) m_pChild->FindAndSetSkinnedMesh(ppSkinnedMeshes, pnSkinnedMesh);
}

CGameObject* CGameObject::FindFrame(char* pstrFrameName)
{
	CGameObject* pFrameObject = NULL;
	if (!strncmp(m_pstrFrameName, pstrFrameName, strlen(pstrFrameName))) return(this);

	if (m_pSibling) if (pFrameObject = m_pSibling->FindFrame(pstrFrameName)) return(pFrameObject);
	if (m_pChild) if (pFrameObject = m_pChild->FindFrame(pstrFrameName)) return(pFrameObject);

	return(NULL);
}

void CGameObject::UpdateTransform(XMFLOAT4X4* pxmf4x4Parent)
{
	m_xmf4x4World = (pxmf4x4Parent) ? Matrix4x4::Multiply(m_xmf4x4ToParent, *pxmf4x4Parent) : m_xmf4x4ToParent;

	if (m_pSibling) m_pSibling->UpdateTransform(pxmf4x4Parent);
	if (m_pChild) m_pChild->UpdateTransform(&m_xmf4x4World);
}

void CGameObject::SetTrackAnimationSet(int nAnimationTrack, int nAnimationSet)
{
	if (m_pSkinnedAnimationController) m_pSkinnedAnimationController->SetTrackAnimationSet(nAnimationTrack, nAnimationSet);
}

void CGameObject::SetTrackAnimationPosition(int nAnimationTrack, float fPosition)
{
	if (m_pSkinnedAnimationController) m_pSkinnedAnimationController->SetTrackPosition(nAnimationTrack, fPosition);
}

void CGameObject::Animate(float fTimeElapsed)
{
	OnPrepareRender();
	Update(fTimeElapsed);
	//BB
	if (!m_bCheckBB)UpdateBoundingBox(XMFLOAT3(0, 0, 0));
	if (m_pSkinnedAnimationController) m_pSkinnedAnimationController->AdvanceTime(fTimeElapsed, this);

	if (m_pSibling) m_pSibling->Animate(fTimeElapsed);
	if (m_pChild) m_pChild->Animate(fTimeElapsed);
}


void CGameObject::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (m_pSkinnedAnimationController) m_pSkinnedAnimationController->UpdateShaderVariables(pd3dCommandList);

	if (m_pMesh/*&&IsVisible(pCamera)*/)
	{
		UpdateShaderVariable(pd3dCommandList, &m_xmf4x4World);

		if (m_nMaterials > 0)
		{
			for (int i = 0; i < m_nMaterials; i++)
			{
				if (m_ppMaterials[i])
				{
					if (m_ppMaterials[i]->m_pShader) m_ppMaterials[i]->m_pShader->Render(pd3dCommandList, pCamera);
					m_ppMaterials[i]->UpdateShaderVariable(pd3dCommandList);
				}

				m_pMesh->Render(pd3dCommandList, i);
			}
		}
	}

	if (m_pSibling) m_pSibling->Render(pd3dCommandList, pCamera);
	if (m_pChild) m_pChild->Render(pd3dCommandList, pCamera);

}

void CGameObject::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void CGameObject::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, XMFLOAT4X4* pxmf4x4World)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4World, 0);
}

void CGameObject::UpdateShaderVariable(ID3D12GraphicsCommandList* pd3dCommandList, CMaterial* pMaterial)
{
}

void CGameObject::ReleaseShaderVariables()
{
}

void CGameObject::ReleaseUploadBuffers()
{
	if (m_pMesh) {
		m_pMesh->ReleaseUploadBuffers();
	}

	for (int i = 0; i < m_nMaterials; i++)
	{
		if (m_ppMaterials[i]) m_ppMaterials[i]->ReleaseUploadBuffers();
	}

	if (m_pSibling) m_pSibling->ReleaseUploadBuffers();
	if (m_pChild) m_pChild->ReleaseUploadBuffers();
}

void CGameObject::SetPosition(float x, float y, float z)
{
	m_xmf4x4ToParent._41 = x;
	m_xmf4x4ToParent._42 = y;
	m_xmf4x4ToParent._43 = z;

	UpdateTransform(NULL);
}

void CGameObject::SetPosition(XMFLOAT3 xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}

void CGameObject::Move(XMFLOAT3 xmf3Offset)
{
	m_xmf4x4ToParent._41 += xmf3Offset.x;
	m_xmf4x4ToParent._42 += xmf3Offset.y;
	m_xmf4x4ToParent._43 += xmf3Offset.z;

	UpdateTransform(NULL);
}

void CGameObject::SetScale(float x, float y, float z)
{
	XMMATRIX mtxScale = XMMatrixScaling(x, y, z);
	m_xmf4x4ToParent = Matrix4x4::Multiply(mtxScale, m_xmf4x4ToParent);

	UpdateTransform(NULL);
}

XMFLOAT3 CGameObject::GetPosition()
{
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));
}

XMFLOAT3 CGameObject::GetToParentPosition()
{
	return(XMFLOAT3(m_xmf4x4ToParent._41, m_xmf4x4ToParent._42, m_xmf4x4ToParent._43));
}

XMFLOAT3 CGameObject::GetLook()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33)));
}

XMFLOAT3 CGameObject::GetUp()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23)));
}

XMFLOAT3 CGameObject::GetRight()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13)));
}

void CGameObject::MoveStrafe(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);
	CGameObject::SetPosition(xmf3Position);
}

void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch), XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	m_xmf4x4ToParent = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParent);

	UpdateTransform(NULL);
}

void CGameObject::Rotate(XMFLOAT3* pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis), XMConvertToRadians(fAngle));
	m_xmf4x4ToParent = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParent);

	UpdateTransform(NULL);
}

void CGameObject::Rotate(XMFLOAT4* pxmf4Quaternion)
{
	XMMATRIX mtxRotate = XMMatrixRotationQuaternion(XMLoadFloat4(pxmf4Quaternion));
	m_xmf4x4ToParent = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParent);

	UpdateTransform(NULL);
}

//#define _WITH_DEBUG_FRAME_HIERARCHY

CTexture* CGameObject::FindReplicatedTexture(_TCHAR* pstrTextureName)
{
	for (int i = 0; i < m_nMaterials; i++)
	{
		if (m_ppMaterials[i])
		{
			for (int j = 0; j < m_ppMaterials[i]->m_nTextures; j++)
			{
				if (m_ppMaterials[i]->m_ppTextures[j])
				{
					if (!_tcsncmp(m_ppMaterials[i]->m_ppstrTextureNames[j], pstrTextureName, _tcslen(pstrTextureName))) return(m_ppMaterials[i]->m_ppTextures[j]);
				}
			}
		}
	}
	CTexture* pTexture = NULL;
	if (m_pSibling) if (pTexture = m_pSibling->FindReplicatedTexture(pstrTextureName)) return(pTexture);
	if (m_pChild) if (pTexture = m_pChild->FindReplicatedTexture(pstrTextureName)) return(pTexture);

	return(NULL);
}

int ReadIntegerFromFile(FILE* pInFile)
{
	int nValue = 0;
	UINT nReads = (UINT)::fread(&nValue, sizeof(int), 1, pInFile);
	return(nValue);
}

float ReadFloatFromFile(FILE* pInFile)
{
	float fValue = 0;
	UINT nReads = (UINT)::fread(&fValue, sizeof(float), 1, pInFile);
	return(fValue);
}

BYTE ReadStringFromFile(FILE* pInFile, char* pstrToken)
{
	BYTE nStrLength = 0;
	UINT nReads = 0;
	nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
	nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
	pstrToken[nStrLength] = '\0';

	return(nStrLength);
}

void CGameObject::LoadMaterialsFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CGameObject* pParent, FILE* pInFile, CShader* pShader)
{
	char pstrToken[64] = { '\0' };
	int nMaterial = 0;
	UINT nReads = 0;

	m_nMaterials = ReadIntegerFromFile(pInFile);

	m_ppMaterials = new CMaterial * [m_nMaterials];
	for (int i = 0; i < m_nMaterials; i++) m_ppMaterials[i] = NULL;

	CMaterial* pMaterial = NULL;

	for (; ; )
	{
		::ReadStringFromFile(pInFile, pstrToken);

		if (!strcmp(pstrToken, "<Material>:"))
		{
			nMaterial = ReadIntegerFromFile(pInFile);

			pMaterial = new CMaterial(7); //0:Albedo, 1:Specular, 2:Metallic, 3:Normal, 4:Emission, 5:DetailAlbedo, 6:DetailNormal

			if (!pShader)
			{
				UINT nMeshType = GetMeshType();
				if (nMeshType & VERTEXT_NORMAL_TANGENT_TEXTURE)
				{
					if (nMeshType & VERTEXT_BONE_INDEX_WEIGHT)
					{
						pMaterial->SetSkinnedAnimationShader();
					}
					else
					{
						pMaterial->SetStandardShader();
					}
				}
			}
			SetMaterial(nMaterial, pMaterial);
		}
		else if (!strcmp(pstrToken, "<AlbedoColor>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_xmf4AlbedoColor), sizeof(float), 4, pInFile);
		}
		else if (!strcmp(pstrToken, "<EmissiveColor>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_xmf4EmissiveColor), sizeof(float), 4, pInFile);
		}
		else if (!strcmp(pstrToken, "<SpecularColor>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_xmf4SpecularColor), sizeof(float), 4, pInFile);
		}
		else if (!strcmp(pstrToken, "<Glossiness>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fGlossiness), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<Smoothness>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fSmoothness), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<Metallic>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fSpecularHighlight), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<SpecularHighlight>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fMetallic), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<GlossyReflection>:"))
		{
			nReads = (UINT)::fread(&(pMaterial->m_fGlossyReflection), sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<AlbedoMap>:"))
		{
			pMaterial->LoadTextureFromFile(pd3dDevice, pd3dCommandList, MATERIAL_ALBEDO_MAP, 3, pMaterial->m_ppstrTextureNames[0], &(pMaterial->m_ppTextures[0]), pParent, pInFile, pShader);
		}
		else if (!strcmp(pstrToken, "<SpecularMap>:"))
		{
			m_ppMaterials[nMaterial]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, MATERIAL_SPECULAR_MAP, 4, pMaterial->m_ppstrTextureNames[1], &(pMaterial->m_ppTextures[1]), pParent, pInFile, pShader);
		}
		else if (!strcmp(pstrToken, "<NormalMap>:"))
		{
			m_ppMaterials[nMaterial]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, MATERIAL_NORMAL_MAP, 5, pMaterial->m_ppstrTextureNames[2], &(pMaterial->m_ppTextures[2]), pParent, pInFile, pShader);
		}
		else if (!strcmp(pstrToken, "<MetallicMap>:"))
		{
			m_ppMaterials[nMaterial]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, MATERIAL_METALLIC_MAP, 6, pMaterial->m_ppstrTextureNames[3], &(pMaterial->m_ppTextures[3]), pParent, pInFile, pShader);
		}
		else if (!strcmp(pstrToken, "<EmissionMap>:"))
		{
			m_ppMaterials[nMaterial]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, MATERIAL_EMISSION_MAP, 7, pMaterial->m_ppstrTextureNames[4], &(pMaterial->m_ppTextures[4]), pParent, pInFile, pShader);
		}
		else if (!strcmp(pstrToken, "<DetailAlbedoMap>:"))
		{
			m_ppMaterials[nMaterial]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, MATERIAL_DETAIL_ALBEDO_MAP, 8, pMaterial->m_ppstrTextureNames[5], &(pMaterial->m_ppTextures[5]), pParent, pInFile, pShader);
		}
		else if (!strcmp(pstrToken, "<DetailNormalMap>:"))
		{
			m_ppMaterials[nMaterial]->LoadTextureFromFile(pd3dDevice, pd3dCommandList, MATERIAL_DETAIL_NORMAL_MAP, 9, pMaterial->m_ppstrTextureNames[6], &(pMaterial->m_ppTextures[6]), pParent, pInFile, pShader);
		}
		else if (!strcmp(pstrToken, "</Materials>"))
		{
			break;
		}
	}
}

CGameObject* CGameObject::LoadFrameHierarchyFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CGameObject* pParent, FILE* pInFile, CShader* pShader, int* pnSkinnedMeshes)
{
	// 기존 코드
	
	char pstrToken[64] = { '\0' };
	UINT nReads = 0;

	int nFrame = 0, nTextures = 0;

	CGameObject* pGameObject = new CGameObject();

	for (; ; )
	{
		::ReadStringFromFile(pInFile, pstrToken);
		if (!strcmp(pstrToken, "<Frame>:"))
		{
			nFrame = ::ReadIntegerFromFile(pInFile);
			nTextures = ::ReadIntegerFromFile(pInFile);

			::ReadStringFromFile(pInFile, pGameObject->m_pstrFrameName);
		}
		else if (!strcmp(pstrToken, "<Transform>:"))
		{
			XMFLOAT3 xmf3Position, xmf3Rotation, xmf3Scale;
			XMFLOAT4 xmf4Rotation;
			nReads = (UINT)::fread(&xmf3Position, sizeof(float), 3, pInFile);
			nReads = (UINT)::fread(&xmf3Rotation, sizeof(float), 3, pInFile); //Euler Angle
			nReads = (UINT)::fread(&xmf3Scale, sizeof(float), 3, pInFile);
			nReads = (UINT)::fread(&xmf4Rotation, sizeof(float), 4, pInFile); //Quaternion
		}
		else if (!strcmp(pstrToken, "<TransformMatrix>:"))
		{
			nReads = (UINT)::fread(&pGameObject->m_xmf4x4ToParent, sizeof(float), 16, pInFile);
		}
		else if (!strcmp(pstrToken, "<Mesh>:"))
		{
			CStandardMesh* pMesh = new CStandardMesh(pd3dDevice, pd3dCommandList);
			pMesh->LoadMeshFromFile(pd3dDevice, pd3dCommandList, pInFile);
			pGameObject->SetMesh(pMesh);

			XMFLOAT4 bbColor = XMFLOAT4(1.f, 0.f, 0.f, 1.f); // 애니 없으면 빨강

			CBoundingBoxMesh* pBoundingBoxMesh = new CBoundingBoxMesh(pd3dDevice, pd3dCommandList, bbColor);
			pGameObject->SetBoundingBoxMesh(pBoundingBoxMesh);
		}
		else if (!strcmp(pstrToken, "<SkinningInfo>:"))
		{
			if (pnSkinnedMeshes) (*pnSkinnedMeshes)++;

			CSkinnedMesh* pSkinnedMesh = new CSkinnedMesh(pd3dDevice, pd3dCommandList);
			pSkinnedMesh->LoadSkinInfoFromFile(pd3dDevice, pd3dCommandList, pInFile);
			pSkinnedMesh->CreateShaderVariables(pd3dDevice, pd3dCommandList);

			::ReadStringFromFile(pInFile, pstrToken); //<Mesh>:
			if (!strcmp(pstrToken, "<Mesh>:")) {
				pSkinnedMesh->LoadMeshFromFile(pd3dDevice, pd3dCommandList, pInFile);

				XMFLOAT4 bbColor = XMFLOAT4(0.f, 1.f, 0.f, 1.f); // 애니 있음 초록

				CBoundingBoxMesh* pBoundingBoxMesh = new CBoundingBoxMesh(pd3dDevice, pd3dCommandList, bbColor);
				pGameObject->SetBoundingBoxMesh(pBoundingBoxMesh);
			}

			pGameObject->SetMesh(pSkinnedMesh);
		}
		else if (!strcmp(pstrToken, "<Materials>:"))
		{
			pGameObject->LoadMaterialsFromFile(pd3dDevice, pd3dCommandList, pParent, pInFile, pShader);
		}
		else if (!strcmp(pstrToken, "<Children>:"))
		{
			int nChilds = ::ReadIntegerFromFile(pInFile);
			if (nChilds > 0)
			{
				for (int i = 0; i < nChilds; i++)
				{
					CGameObject* pChild = CGameObject::LoadFrameHierarchyFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pGameObject, pInFile, pShader, pnSkinnedMeshes);
					if (pChild) pGameObject->SetChild(pChild);
#ifdef _WITH_DEBUG_FRAME_HIERARCHY
					TCHAR pstrDebug[256] = { 0 };
					_stprintf_s(pstrDebug, 256, "(Frame: %p) (Parent: %p)\n"), pChild, pGameObject);
					OutputDebugString(pstrDebug);
#endif
				}
			}
		}
		else if (!strcmp(pstrToken, "</Frame>"))
		{
			break;
		}
	}
	return(pGameObject);
	

}

void CGameObject::PrintFrameInfo(CGameObject* pGameObject, CGameObject* pParent)
{
	TCHAR pstrDebug[256] = { 0 };

	_stprintf_s(pstrDebug, 256, _T("(Frame: %p) (Parent: %p)\n"), pGameObject, pParent);
	OutputDebugString(pstrDebug);

	if (pGameObject->m_pSibling) CGameObject::PrintFrameInfo(pGameObject->m_pSibling, pParent);
	if (pGameObject->m_pChild) CGameObject::PrintFrameInfo(pGameObject->m_pChild, pGameObject);
}

bool CGameObject::IsVisible(CCamera* pCamera)
{
	OnPrepareRender();
	bool bIsVisible = false;
	BoundingOrientedBox xmBoundingBox = m_pMesh->m_xmBoundingBox;
	if (pCamera) bIsVisible = reinterpret_cast<CThirdPersonCamera*>(pCamera)->IsInFrustum(xmBoundingBox);
	return(bIsVisible);
}

void CGameObject::LoadAnimationFromFile(FILE* pInFile, CLoadedModelInfo* pLoadedModel)
{
	char pstrToken[64] = { '\0' };
	UINT nReads = 0;

	int nAnimationSets = 0;

	for (; ; )
	{
		::ReadStringFromFile(pInFile, pstrToken);
		if (!strcmp(pstrToken, "<AnimationSets>:"))
		{
			nAnimationSets = ::ReadIntegerFromFile(pInFile);
			pLoadedModel->m_pAnimationSets = new CAnimationSets(nAnimationSets);
		}
		else if (!strcmp(pstrToken, "<FrameNames>:"))
		{
			pLoadedModel->m_pAnimationSets->m_nBoneFrames = ::ReadIntegerFromFile(pInFile);
			pLoadedModel->m_pAnimationSets->m_ppBoneFrameCaches = new CGameObject * [pLoadedModel->m_pAnimationSets->m_nBoneFrames];

			for (int j = 0; j < pLoadedModel->m_pAnimationSets->m_nBoneFrames; j++)
			{
				::ReadStringFromFile(pInFile, pstrToken);
				pLoadedModel->m_pAnimationSets->m_ppBoneFrameCaches[j] = pLoadedModel->m_pModelRootObject->FindFrame(pstrToken);

#ifdef _WITH_DEBUG_SKINNING_BONE
				TCHAR pstrDebug[256] = { 0 };
				TCHAR pwstrAnimationBoneName[64] = { 0 };
				TCHAR pwstrBoneCacheName[64] = { 0 };
				size_t nConverted = 0;
				mbstowcs_s(&nConverted, pwstrAnimationBoneName, 64, pstrToken, _TRUNCATE);
				mbstowcs_s(&nConverted, pwstrBoneCacheName, 64, pLoadedModel->m_ppBoneFrameCaches[j]->m_pstrFrameName, _TRUNCATE);
				_stprintf_s(pstrDebug, 256, _T("AnimationBoneFrame:: Cache(%s) AnimationBone(%s)\n"), pwstrBoneCacheName, pwstrAnimationBoneName);
				OutputDebugString(pstrDebug);
#endif
			}
		}
		else if (!strcmp(pstrToken, "<AnimationSet>:"))
		{
			int nAnimationSet = ::ReadIntegerFromFile(pInFile);

			::ReadStringFromFile(pInFile, pstrToken); //Animation Set Name

			float fLength = ::ReadFloatFromFile(pInFile);
			int nFramesPerSecond = ::ReadIntegerFromFile(pInFile);
			int nKeyFrames = ::ReadIntegerFromFile(pInFile);

			pLoadedModel->m_pAnimationSets->m_pAnimationSets[nAnimationSet] = new CAnimationSet(fLength, nFramesPerSecond, nKeyFrames, pLoadedModel->m_pAnimationSets->m_nBoneFrames, pstrToken);

			for (int i = 0; i < nKeyFrames; i++)
			{
				::ReadStringFromFile(pInFile, pstrToken);
				if (!strcmp(pstrToken, "<Transforms>:"))
				{
					CAnimationSet* pAnimationSet = pLoadedModel->m_pAnimationSets->m_pAnimationSets[nAnimationSet];

					int nKey = ::ReadIntegerFromFile(pInFile); //i
					float fKeyTime = ::ReadFloatFromFile(pInFile);

#ifdef _WITH_ANIMATION_SRT
					m_pfKeyFrameScaleTimes[i] = fKeyTime;
					m_pfKeyFrameRotationTimes[i] = fKeyTime;
					m_pfKeyFrameTranslationTimes[i] = fKeyTime;
					nReads = (UINT)::fread(pAnimationSet->m_ppxmf3KeyFrameScales[i], sizeof(XMFLOAT3), pLoadedModel->m_pAnimationSets->m_nBoneFrames, pInFile);
					nReads = (UINT)::fread(pAnimationSet->m_ppxmf4KeyFrameRotations[i], sizeof(XMFLOAT4), pLoadedModel->m_pAnimationSets->m_nBoneFrames, pInFile);
					nReads = (UINT)::fread(pAnimationSet->m_ppxmf3KeyFrameTranslations[i], sizeof(XMFLOAT3), pLoadedModel->m_pAnimationSets->m_nBoneFrames, pInFile);
#else
					pAnimationSet->m_pfKeyFrameTimes[i] = fKeyTime;
					nReads = (UINT)::fread(pAnimationSet->m_ppxmf4x4KeyFrameTransforms[i], sizeof(XMFLOAT4X4), pLoadedModel->m_pAnimationSets->m_nBoneFrames, pInFile);
#endif
				}
			}
		}
		else if (!strcmp(pstrToken, "</AnimationSets>"))
		{
			break;
		}
	}
}

CLoadedModelInfo* CGameObject::LoadGeometryAndAnimationFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, char* pstrFileName, CShader* pShader)
{
	FILE* pInFile = NULL;
	::fopen_s(&pInFile, pstrFileName, "rb");
	::rewind(pInFile);

	CLoadedModelInfo* pLoadedModel = new CLoadedModelInfo();

	char pstrToken[64] = { '\0' };

	for (; ; )
	{
		if (::ReadStringFromFile(pInFile, pstrToken))
		{
			if (!strcmp(pstrToken, "<Hierarchy>:"))
			{
				pLoadedModel->m_pModelRootObject = CGameObject::LoadFrameHierarchyFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, NULL, pInFile, pShader, &pLoadedModel->m_nSkinnedMeshes);
				::ReadStringFromFile(pInFile, pstrToken); //"</Hierarchy>"
			}
			else if (!strcmp(pstrToken, "<Animation>:"))
			{
				CGameObject::LoadAnimationFromFile(pInFile, pLoadedModel);
				pLoadedModel->PrepareSkinning();
			}
			else if (!strcmp(pstrToken, "</Animation>:"))
			{
				break;
			}
		}
		else
		{
			break;
		}
	}

#ifdef _WITH_DEBUG_FRAME_HIERARCHY
	TCHAR pstrDebug[256] = { 0 };
	_stprintf_s(pstrDebug, 256, "Frame Hierarchy\n"));
	OutputDebugString(pstrDebug);

	CGameObject::PrintFrameInfo(pGameObject, NULL);
#endif

	return(pLoadedModel);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CHeightMapTerrain::CHeightMapTerrain(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, LPCTSTR pFileName, int nWidth, int nLength, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color) : CGameObject(1)
{
	m_nWidth = nWidth;
	m_nLength = nLength;

	m_xmf3Scale = xmf3Scale;

	m_pHeightMapImage = new CHeightMapImage(pFileName, nWidth, nLength, xmf3Scale);

	CHeightMapGridMesh* pMesh = new CHeightMapGridMesh(pd3dDevice, pd3dCommandList, 0, 0, nWidth, nLength, xmf3Scale, xmf4Color, m_pHeightMapImage);
	SetMesh(pMesh);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CTexture* pTerrainBaseTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pTerrainBaseTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Terrain/Base_Texture.dds", RESOURCE_TEXTURE2D, 0);

	CTexture* pTerrainDetailTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pTerrainDetailTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Terrain/Detail_Texture_8.dds", RESOURCE_TEXTURE2D, 0);

	CTerrainShader* pTerrainShader = new CTerrainShader();
	pTerrainShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D32_FLOAT);
	pTerrainShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);


	CScene::CreateShaderResourceViews(pd3dDevice, pTerrainBaseTexture, 0, 13);
	CScene::CreateShaderResourceViews(pd3dDevice, pTerrainDetailTexture, 0, 14);

	CMaterial* pTerrainMaterial = new CMaterial(2);
	pTerrainMaterial->SetTexture(pTerrainBaseTexture, 0);
	pTerrainMaterial->SetTexture(pTerrainDetailTexture, 1);
	pTerrainMaterial->SetShader(pTerrainShader);

	SetMaterial(0, pTerrainMaterial);
}

CHeightMapTerrain::~CHeightMapTerrain(void)
{
	if (m_pHeightMapImage) delete m_pHeightMapImage;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
CSkyBox::CSkyBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, wchar_t* pszFileName) : CGameObject(1)
{
	CSkyBoxMesh* pSkyBoxMesh = new CSkyBoxMesh(pd3dDevice, pd3dCommandList, 20.0f, 20.0f, 2.0f);
	SetMesh(pSkyBoxMesh);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CTexture* pSkyBoxTexture = new CTexture(1, RESOURCE_TEXTURE_CUBE, 0, 1);
	pSkyBoxTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, pszFileName, RESOURCE_TEXTURE_CUBE, 0);

	CSkyBoxShader* pSkyBoxShader = new CSkyBoxShader();
	pSkyBoxShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature, 1, NULL, DXGI_FORMAT_D32_FLOAT);
	pSkyBoxShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CScene::CreateShaderResourceViews(pd3dDevice, pSkyBoxTexture, 0, 10);

	CMaterial* pSkyBoxMaterial = new CMaterial(1);
	pSkyBoxMaterial->SetTexture(pSkyBoxTexture);
	pSkyBoxMaterial->SetShader(pSkyBoxShader);

	SetMaterial(0, pSkyBoxMaterial);
}

CSkyBox::~CSkyBox()
{
}

void CSkyBox::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	XMFLOAT3 xmf3CameraPos = pCamera->GetPosition();
	SetPosition(xmf3CameraPos.x, xmf3CameraPos.y, xmf3CameraPos.z);

	CGameObject::Render(pd3dCommandList, pCamera);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
CAngrybotAnimationController::CAngrybotAnimationController(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int nAnimationTracks, CLoadedModelInfo* pModel) : CAnimationController(pd3dDevice, pd3dCommandList, nAnimationTracks, pModel)
{
}

CAngrybotAnimationController::~CAngrybotAnimationController()
{
}

void CAngrybotAnimationController::OnRootMotion(CGameObject* pRootGameObject)
{

}

CAngrybotObject::CAngrybotObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CLoadedModelInfo* pModel, int nAnimationTracks)
{
	CLoadedModelInfo* pAngrybotModel = pModel;
	if (!pAngrybotModel) pAngrybotModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Player.bin", NULL);

	SetChild(pAngrybotModel->m_pModelRootObject, true);
	m_pSkinnedAnimationController = new CAngrybotAnimationController(pd3dDevice, pd3dCommandList, nAnimationTracks, pAngrybotModel);
}

CAngrybotObject::~CAngrybotObject()
{
}



////////////////////////////////////////////////////////////////////////////////////////////////////
//
CRobotObject::CRobotObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CLoadedModelInfo* pModel, int nAnimationTracks)
{
	CLoadedModelInfo* pRobotModel = pModel;
	if (!pRobotModel) pRobotModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Robot.bin", NULL);

	SetChild(pRobotModel->m_pModelRootObject, true);

	m_pSkinnedAnimationController = new CAnimationController(pd3dDevice, pd3dCommandList, nAnimationTracks, pRobotModel);

	for (int i = 0; i < 3; ++i)
		m_pSkinnedAnimationController->SetTrackAnimationSet(i, i);

	// Default animation setting
	m_pSkinnedAnimationController->SetAllTrackDisable();
	m_pSkinnedAnimationController->SetTrackEnable(0, true);
	m_pSkinnedAnimationController->SetTrackSpeed(0, 0.5f);
}

CRobotObject::~CRobotObject()
{
}

void CRobotObject::Update(float fTimeElapsed)
{
	CGameObject::Update(fTimeElapsed);
	if (m_pSkinnedAnimationController) {
		AnimationBlending(m_pasCurrentAni, m_pasNextAni);
		IsAttackP() ? 0 : MoveToTarget(), IsMove(m_pasNextAni);

	}
}

void CRobotObject::MoveToTarget()
{
	XMFLOAT3 xmfVel = Vector3::XMVectorToFloat3(XMLoadFloat3(&m_xmf3Target) - XMLoadFloat3(&GetPosition()));
	float fLength = sqrt(xmfVel.x * xmfVel.x + xmfVel.z * xmfVel.z);

	if (fLength < 1.0f) {
		m_xmf3Target = XMFLOAT3(0, 0, 0);
	}
	


	/*Vector3::IsZero(m_xmf3Target) ?
		m_pasNextAni = IDLE : RotateDirection(5.f, m_xmf3Target), m_pasNextAni = WALK;
	if (!Vector3::IsZero(m_xmf3Target))
		Vector3::IsZero(Vector3::XMVectorToFloat3(XMLoadFloat3(&m_xmf3Target) - XMLoadFloat3(&GetPosition()))) ?
		m_pasNextAni = IDLE : MoveForward(0.4f), m_pasNextAni = WALK;
	else { m_pasNextAni = IDLE; }*/


	if (Vector3::IsZero(m_xmf3Target)) {
		m_pasNextAni = IDLE;
	}
	else  {
		if (Vector3::IsZero(Vector3::XMVectorToFloat3(XMLoadFloat3(&m_xmf3Target) - XMLoadFloat3(&GetPosition())))) {
			m_pasNextAni = IDLE;
		}
		else {
			RotateDirection(10.f, m_xmf3Target);
			MoveForward(0.33f);
			m_pasNextAni = WALK;
		}
		// 
		m_pasNextAni = WALK;
	}


}

bool CRobotObject::RotateDirection(float fAngle, XMFLOAT3& xmf3Target)
{
	XMFLOAT3 xmfVel = Vector3::XMVectorToFloat3(XMLoadFloat3(&xmf3Target) - XMLoadFloat3(&GetPosition()));
	if (Vector3::IsZero(xmfVel))xmf3Target = XMFLOAT3(0.f, 0.f, 0.f);

	float fCurrentAngle = Vector3::Angle(Vector3::Normalize(xmfVel), GetLook());
	if (fCurrentAngle > 5.f) {

		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&GetUp()), XMConvertToRadians(fAngle));
		XMFLOAT3 xmf3Look = Vector3::TransformNormal(GetLook(), xmmtxRotate);

		float fNextAngle = Vector3::Angle(Vector3::Normalize(xmfVel), xmf3Look);

		fCurrentAngle >= fNextAngle ? Rotate(0.0f, (fCurrentAngle - fNextAngle) / 1.5, 0.0f) : Rotate(0.0f, -(fNextAngle - fCurrentAngle) / 1.5, 0.0f);
		return 0;
	}
	else return 1;
}

void CRobotObject::AnimationBlending(Player_Animation_ST type1, Player_Animation_ST type2)
{
	if (m_pSkinnedAnimationController->m_fBlendingTime <= 1.0f && type2 != NONE) {
		// If need to blending
		// 체인지 애니메이션(섞인거) 
		m_pSkinnedAnimationController->m_fBlendingWeight = 2.0f;
		m_pSkinnedAnimationController->SetAnimationBlending(true); // 블렌딩을 할지 말지
		m_pSkinnedAnimationController->SetTrackBlending(m_pasCurrentAni, type2); // 몇번째랑 몇번째
	}
	else { // 무조건 WALK
		if (type2 != NONE)m_pasCurrentAni = type2;
		m_pSkinnedAnimationController->SetTrackEnable(m_pasCurrentAni, true);
		m_pSkinnedAnimationController->SetAnimationBlending(false);
		m_pasNextAni = NONE;
	}
}

void CRobotObject::IsMove(Player_Animation_ST CheckAni)
{
	if (m_pasCurrentAni != CheckAni && m_pSkinnedAnimationController->m_fBlendingTime >= 1.0f) {
		m_pasNextAni = CheckAni;
		m_pSkinnedAnimationController->m_fBlendingTime = 0.0f;
		m_pSkinnedAnimationController->SetTrackSpeed(0, 0.3f);
		// AnimationPacket(m_pasNextAni);
	}
}

void CRobotObject::IsIdle()
{
}

bool CRobotObject::IsAttackP()
{
	if (m_bAttackStatus == true) {
		if (m_pasCurrentAni != RUN && m_pSkinnedAnimationController->m_fBlendingTime >= 1.0f) {
			m_pasNextAni = RUN;
			m_pSkinnedAnimationController->m_fBlendingTime = 0.0f;
			m_pSkinnedAnimationController->SetTrackType(RUN, ANIMATION_TYPE_ONCE);
			m_pSkinnedAnimationController->SetTrackSpeed(0, 0.3f);
		}
		XMFLOAT3 xmfVel = Vector3::XMVectorToFloat3(XMLoadFloat3(&m_xmf3Target) - XMLoadFloat3(&GetPosition()));
		if (Vector3::IsZero(xmfVel))m_xmf3Target = XMFLOAT3(0.f, 0.f, 0.f);

		float fCurrentAngle = Vector3::Angle(Vector3::Normalize(xmfVel), GetLook());
		if (fCurrentAngle > 1) {

			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&GetUp()), XMConvertToRadians(fCurrentAngle));
			XMFLOAT3 xmf3Look = Vector3::TransformNormal(GetLook(), xmmtxRotate);

			float fNextAngle = Vector3::Angle(Vector3::Normalize(xmfVel), xmf3Look);

			fCurrentAngle >= fNextAngle ? Rotate(0.0f, (fCurrentAngle - fNextAngle), 0.0f) : Rotate(0.0f, -(fNextAngle - fCurrentAngle), 0.0f);
		}
		CAnimationTrack AnimationTrack = m_pSkinnedAnimationController->m_pAnimationTracks[RUN];
		CAnimationSet* pAnimationSet = m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[AnimationTrack.m_nAnimationSet];
		float fTrackPosition = AnimationTrack.m_fPosition;
		float fTrackLength = pAnimationSet->m_fLength;
		if (fTrackPosition >= fTrackLength) {
			m_pSkinnedAnimationController->m_pAnimationTracks[RUN].m_fPosition = 0;
			m_pSkinnedAnimationController->m_fBlendingTime = 0.0f;
			m_pasNextAni = IDLE;
			m_bAttackStatus = false;
			return false;
		}
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
CStandardOBJ::CStandardOBJ(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CLoadedModelInfo* pModel)
{
	CLoadedModelInfo* pObjectModel = pModel;
	if (!pObjectModel) pObjectModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/BigContainer.bin", NULL);

	SetChild(pObjectModel->m_pModelRootObject, true);
}

CStandardOBJ::~CStandardOBJ()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
CMissonOBJ::CMissonOBJ(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CLoadedModelInfo* pModel, XMFLOAT3 f3MissionRange, int nCategory)
{
	CLoadedModelInfo* pObjectModel = pModel;
	if (!pObjectModel) pObjectModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/ObjModel/Barrel.bin", NULL);

	SetChild(pObjectModel->m_pModelRootObject, true);

	SetMissionRange(f3MissionRange.x, f3MissionRange.y, f3MissionRange.z);

	XMFLOAT4 bbColor = XMFLOAT4(0.f, 0.f, 1.f, 1.f); // 미션 범위는 파랑
	CBoundingBoxMesh* MissionRangeMesh = new CBoundingBoxMesh(pd3dDevice, pd3dCommandList, bbColor);
	SetMissionRangeMesh(MissionRangeMesh);

	m_nCategory = nCategory;

}

CMissonOBJ::~CMissonOBJ()
{
}

void CMissonOBJ::RenderBoundingBox(ID3D12GraphicsCommandList* pd3dCommandList)
{
	CGameObject::RenderBoundingBox(pd3dCommandList);
	RenderMissionRange(pd3dCommandList);
}

void CMissonOBJ::SetMissionRange(float x, float y, float z)
{
	m_xmMissionRange.Extents.x = x;
	m_xmMissionRange.Extents.y = y;
	m_xmMissionRange.Extents.z = z;
}

void CMissonOBJ::SetMissionRangeMesh(CBoundingBoxMesh* pMesh)
{
	if (m_pMissionRangeMesh) m_pMissionRangeMesh->Release();
	m_pMissionRangeMesh = pMesh;
	if (pMesh) pMesh->AddRef();
}

void CMissonOBJ::RenderMissionRange(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_pMissionRangeMesh)
	{
		m_pMissionRangeMesh->UpdateVertexPosition(&m_xmMissionRange);
		m_pMissionRangeMesh->Render(pd3dCommandList);
	}
}

void CMissonOBJ::SetPosition(float x, float y, float z)
{
	CGameObject::SetPosition(x, y, z);

	m_xmMissionRange.Center = XMFLOAT3(x, y, z);

}

void CMissonOBJ::SetPosition(XMFLOAT3 xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//
CFloorObj::CFloorObj(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CLoadedModelInfo* pModel)
{
	CLoadedModelInfo* pObjectModel = pModel;
	if (!pObjectModel) pObjectModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/test/Occ_Range.bin", NULL);

	SetChild(pObjectModel->m_pModelRootObject, true);

	XMFLOAT4 bbColor = XMFLOAT4(0.f, 0.f, 0.f, 0.f); //땅 바운딩 박스는 투명
	CBoundingBoxMesh* MissionRangeMesh = new CBoundingBoxMesh(pd3dDevice, pd3dCommandList, bbColor);

}

CFloorObj::~CFloorObj()
{
}

CBossRobotObject::CBossRobotObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CLoadedModelInfo* pModel, int nAnimationTracks)
{
	CLoadedModelInfo* pRobotModel = pModel;
	if (!pRobotModel) pRobotModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Robot2.bin", NULL);

	SetChild(pRobotModel->m_pModelRootObject, true);

	m_pSkinnedAnimationController = new CAnimationController(pd3dDevice, pd3dCommandList, nAnimationTracks, pRobotModel);

	for (int i = 0; i < 7; ++i)
		m_pSkinnedAnimationController->SetTrackAnimationSet(i, i);

	// Default animation setting
	m_pSkinnedAnimationController->SetAllTrackDisable();
	m_pSkinnedAnimationController->SetTrackEnable(0, true);
	m_pSkinnedAnimationController->SetTrackSpeed(WALK, 0.5f);
	m_pSkinnedAnimationController->SetTrackSpeed(RUN, 0.5f);
	m_pSkinnedAnimationController->SetTrackSpeed(HIT, 0.7f);
}

CBossRobotObject::~CBossRobotObject()
{
}

void CBossRobotObject::Update(float fTimeElapsed)
{
	CGameObject::Update(fTimeElapsed);
	if (m_pSkinnedAnimationController) {
		AnimationBlending(m_pasCurrentAni, m_pasNextAni);
		m_nDiyCheck = IsDiying();
		if (m_nDiyCheck == 0) {
			IsAttackP(CREEP) ? 0 : MoveToTarget(), IsMove(m_pasNextAni);
		}

	}
}

void CBossRobotObject::MoveToTarget()
{
	XMFLOAT3 xmfVel = Vector3::XMVectorToFloat3(XMLoadFloat3(&m_xmf3Target) - XMLoadFloat3(&GetPosition()));
	float fLength = sqrt(xmfVel.x * xmfVel.x + xmfVel.z * xmfVel.z);

	if (fLength < 1.f) {
		m_xmf3Target = XMFLOAT3(0.f, 0.f, 0.f);
	}



	if (Vector3::IsZero(m_xmf3Target)) {
		m_pasNextAni = IDLE;
	}
	else {
		if (Vector3::IsZero(Vector3::XMVectorToFloat3(XMLoadFloat3(&m_xmf3Target) - XMLoadFloat3(&GetPosition())))) {
			m_pasNextAni = IDLE;
		}
		else {
			RotateDirection(10.f);
			MoveForward(0.165f);
			m_pasNextAni = WALK;
		}
		// 
		m_pasNextAni = WALK;
	}
}

void CBossRobotObject::RotateDirection(float fAngle)
{
	XMFLOAT3 xmfVel = Vector3::XMVectorToFloat3(XMLoadFloat3(&m_xmf3Target) - XMLoadFloat3(&GetPosition()));
	if (Vector3::IsZero(xmfVel))m_xmf3Target = XMFLOAT3(0.f, 0.f, 0.f);

	float fCurrentAngle = Vector3::Angle(Vector3::Normalize(xmfVel), GetLook());

	if (fCurrentAngle > 5.f) {

		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&GetUp()), XMConvertToRadians(fAngle));
		XMFLOAT3 xmf3Look = Vector3::TransformNormal(GetLook(), xmmtxRotate);

		float fNextAngle = Vector3::Angle(Vector3::Normalize(xmfVel), xmf3Look);

		fCurrentAngle >= fNextAngle ? Rotate(0.0f, (fCurrentAngle - fNextAngle) / 1.5, 0.0f) : Rotate(0.0f, -(fNextAngle - fCurrentAngle) / 1.5, 0.0f);
	}
}

void CBossRobotObject::AnimationBlending(Player_Animation_ST type1, Player_Animation_ST type2)
{
	if (m_pSkinnedAnimationController->m_fBlendingTime <= 1.0f && type2 != NONE) {
		// If need to blending
		// 체인지 애니메이션(섞인거) 
		m_pSkinnedAnimationController->m_fBlendingWeight = 2.0f;
		m_pSkinnedAnimationController->SetAnimationBlending(true); // 블렌딩을 할지 말지
		m_pSkinnedAnimationController->SetTrackBlending(m_pasCurrentAni, type2); // 몇번째랑 몇번째
	}
	else { // 무조건 WALK
		if (type2 != NONE)m_pasCurrentAni = type2;
		m_pSkinnedAnimationController->SetTrackEnable(m_pasCurrentAni, true);
		m_pSkinnedAnimationController->SetAnimationBlending(false);
		m_pasNextAni = NONE;
	}
}

void CBossRobotObject::IsMove(Player_Animation_ST CheckAni)
{
	if (m_pasCurrentAni != CheckAni && m_pSkinnedAnimationController->m_fBlendingTime >= 1.0f) {
		m_pasNextAni = CheckAni;
		m_pSkinnedAnimationController->m_fBlendingTime = 0.0f;
		m_pSkinnedAnimationController->SetTrackSpeed(0, 0.3f);
		// AnimationPacket(m_pasNextAni);
	}
}

bool CBossRobotObject::IsAttackP(Player_Animation_ST status)
{
	Player_Animation_ST Status = Player_Animation_ST(int(status) + AttackType);
	if (m_bAttackStatus == true) {
		if (m_pasCurrentAni != Status && m_pSkinnedAnimationController->m_fBlendingTime >= 1.0f) {
			m_pasNextAni = Status;
			m_pSkinnedAnimationController->m_fBlendingTime = 0.0f;
			m_pSkinnedAnimationController->SetTrackType(Status, ANIMATION_TYPE_ONCE);
		}
		CAnimationTrack AnimationTrack = m_pSkinnedAnimationController->m_pAnimationTracks[Status];
		CAnimationSet* pAnimationSet = m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[AnimationTrack.m_nAnimationSet];
		float fTrackPosition = AnimationTrack.m_fPosition;
		float fTrackLength = pAnimationSet->m_fLength;
		if (fTrackPosition >= fTrackLength) {
			m_pSkinnedAnimationController->m_pAnimationTracks[Status].m_fPosition = 0;
			m_pSkinnedAnimationController->m_fBlendingTime = 0.0f;
			m_pasNextAni = IDLE;
			m_bAttackStatus = false;
			return false;
		}
		return true;
	}
	return false;
}

int CBossRobotObject::IsDiying()
{
	if (m_bDiyingStatus == true) {
		if (m_pasCurrentAni != HIT && m_pSkinnedAnimationController->m_fBlendingTime >= 1.0f) {
			m_pasNextAni = HIT;
			m_pSkinnedAnimationController->m_fBlendingTime = 0.0f;
			m_pSkinnedAnimationController->SetTrackType(HIT, ANIMATION_TYPE_ONCE);
		}
		CAnimationTrack AnimationTrack = m_pSkinnedAnimationController->m_pAnimationTracks[HIT];
		CAnimationSet* pAnimationSet = m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[AnimationTrack.m_nAnimationSet];
		float fTrackPosition = AnimationTrack.m_fPosition;
		float fTrackLength = pAnimationSet->m_fLength;
		if (fTrackPosition >= fTrackLength-0.4f) {
			//m_pSkinnedAnimationController->m_pAnimationTracks[HIT].m_fPosition = fTrackLength-0.4f;
			m_pSkinnedAnimationController->m_fBlendingTime = 0.0f;
			m_pasNextAni = NONE;
			return 2;
		}
		return 1;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CParticle::CParticle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, XMFLOAT3 xmf3Pos) : CGameObject(1)
{
	ParticleMesh* pParticleMesh = new ParticleMesh(pd3dDevice, pd3dCommandList, xmf3Pos);
	SetMesh(pParticleMesh);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CTexture* pParticleTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pParticleTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"SkyBox/SkyBox_0.dds", RESOURCE_TEXTURE2D, 0);

	ParticleShader* pParticleShader = new ParticleShader();
	pParticleShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pParticleShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CScene::CreateShaderResourceViews(pd3dDevice, pParticleTexture, 0, 19);

	CMaterial* pParticleMaterial = new CMaterial(1);
	pParticleMaterial->SetTexture(pParticleTexture,0);
	pParticleMaterial->SetShader(pParticleShader);

	SetMaterial(0, pParticleMaterial);
}

CParticle::~CParticle()
{
}

void CParticle::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	XMFLOAT3 xmf3CameraPos = pCamera->GetPosition();
	SetPosition(xmf3CameraPos.x, xmf3CameraPos.y, xmf3CameraPos.z);
	reinterpret_cast<ParticleMesh*>(m_pMesh)->UpdatePosTime(GetPosition());
	CGameObject::Render(pd3dCommandList, pCamera);
}
