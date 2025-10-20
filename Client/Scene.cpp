//-----------------------------------------------------------------------------
// File: CScene.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Scene.h"

ID3D12DescriptorHeap* CScene::m_pd3dCbvSrvDescriptorHeap = NULL;

D3D12_CPU_DESCRIPTOR_HANDLE	CScene::m_d3dCbvCPUDescriptorStartHandle;
D3D12_GPU_DESCRIPTOR_HANDLE	CScene::m_d3dCbvGPUDescriptorStartHandle;
D3D12_CPU_DESCRIPTOR_HANDLE	CScene::m_d3dSrvCPUDescriptorStartHandle;
D3D12_GPU_DESCRIPTOR_HANDLE	CScene::m_d3dSrvGPUDescriptorStartHandle;

D3D12_CPU_DESCRIPTOR_HANDLE	CScene::m_d3dCbvCPUDescriptorNextHandle;
D3D12_GPU_DESCRIPTOR_HANDLE	CScene::m_d3dCbvGPUDescriptorNextHandle;
D3D12_CPU_DESCRIPTOR_HANDLE	CScene::m_d3dSrvCPUDescriptorNextHandle;
D3D12_GPU_DESCRIPTOR_HANDLE	CScene::m_d3dSrvGPUDescriptorNextHandle;

CScene::CScene()
{
}

CScene::~CScene()
{
}

void CScene::BuildDefaultLightsAndMaterials()
{
	m_nLights = MAX_LIGHTS;
	m_pLights = new LIGHT[m_nLights];
	::ZeroMemory(m_pLights, sizeof(LIGHT) * m_nLights);

	m_xmf4GlobalAmbient = XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f);

	// 전체 조명
	m_pLights[0].m_bEnable = true;
	m_pLights[0].m_nType = DIRECTIONAL_LIGHT;
	m_pLights[0].m_fRange = 1000.0f;

	m_pLights[0].m_xmf4Ambient = XMFLOAT4(0.5f, 1.0f, 0.3f, 1.0f);
	m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.4f, 0.3f, 0.6f, 0.0f);
	m_pLights[0].m_xmf4Specular = XMFLOAT4(0.7f, 0.6f, 0.5f, 0.0f);

	m_pLights[0].m_xmf3Position = XMFLOAT3(500.f, 200.0f, 500.f);
	m_pLights[0].m_xmf3Direction = XMFLOAT3(0.3f, -0.8f, 0.0f);

	// Player Flash Light 
	m_pLights[1].m_bEnable = false;
	m_pLights[1].m_nType = SPOT_LIGHT;
	m_pLights[1].m_fRange = 500.0f;
	
	m_pLights[1].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights[1].m_xmf4Diffuse = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	m_pLights[1].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 0.0f);
	m_pLights[1].m_xmf3Position = XMFLOAT3(-50.0f, 20.0f, -5.0f);

	m_pLights[1].m_xmf3Direction = XMFLOAT3(0.0f, -1.0f, 1.0f);
	m_pLights[1].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);

	m_pLights[1].m_fFalloff = 8.0f;
	m_pLights[1].m_fPhi = (float)cos(XMConvertToRadians(40.0f));
	m_pLights[1].m_fTheta = (float)cos(XMConvertToRadians(20.0f));

	// Sun
	m_pLights[2].m_bEnable = false;
	m_pLights[2].m_nType = DIRECTIONAL_LIGHT;
	m_pLights[2].m_xmf4Ambient = XMFLOAT4(0.2f, 1.0f, 0.9f, 1.0f);
	m_pLights[2].m_xmf4Diffuse = XMFLOAT4(0.4f, 0.3f, 0.6f, 1.0f);
	m_pLights[2].m_xmf4Specular = XMFLOAT4(0.7f, 0.6f, 0.5f, 0.0f);
	m_pLights[2].m_xmf3Direction = XMFLOAT3(-1.0f, -1.0f, 0.0f);

	m_pLights[2].m_xmf3Position = XMFLOAT3(500.0f, 100.0f, 500.0f);

	// 중앙 상단에 붉은 조명 추가(분위기 조성용)
	m_pLights[3].m_bEnable = false;
	m_pLights[3].m_nType = SPOT_LIGHT;
	m_pLights[3].m_fRange = 800.0f;
	m_pLights[3].m_fFalloff = 8.0f;

	m_pLights[3].m_xmf4Ambient = XMFLOAT4(1.0f, 0.3f, 0.3f, 1.0f);
	m_pLights[3].m_xmf4Diffuse = XMFLOAT4(1.0f, 0.3f, 0.3f, 1.0f);
	m_pLights[3].m_xmf4Specular = XMFLOAT4(1.0f, 0.3f, 0.3f, 0.0f);
	m_pLights[3].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);

	m_pLights[3].m_xmf3Position = XMFLOAT3(300.0f, 100.0f, 500.0f);
	m_pLights[3].m_xmf3Direction = XMFLOAT3(-1.0f, 0.0f, -1.0f);

	m_pLights[3].m_fPhi = (float)cos(XMConvertToRadians(60.0f));
	m_pLights[3].m_fTheta = (float)cos(XMConvertToRadians(20.0f));


	// ??
	m_pLights[4].m_bEnable = false;
	m_pLights[4].m_nType = POINT_LIGHT;
	m_pLights[4].m_fRange = 200.0f;
	m_pLights[4].m_xmf4Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_pLights[4].m_xmf4Diffuse = XMFLOAT4(0.8f, 0.3f, 0.3f, 1.0f);
	m_pLights[4].m_xmf4Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.0f);
	m_pLights[4].m_xmf3Position = XMFLOAT3(600.0f, 250.0f, 700.0f);
	m_pLights[4].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.001f, 0.0001f);
	m_pLights[4].m_xmf3Direction = XMFLOAT3(-1.0f, -1.0f, 0.0f);

	
}

void CScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int myPlayernum)
{

	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 1200); //나중에 다시 계산해서 넣기
	
	BuildDefaultLightsAndMaterials();

	CMaterial::PrepareShaders(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

}

void CScene::CreateShadowShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList , bool bBoss)
{
	m_pDepthRenderShader = new CDepthRenderShader(m_ppHierarchicalGameObjects, m_ppMissionObj, m_pLights, m_nHierarchicalGameObjects, m_nMissionObj);
	m_pDepthRenderShader->SetShadowObject(m_ppModelInfoPlayer, m_nPlayer, m_ppEnemy, m_nEnemy, m_ppFloorObj, m_nFloorObj, m_pBoss, bBoss);
	DXGI_FORMAT pdxgiRtvFormats[1] = { DXGI_FORMAT_D32_FLOAT };
	m_pDepthRenderShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature, 1, pdxgiRtvFormats, DXGI_FORMAT_D32_FLOAT);
	m_pDepthRenderShader->BuildObjects(pd3dDevice, pd3dCommandList, NULL);

}

void CScene::ReleaseObjects()
{
	//UI
	if (m_pUI)
	{
		m_pUI->Release();
		delete m_pUI;
	}

	// 셰이더 
	if (m_ppShaders)
	{
		for (int i = 0; i < m_nShaders; i++)
		{
			m_ppShaders[i]->ReleaseShaderVariables();
			m_ppShaders[i]->ReleaseObjects();			// 내부 객체
			m_ppShaders[i]->Release();					// Scene 객체
		}
		delete[] m_ppShaders;
	}
	if (m_pDepthRenderShader)
	{
		m_pDepthRenderShader->ReleaseShaderVariables();
		m_pDepthRenderShader->ReleaseObjects();
		m_pDepthRenderShader->Release();
	}


	// 오브젝트 해제 (Upload 버퍼 포함)
	if (m_ppPlayer) {
		for (int i = 0; i < m_nPlayer; ++i) {
			m_ppPlayer[i]->Release();
		}
		delete[] m_ppPlayer;

	}

	if (m_ppMissionObj)
	{
		for (int i = 0; i < m_nMissionObj; i++) if (m_ppMissionObj[i])
		{
			m_ppMissionObj[i]->ReleaseUploadBuffers();
			m_ppMissionObj[i]->Release();
		}
		delete[] m_ppMissionObj;
	}

	if (m_ppHierarchicalGameObjects)
	{
		for (int i = 0; i < m_nHierarchicalGameObjects; i++) if (m_ppHierarchicalGameObjects[i])
		{
			m_ppHierarchicalGameObjects[i]->Release();					// 객체
		}
		delete[] m_ppHierarchicalGameObjects;
	}

	if (m_ppEnemy)
	{
		for (int i = 0; i < m_nEnemy; i++) if (m_ppEnemy[i]) m_ppEnemy[i]->Release();
		delete[] m_ppEnemy;
	}

	if (m_pBoss) {
		m_pBoss->Release();
	}

	// 지형&스카이박스
	if (m_pTerrain) delete m_pTerrain;
	if (m_pSkyBox) delete m_pSkyBox;

	// 조명
	ReleaseShaderVariables();
	if (m_pLights) delete[] m_pLights;

	// DirectX 리소스
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();
	if (m_pd3dCbvSrvDescriptorHeap) m_pd3dCbvSrvDescriptorHeap->Release();

}

ID3D12RootSignature* CScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;

	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[13];

	pd3dDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[0].NumDescriptors = 1;
	pd3dDescriptorRanges[0].BaseShaderRegister = 6; //t6: gtxtAlbedoTexture
	pd3dDescriptorRanges[0].RegisterSpace = 0;
	pd3dDescriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[1].NumDescriptors = 1;
	pd3dDescriptorRanges[1].BaseShaderRegister = 7; //t7: gtxtSpecularTexture
	pd3dDescriptorRanges[1].RegisterSpace = 0;
	pd3dDescriptorRanges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[2].NumDescriptors = 1;
	pd3dDescriptorRanges[2].BaseShaderRegister = 8; //t8: gtxtNormalTexture
	pd3dDescriptorRanges[2].RegisterSpace = 0;
	pd3dDescriptorRanges[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[3].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[3].NumDescriptors = 1;
	pd3dDescriptorRanges[3].BaseShaderRegister = 9; //t9: gtxtMetallicTexture
	pd3dDescriptorRanges[3].RegisterSpace = 0;
	pd3dDescriptorRanges[3].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[4].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[4].NumDescriptors = 1;
	pd3dDescriptorRanges[4].BaseShaderRegister = 10; //t10: gtxtEmissionTexture
	pd3dDescriptorRanges[4].RegisterSpace = 0;
	pd3dDescriptorRanges[4].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[5].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[5].NumDescriptors = 1;
	pd3dDescriptorRanges[5].BaseShaderRegister = 11; //t11: gtxtDetailAlbedoTexture
	pd3dDescriptorRanges[5].RegisterSpace = 0;
	pd3dDescriptorRanges[5].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[6].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[6].NumDescriptors = 1;
	pd3dDescriptorRanges[6].BaseShaderRegister = 12; //t12: gtxtEmissionTexture
	pd3dDescriptorRanges[6].RegisterSpace = 0;
	pd3dDescriptorRanges[6].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[7].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[7].NumDescriptors = 1;
	pd3dDescriptorRanges[7].BaseShaderRegister = 13; //t13: gtxtSkyBoxTexture
	pd3dDescriptorRanges[7].RegisterSpace = 0;
	pd3dDescriptorRanges[7].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[8].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[8].NumDescriptors = 1;
	pd3dDescriptorRanges[8].BaseShaderRegister = 1; //t1: gtxtTerrainBaseTexture
	pd3dDescriptorRanges[8].RegisterSpace = 0;
	pd3dDescriptorRanges[8].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[9].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[9].NumDescriptors = 1;
	pd3dDescriptorRanges[9].BaseShaderRegister = 2; //t2: gtxtTerrainDetailTexture
	pd3dDescriptorRanges[9].RegisterSpace = 0;
	pd3dDescriptorRanges[9].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[10].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[10].NumDescriptors = 5;
	pd3dDescriptorRanges[10].BaseShaderRegister = 14; //t14~t18: Texture2D<float4>
	pd3dDescriptorRanges[10].RegisterSpace = 0;
	pd3dDescriptorRanges[10].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[11].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[11].NumDescriptors = MAX_DEPTH_TEXTURES;
	pd3dDescriptorRanges[11].BaseShaderRegister = 19; //Depth Buffer (Light.hlsl / t19)
	pd3dDescriptorRanges[11].RegisterSpace = 0;
	pd3dDescriptorRanges[11].OffsetInDescriptorsFromTableStart = 0;

	pd3dDescriptorRanges[12].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[12].NumDescriptors = 1;
	pd3dDescriptorRanges[12].BaseShaderRegister = 25; //t25: gtxtParticleTexture
	pd3dDescriptorRanges[12].RegisterSpace = 0;
	pd3dDescriptorRanges[12].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER pd3dRootParameters[20];

	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[0].Descriptor.ShaderRegister = 1; //b1 Camera
	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[1].Constants.Num32BitValues = 33;
	pd3dRootParameters[1].Constants.ShaderRegister = 2; //b2 GameObject
	pd3dRootParameters[1].Constants.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[2].Descriptor.ShaderRegister = 4; //b4 Lights
	pd3dRootParameters[2].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[3].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[3].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[0]);
	pd3dRootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[4].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[4].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[1]);
	pd3dRootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[5].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[5].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[2]);
	pd3dRootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[6].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[6].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[3]);
	pd3dRootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[7].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[7].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[4]);
	pd3dRootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[8].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[8].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[8].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[5]);
	pd3dRootParameters[8].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[9].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[9].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[9].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[6]);
	pd3dRootParameters[9].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[10].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[10].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[10].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[7]);
	pd3dRootParameters[10].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[11].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[11].Descriptor.ShaderRegister = 7; //Skinned Bone Offsets
	pd3dRootParameters[11].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[11].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pd3dRootParameters[12].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[12].Descriptor.ShaderRegister = 8; //Skinned Bone Transforms
	pd3dRootParameters[12].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[12].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pd3dRootParameters[13].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[13].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[13].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[8]); //gtxtTerrainBaseTexture
	pd3dRootParameters[13].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[14].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[14].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[14].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[9]); //gtxtTerrainDetailTexture
	pd3dRootParameters[14].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[15].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[15].Descriptor.ShaderRegister = 5; //DrawOptions
	pd3dRootParameters[15].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[15].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[16].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[16].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[16].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[10]); //Texture2D
	pd3dRootParameters[16].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[17].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[17].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[17].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[11]; //Depth Buffer
	pd3dRootParameters[17].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[18].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[18].Descriptor.ShaderRegister = 6; //b6 ToLight 
	pd3dRootParameters[18].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[18].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[19].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[19].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[19].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[12]); //Texture2D
	pd3dRootParameters[19].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_STATIC_SAMPLER_DESC pd3dSamplerDescs[3];

	pd3dSamplerDescs[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	pd3dSamplerDescs[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].MipLODBias = 0;
	pd3dSamplerDescs[0].MaxAnisotropy = 1;
	pd3dSamplerDescs[0].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	pd3dSamplerDescs[0].MinLOD = 0;
	pd3dSamplerDescs[0].MaxLOD = D3D12_FLOAT32_MAX;
	pd3dSamplerDescs[0].ShaderRegister = 0;
	pd3dSamplerDescs[0].RegisterSpace = 0;
	pd3dSamplerDescs[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dSamplerDescs[1].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	pd3dSamplerDescs[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[1].MipLODBias = 0;
	pd3dSamplerDescs[1].MaxAnisotropy = 1;
	pd3dSamplerDescs[1].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	pd3dSamplerDescs[1].MinLOD = 0;
	pd3dSamplerDescs[1].MaxLOD = D3D12_FLOAT32_MAX;
	pd3dSamplerDescs[1].ShaderRegister = 1;
	pd3dSamplerDescs[1].RegisterSpace = 0;
	pd3dSamplerDescs[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dSamplerDescs[2].Filter = D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	pd3dSamplerDescs[2].AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	pd3dSamplerDescs[2].AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	pd3dSamplerDescs[2].AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	pd3dSamplerDescs[2].MipLODBias = 0.0f;
	pd3dSamplerDescs[2].MaxAnisotropy = 1;
	pd3dSamplerDescs[2].ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL; //D3D12_COMPARISON_FUNC_LESS
	pd3dSamplerDescs[2].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE; // D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	pd3dSamplerDescs[2].MinLOD = 0;
	pd3dSamplerDescs[2].MaxLOD = D3D12_FLOAT32_MAX;
	pd3dSamplerDescs[2].ShaderRegister = 2;
	pd3dSamplerDescs[2].RegisterSpace = 0;
	pd3dSamplerDescs[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = _countof(pd3dSamplerDescs);
	d3dRootSignatureDesc.pStaticSamplers = pd3dSamplerDescs;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob* pd3dSignatureBlob = NULL;
	ID3DBlob* pd3dErrorBlob = NULL;
	D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&pd3dGraphicsRootSignature);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
}

void CScene::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256의 배수
	m_pd3dcbLights = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbLights->Map(0, NULL, (void**)&m_pcbMappedLights);
}

void CScene::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	::memcpy(m_pcbMappedLights->m_pLights, m_pLights, sizeof(LIGHT) * m_nLights);
	::memcpy(&m_pcbMappedLights->m_xmf4GlobalAmbient, &m_xmf4GlobalAmbient, sizeof(XMFLOAT4));
	::memcpy(&m_pcbMappedLights->m_nLights, &m_nLights, sizeof(int));
}

void CScene::ReleaseShaderVariables()
{
	if (m_pd3dcbLights)
	{
		m_pd3dcbLights->Unmap(0, NULL);
		m_pd3dcbLights->Release();
	}
}

void CScene::ReleaseUploadBuffers()
{
	if (m_pSkyBox) m_pSkyBox->ReleaseUploadBuffers();
	if (m_pTerrain) m_pTerrain->ReleaseUploadBuffers();

	for (int i = 0; i < m_nPlayer; ++i) m_ppPlayer[i]->ReleaseUploadBuffers();

	for (int i = 0; i < m_nShaders; i++) m_ppShaders[i]->ReleaseUploadBuffers();
	for (int i = 0; i < m_nHierarchicalGameObjects; i++) m_ppHierarchicalGameObjects[i]->ReleaseUploadBuffers();
	for (int i = 0; i < m_nMissionObj; i++) m_ppMissionObj[i]->ReleaseUploadBuffers();
	for (int i = 0; i < m_nEnemy; i++) m_ppEnemy[i]->ReleaseUploadBuffers();
	if (m_pBoss) m_pBoss->ReleaseUploadBuffers();

	if (m_pDepthRenderShader) m_pDepthRenderShader->ReleaseUploadBuffers();

}

void CScene::CreateCbvSrvDescriptorHeaps(ID3D12Device* pd3dDevice, int nConstantBufferViews, int nShaderResourceViews)
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	d3dDescriptorHeapDesc.NumDescriptors = nConstantBufferViews + nShaderResourceViews; //CBVs + SRVs 
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	d3dDescriptorHeapDesc.NodeMask = 0;
	pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_pd3dCbvSrvDescriptorHeap);

	m_d3dCbvCPUDescriptorNextHandle = m_d3dCbvCPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_d3dCbvGPUDescriptorNextHandle = m_d3dCbvGPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	m_d3dSrvCPUDescriptorNextHandle.ptr = m_d3dSrvCPUDescriptorStartHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * nConstantBufferViews);
	m_d3dSrvGPUDescriptorNextHandle.ptr = m_d3dSrvGPUDescriptorStartHandle.ptr = m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * nConstantBufferViews);
}

D3D12_GPU_DESCRIPTOR_HANDLE CScene::CreateConstantBufferViews(ID3D12Device* pd3dDevice, int nConstantBufferViews, ID3D12Resource* pd3dConstantBuffers, UINT nStride)
{
	D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvGPUDescriptorHandle = m_d3dCbvGPUDescriptorNextHandle;
	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = pd3dConstantBuffers->GetGPUVirtualAddress();
	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;
	d3dCBVDesc.SizeInBytes = nStride;
	for (int j = 0; j < nConstantBufferViews; j++)
	{
		d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress + (nStride * j);
		m_d3dCbvCPUDescriptorNextHandle.ptr = m_d3dCbvCPUDescriptorNextHandle.ptr + ::gnCbvSrvDescriptorIncrementSize;
		pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, m_d3dCbvCPUDescriptorNextHandle);
		m_d3dCbvGPUDescriptorNextHandle.ptr = m_d3dCbvGPUDescriptorNextHandle.ptr + ::gnCbvSrvDescriptorIncrementSize;
	}
	return(d3dCbvGPUDescriptorHandle);
}

D3D12_GPU_DESCRIPTOR_HANDLE CScene::CreateConstantBufferView(ID3D12Device* pd3dDevice, ID3D12Resource* pd3dConstantBuffer, UINT nStride)
{
	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;
	d3dCBVDesc.SizeInBytes = nStride;
	d3dCBVDesc.BufferLocation = pd3dConstantBuffer->GetGPUVirtualAddress();
	pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, m_d3dCbvCPUDescriptorNextHandle);
	D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvGPUDescriptorHandle = m_d3dCbvGPUDescriptorNextHandle;
	m_d3dCbvCPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
	m_d3dCbvGPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;

	return(d3dCbvGPUDescriptorHandle);
}

D3D12_GPU_DESCRIPTOR_HANDLE CScene::CreateShaderResourceView(ID3D12Device* pd3dDevice, ID3D12Resource* pd3dResource, DXGI_FORMAT dxgiSrvFormat)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc;
	d3dShaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	d3dShaderResourceViewDesc.Format = dxgiSrvFormat;
	d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	d3dShaderResourceViewDesc.Texture2D.MipLevels = 1;
	d3dShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	d3dShaderResourceViewDesc.Texture2D.PlaneSlice = 0;
	d3dShaderResourceViewDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGPUDescriptorHandle = m_d3dSrvGPUDescriptorNextHandle;
	pd3dDevice->CreateShaderResourceView(pd3dResource, &d3dShaderResourceViewDesc, m_d3dSrvCPUDescriptorNextHandle);
	m_d3dSrvCPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
	m_d3dSrvGPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;

	return(d3dSrvGPUDescriptorHandle);
}



void CScene::CreateShaderResourceViews(ID3D12Device* pd3dDevice, CTexture* pTexture, UINT nDescriptorHeapIndex, UINT nRootParameterStartIndex)
{
	m_d3dSrvCPUDescriptorNextHandle.ptr += (::gnCbvSrvDescriptorIncrementSize * nDescriptorHeapIndex);
	m_d3dSrvGPUDescriptorNextHandle.ptr += (::gnCbvSrvDescriptorIncrementSize * nDescriptorHeapIndex);

	if (pTexture)
	{
		int nTextures = pTexture->GetTextures();
		for (int i = 0; i < nTextures; i++)
		{
			ID3D12Resource* pShaderResource = pTexture->GetResource(i);
			D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc = pTexture->GetShaderResourceViewDesc(i);
			pd3dDevice->CreateShaderResourceView(pShaderResource, &d3dShaderResourceViewDesc, m_d3dSrvCPUDescriptorNextHandle);
			m_d3dSrvCPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;

			pTexture->SetGpuDescriptorHandle(i, m_d3dSrvGPUDescriptorNextHandle);
			m_d3dSrvGPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
		}
	}
	int nRootParameters = pTexture->GetRootParameters();
	for (int j = 0; j < nRootParameters; j++) pTexture->SetRootParameterIndex(j, nRootParameterStartIndex + j);
}


bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN: {
		switch (wParam) {
		default:
			break;
		}
	case WM_KEYUP: {
		switch (wParam) {
		default:
			break;
		}
	}
	}
	}

	return(false);
}

bool CScene::ProcessInput(HWND m_hWnd, POINT m_ptOldCursorPos, UCHAR* pKeysBuffer)
{
	return false;
}

void CScene::AnimateObjects(float fTimeElapsed)
{
	m_fElapsedTime = fTimeElapsed;

	for (int i = 0; i < m_nShaders; i++) if (m_ppShaders[i]) m_ppShaders[i]->AnimateObjects(fTimeElapsed);

	for (int i = 0; i < m_nHierarchicalGameObjects; i++) if (m_ppHierarchicalGameObjects[i])	m_ppHierarchicalGameObjects[i]->Animate(m_fElapsedTime);
	for (int i = 0; i < m_nEnemy; i++) if (m_ppEnemy[i]) m_ppEnemy[i]->Animate(m_fElapsedTime);
	if(m_pBoss)m_pBoss->Animate(m_fElapsedTime);


	
	for (int i = 0; i < m_nPlayer; i++) if (m_ppPlayer[i]) {

		m_ppPlayer[i]->Animate(fTimeElapsed);
		bool flag = false;
		
		// MAP Obj
		for (int j = 0; j < m_nHierarchicalGameObjects; ++j) {
			if (CheckObjByObjCollition(m_ppPlayer[i], m_ppHierarchicalGameObjects[j], m_ppPlayer[i]->m_xmf3ContactNormal)) {
				flag = true;
				break;
			}
		}

		XMFLOAT3 look = m_pMyPlayer->GetLookVector();
		XMFLOAT3 look2 = m_pMyPlayer->GetLook();

		//Mission Obj
		for (int k = 0; k < m_nMissionObj; ++k) if(m_ppMissionObj[k]) {
			if (CheckObjByObjCollition(m_ppPlayer[i], m_ppMissionObj[k], m_ppPlayer[i]->m_xmf3ContactNormal)) {
				flag = true;
			}
		}

		m_ppPlayer[i]->m_bIntersects = flag;
		m_ppPlayer[i]->Update(fTimeElapsed);
	}



	// 플레이어 플레쉬 라이트 사용 안하니까 일단 주석
	/*
	if (m_pLights)
	{
		m_pLights[1].m_xmf3Position = m_ppPlayer[FIRST_PLAYER]->GetPosition();
		m_pLights[1].m_xmf3Direction = m_ppPlayer[FIRST_PLAYER]->GetLookVector();
	}
	*/  
}

void CScene::OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_pd3dGraphicsRootSignature) pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	if (m_pd3dCbvSrvDescriptorHeap) pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);


	UpdateShaderVariables(pd3dCommandList);

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(2, d3dcbLightsGpuVirtualAddress); //Lights
}

void CScene::OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	BoundingOrientedBox xmBoundingBoxs = CalculateBoundingBox();
	if (m_pDepthRenderShader)m_pDepthRenderShader->PrepareShadowMap( &xmBoundingBoxs, pd3dCommandList, pCamera); // 그림자 준비 연산

}

void CScene::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	UpdateShaderVariables(pd3dCommandList);

	// 그림자 연산을 위한 깊이 렌더 쉐이더 
	if(m_pDepthRenderShader)m_pDepthRenderShader->UpdateShaderVariables(pd3dCommandList);
	
	
	// 카메라 변경
	//pCamera = m_pMyPlayer->GetCamera();
	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	//////////////////////////////////////////

	m_pDepthRenderShader->UpdateTextureShader(pd3dCommandList);

	//=======================================

	if (m_pSkyBox) m_pSkyBox->Render(pd3dCommandList, pCamera);
	if (m_pTerrain) m_pTerrain->Render(pd3dCommandList, pCamera);

	for (int i = 0; i < m_nShaders; i++) if (m_ppShaders[i]) m_ppShaders[i]->Render(pd3dCommandList, pCamera);

	for (int i = 0; i < m_nHierarchicalGameObjects; i++)
	{
		if (m_ppHierarchicalGameObjects[i])
		{
			if (!m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController) m_ppHierarchicalGameObjects[i]->UpdateTransform(NULL);
			m_ppHierarchicalGameObjects[i]->Animate(m_fElapsedTime);
			m_ppHierarchicalGameObjects[i]->Render(pd3dCommandList, pCamera);
		}
	}

	for (int i = 0; i < m_nMissionObj; i++)
	{
		if (m_ppMissionObj[i])
		{
			if (!m_ppMissionObj[i]->m_pSkinnedAnimationController) m_ppMissionObj[i]->UpdateTransform(NULL);
			m_ppMissionObj[i]->Animate(m_fElapsedTime);
			m_ppMissionObj[i]->Render(pd3dCommandList, pCamera);
		}
	}

	for (int i = 0; i < m_nEnemy; i++)
	{
		if (m_ppEnemy[i])
		{
			if (!m_ppEnemy[i]->m_pSkinnedAnimationController) m_ppEnemy[i]->UpdateTransform(NULL);
			m_ppEnemy[i]->Animate(m_fElapsedTime);
			m_ppEnemy[i]->Render(pd3dCommandList, pCamera);
		}
	}
	if (m_pBoss) {
		if (!m_pBoss->m_pSkinnedAnimationController)m_pBoss->UpdateTransform(NULL);
		m_pBoss->Animate(m_fElapsedTime);
		m_pBoss->Render(pd3dCommandList, pCamera);
	}

	if (m_ppPlayer) {
		for (int i = 0; i < m_nPlayer; ++i) {
			if (m_ppPlayer[i]->m_bUnable)m_ppPlayer[i]->Render(pd3dCommandList, pCamera);
		}
	}
	

}

void CScene::RenderBoundingBox(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	CMaterial::m_pBoundingBoxShader->Render(pd3dCommandList, pCamera);

	for (int i = 0; i < m_nHierarchicalGameObjects; i++)
	{
		if (m_ppHierarchicalGameObjects[i])
		{
			m_ppHierarchicalGameObjects[i]->RenderBoundingBox(pd3dCommandList);
		}
	}

	for (int i = 0; i < m_nMissionObj; i++)
	{
		if (m_ppMissionObj[i])
		{
			m_ppMissionObj[i]->RenderBoundingBox(pd3dCommandList);
		}
	}

	
	for (int i = 0; i < m_nEnemy; i++)
	{
		if (m_ppEnemy[i])
		{
			m_ppEnemy[i]->RenderBoundingBox(pd3dCommandList);
		}
	}

	if(m_pBoss)m_pBoss->RenderBoundingBox(pd3dCommandList);
	
	for (int i = 0; i < m_nPlayer; i++)	m_ppPlayer[i]->RenderBoundingBox(pd3dCommandList);
}

bool CScene::CheckObjByObjCollition(CGameObject* pBase, CGameObject* pTarget, XMFLOAT3& out)
{
	if (pBase->m_xmBoundingBox.Intersects(pTarget->m_xmBoundingBox)) {
		XMFLOAT3 corner[8];
		pTarget->m_xmBoundingBox.GetCorners(corner);

		XMVECTOR* co = new XMVECTOR[8];
		for (int i = 0; i < 8; ++i)
			co[i] = XMLoadFloat3(&corner[i]);

		if (pBase->m_xmBoundingBox.Intersects(co[0], co[1], co[2])||
			pBase->m_xmBoundingBox.Intersects(co[0], co[2], co[3])) {
			XMFLOAT3 TargetVector = Vector3::Normalize(Vector3::XMVectorToFloat3(DirectX::XMVector3Cross(co[1] - co[0], co[2] - co[0])));
			if ((TargetVector.x == out.x ) || Vector3::IsZero(out))
				out = TargetVector;
			return(true);
		}
		if (pBase->m_xmBoundingBox.Intersects(co[4], co[0], co[3])||
			pBase->m_xmBoundingBox.Intersects(co[4], co[3], co[7]) ){
			XMFLOAT3 TargetVector = Vector3::Normalize(Vector3::XMVectorToFloat3(DirectX::XMVector3Cross(co[0] - co[4], co[3] - co[4])));
			if ((TargetVector.x == out.x ) || Vector3::IsZero(out))
				out = TargetVector;
			return(true);
		}
		if (pBase->m_xmBoundingBox.Intersects(co[5], co[4], co[7])||
			pBase->m_xmBoundingBox.Intersects(co[5], co[7], co[6])) {
			XMFLOAT3 TargetVector = Vector3::Normalize(Vector3::XMVectorToFloat3(DirectX::XMVector3Cross(co[4] - co[5], co[7] - co[5])));
			if ((TargetVector.x == out.x ) || Vector3::IsZero(out))
				out = TargetVector;
			return(true);
		}

		if (pBase->m_xmBoundingBox.Intersects(co[1], co[5], co[6])||
			pBase->m_xmBoundingBox.Intersects(co[1], co[6], co[2])) {
			XMFLOAT3 TargetVector = Vector3::Normalize(Vector3::XMVectorToFloat3(DirectX::XMVector3Cross(co[5] - co[1], co[6] - co[1])));
			if ((TargetVector.x == out.x ) || Vector3::IsZero(out))
				out = TargetVector;
			return(true);
		}
	
		return(true);
	}
	if (pTarget->m_pChild && CheckObjByObjCollition(pBase, pTarget->m_pChild, out)) return(true);
	if (pTarget->m_pSibling && CheckObjByObjCollition(pBase, pTarget->m_pSibling, out)) return(true);

	else return false;
}

// 포폴용 추가부 (충돌처리)
/*
//----------------------------------------------------------------
namespace Vector3 {
	inline bool IsZero(const XMFLOAT3& v) noexcept {
		constexpr float eps = 1e-6f;
		return (std::fabs(v.x) < eps && std::fabs(v.y) < eps && std::fabs(v.z) < eps);
	}

	inline XMFLOAT3 Normalize(const XMFLOAT3& v) noexcept {
		XMVECTOR vec = XMLoadFloat3(&v);
		vec = XMVector3Normalize(vec);
		XMFLOAT3 result;
		XMStoreFloat3(&result, vec);
		return result;
	}

	inline XMFLOAT3 ToFloat3(FXMVECTOR v) noexcept {
		XMFLOAT3 out;
		XMStoreFloat3(&out, v);
		return out;
	}
}


static bool CheckFaceIntersection(const BoundingOrientedBox& base,
	const std::array<XMVECTOR, 8>& corners,
	std::array<int, 3> indices,
	XMFLOAT3& out)
{
	const auto& [i1, i2, i3] = indices;

	if (base.Intersects(corners[i1], corners[i2], corners[i3])) {
		const XMVECTOR edge1 = corners[i2] - corners[i1];
		const XMVECTOR edge2 = corners[i3] - corners[i1];
		const XMVECTOR normalVec = XMVector3Cross(edge1, edge2);

		XMFLOAT3 normal = Vector3::Normalize(Vector3::ToFloat3(normalVec));

		if (Vector3::IsZero(out) || std::fabs(normal.x - out.x) < 1e-5f)
			out = normal;

		return true;
	}
	return false;
}

bool CScene::CheckObjByObjCollition(CGameObject* pBase, CGameObject* pTarget, XMFLOAT3& out)
{
	if (!pBase || !pTarget)
		return false;

	if (pBase->m_xmBoundingBox.Intersects(pTarget->m_xmBoundingBox)) {

		// 코너 추출
		std::array<XMFLOAT3, 8> cornerPoints{};
		pTarget->m_xmBoundingBox.GetCorners(cornerPoints.data());

		std::array<XMVECTOR, 8> corners{};
		for (size_t i = 0; i < corners.size(); ++i)
			corners[i] = XMLoadFloat3(&cornerPoints[i]);

		// 각 면(삼각형 2개씩) 인덱스 정의
		constexpr std::array<std::array<int, 3>, 8> faces = { {
			{0, 1, 2}, {0, 2, 3},
			{4, 0, 3}, {4, 3, 7},
			{5, 4, 7}, {5, 7, 6},
			{1, 5, 6}, {1, 6, 2}
		} };

		for (const auto& face : faces) {
			// 닿은면 법선백터 연산
			if (CheckFaceIntersection(pBase->m_xmBoundingBox, corners, face, out))
				return true;
		}

		return true;
	}

	if (pTarget->m_pChild && CheckObjByObjCollition(pBase, pTarget->m_pChild, out))
		return true;

	if (pTarget->m_pSibling && CheckObjByObjCollition(pBase, pTarget->m_pSibling, out))
		return true;

	return false;
}
*/


bool CScene::CheckMissionBound(CGameObject* pBase, CMissonOBJ* pTarget)
{
	if (pBase->m_xmBoundingBox.Intersects(pTarget->m_xmMissionRange)) return(true);
	else return false;
}

void CreateMerged(BoundingOrientedBox& Out, const BoundingOrientedBox& b1, const BoundingOrientedBox& b2) noexcept
{
	XMVECTOR b1Center = XMLoadFloat3(&b1.Center);
	XMVECTOR b1Extents = XMLoadFloat3(&b1.Extents);

	XMVECTOR b2Center = XMLoadFloat3(&b2.Center);
	XMVECTOR b2Extents = XMLoadFloat3(&b2.Extents);

	XMVECTOR Min = XMVectorSubtract(b1Center, b1Extents);
	Min = XMVectorMin(Min, XMVectorSubtract(b2Center, b2Extents));

	XMVECTOR Max = XMVectorAdd(b1Center, b1Extents);
	Max = XMVectorMax(Max, XMVectorAdd(b2Center, b2Extents));

	assert(XMVector3LessOrEqual(Min, Max));

	XMStoreFloat3(&Out.Center, XMVectorScale(XMVectorAdd(Min, Max), 0.5f));
	XMStoreFloat3(&Out.Extents, XMVectorScale(XMVectorSubtract(Max, Min), 0.5f));

}

BoundingOrientedBox CScene::CalculateBoundingBox()
{
	BoundingOrientedBox xmBoundingBoxs = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.1f, 0.1f, 0.1f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	//if (m_nMissionObj) xmBoundingBoxs = m_ppMissionObj[0]->m_xmBoundingBox;
	//for (int i = 1; i < m_nMissionObj; i++) CreateMerged(xmBoundingBoxs, xmBoundingBoxs, m_ppMissionObj[i]->m_xmBoundingBox);
	for (int i = 0; i < m_nHierarchicalGameObjects; i++) CreateMerged(xmBoundingBoxs, xmBoundingBoxs, m_ppHierarchicalGameObjects[i]->m_xmBoundingBox);
	return(xmBoundingBoxs); 
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// @@서버코드@@서버코드@@서버코드@@서버코드@@서버코드@@서버코드@@
void CScene::InitNetwork()
{
	wcout.imbue(locale("korean"));
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		cout << "WSA START ERROR" << endl;
	}

	c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, 0, 0, 0);
	if (c_socket == INVALID_SOCKET) {
		cout << "SOCKET INIT ERROR!" << endl;
	}

	SOCKADDR_IN server_address{};
	ZeroMemory(&server_address, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORT_NUM);
	inet_pton(AF_INET, ServerIP.c_str(), &(server_address.sin_addr.s_addr));

	int ret = connect(c_socket, reinterpret_cast<SOCKADDR*>(&server_address), sizeof(server_address));
	if (ret < 0) exit(-1);

	unsigned long noblock = 1;
	ioctlsocket(c_socket, FIONBIO, &noblock);

	/*CS_LOGIN_PACKET p;
	p.size = sizeof(p);
	p.type = CS_LOGIN;
	strcpy_s(p.name, Login_Id.c_str());
	strcpy_s(p.PW, Login_PassWord.c_str());
	send_packet(&p);*/
}

void CScene::Recv_Packet()
{

	char buf[BUF_SIZE] = { 0 };
	WSABUF wsabuf{ BUF_SIZE, buf };
	DWORD recv_byte{ 0 }, recv_flag{ 0 };

	int retval = WSARecv(c_socket, &wsabuf, 1, &recv_byte, &recv_flag, nullptr, nullptr);

	if (recv_byte > 0) {
		process_data(wsabuf.buf, recv_byte);
	}
}

void CScene::send_packet(void* packet)
{
	unsigned char* p = reinterpret_cast<unsigned char*>(packet);
	size_t sent = 0;
	send(c_socket, reinterpret_cast<char*>(p), static_cast<int>(p[0]), sent);
}

void CScene::ProcessPacket(char* p)
{
}
void CScene::process_data(char* net_buf, size_t io_byte)
{
	char* ptr = net_buf;
	static size_t in_packet_size = 0;
	static size_t saved_packet_size = 0;
	static char packet_buffer[BUF_SIZE];

	while (0 != io_byte) {
		if (0 == in_packet_size) in_packet_size = ptr[0];
		if (io_byte + saved_packet_size >= in_packet_size) {
			memcpy(packet_buffer + saved_packet_size, ptr, in_packet_size - saved_packet_size);
			ProcessPacket(packet_buffer);
			ptr += in_packet_size - saved_packet_size;
			io_byte -= in_packet_size - saved_packet_size;
			in_packet_size = 0;
			saved_packet_size = 0;
		}
		else {
			memcpy(packet_buffer + saved_packet_size, ptr, io_byte);
			saved_packet_size += io_byte;
			io_byte = 0;
		}
	}

}

// @@서버코드@@서버코드@@서버코드@@서버코드@@서버코드@@서버코드@@













