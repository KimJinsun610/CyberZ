//-----------------------------------------------------------------------------
// File: Scene.h
//-----------------------------------------------------------------------------

#pragma once

#include "Shader.h"
#include "CyborgPlayer.h"
#include "GUI.h"


struct LIGHT
{
	XMFLOAT4							m_xmf4Ambient;
	XMFLOAT4							m_xmf4Diffuse;
	XMFLOAT4							m_xmf4Specular;
	XMFLOAT3							m_xmf3Position;
	float 								m_fFalloff;
	XMFLOAT3							m_xmf3Direction;
	float 								m_fTheta; //cos(m_fTheta)
	XMFLOAT3							m_xmf3Attenuation;
	float								m_fPhi; //cos(m_fPhi)
	bool								m_bEnable;
	int									m_nType;
	float								m_fRange;
	float								padding;
};										
										
struct LIGHTS							
{										
	LIGHT								m_pLights[MAX_LIGHTS];
	XMFLOAT4							m_xmf4GlobalAmbient;
	int									m_nLights;
};


class CScene
{

public:
    CScene();
    ~CScene();

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseObjects();
	virtual void ReleaseShaderVariables();
	virtual void ReleaseUploadBuffers();

	virtual void BuildDefaultLightsAndMaterials();
	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int myPlayernum);
	void CreateShadowShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, bool bBoss);


	ID3D12RootSignature *CreateGraphicsRootSignature(ID3D12Device *pd3dDevice);
	ID3D12RootSignature *GetGraphicsRootSignature() { return(m_pd3dGraphicsRootSignature); }

	//Input
	virtual bool ProcessInput(HWND m_hWnd, POINT m_ptOldCursorPos, UCHAR* pKeysBuffer);
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	//Onject Update & Animation
	virtual void AnimateObjects(float fTimeElapsed);
	
	// Render
	void OnPrepareRender(ID3D12GraphicsCommandList* pd3dCommandList);
	void OnPreRender(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);
	void OtherRender(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera=NULL);


	//bounding Box
	virtual void RenderBoundingBox(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);
	bool CheckObjByObjCollition(CGameObject* pBase, CGameObject* pTarget, XMFLOAT3& out);
	bool CheckMissionBound(CGameObject* pBase, CMissonOBJ* pTarget);
	BoundingOrientedBox CalculateBoundingBox();

public:
	// @@서버코드@@서버코드@@
	void InitNetwork();
	virtual void Recv_Packet();
	virtual void send_packet(void* packet);
	virtual void ProcessPacket(char* p) = 0;
	virtual void process_data(char* net_buf, size_t io_byte);

	// @@서버코드@@서버코드@@


	CPlayer**				m_ppPlayer = NULL;				// 모든 플레이어 정보
	CPlayer*				m_pMyPlayer = NULL;				// 현재 플레이어 정보
	int						m_nPlayer;						// 플레이어 갯수

	CLoadedModelInfo**		m_ppModelInfoPlayer = NULL;		// Model Data

protected:
	ID3D12RootSignature*				m_pd3dGraphicsRootSignature = NULL;

	static ID3D12DescriptorHeap*		m_pd3dCbvSrvDescriptorHeap;

	static D3D12_CPU_DESCRIPTOR_HANDLE	m_d3dCbvCPUDescriptorStartHandle;
	static D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dCbvGPUDescriptorStartHandle;
	static D3D12_CPU_DESCRIPTOR_HANDLE	m_d3dSrvCPUDescriptorStartHandle;
	static D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dSrvGPUDescriptorStartHandle;

	static D3D12_CPU_DESCRIPTOR_HANDLE	m_d3dCbvCPUDescriptorNextHandle;
	static D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dCbvGPUDescriptorNextHandle;
	static D3D12_CPU_DESCRIPTOR_HANDLE	m_d3dSrvCPUDescriptorNextHandle;
	static D3D12_GPU_DESCRIPTOR_HANDLE	m_d3dSrvGPUDescriptorNextHandle;


public:
	static void CreateCbvSrvDescriptorHeaps(ID3D12Device *pd3dDevice, int nConstantBufferViews, int nShaderResourceViews);

	static D3D12_GPU_DESCRIPTOR_HANDLE CreateConstantBufferViews(ID3D12Device *pd3dDevice, int nConstantBufferViews, ID3D12Resource *pd3dConstantBuffers, UINT nStride);
	static D3D12_GPU_DESCRIPTOR_HANDLE CreateConstantBufferView(ID3D12Device* pd3dDevice, ID3D12Resource* pd3dConstantBuffer, UINT nStride);
	static D3D12_GPU_DESCRIPTOR_HANDLE CScene::CreateShaderResourceView(ID3D12Device* pd3dDevice, ID3D12Resource* pd3dResource, DXGI_FORMAT dxgiSrvFormat);
	static void CreateShaderResourceViews(ID3D12Device* pd3dDevice, CTexture* pTexture, UINT nDescriptorHeapIndex, UINT nRootParameterStartIndex);

	static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUCbvDescriptorStartHandle() { return(m_d3dCbvCPUDescriptorStartHandle); }
	static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUCbvDescriptorStartHandle() { return(m_d3dCbvGPUDescriptorStartHandle); }
	static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUSrvDescriptorStartHandle() { return(m_d3dSrvCPUDescriptorStartHandle); }
	static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUSrvDescriptorStartHandle() { return(m_d3dSrvGPUDescriptorStartHandle); }

	static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUCbvDescriptorNextHandle() { return(m_d3dCbvCPUDescriptorNextHandle); }
	static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUCbvDescriptorNextHandle() { return(m_d3dCbvGPUDescriptorNextHandle); }
	static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUSrvDescriptorNextHandle() { return(m_d3dSrvCPUDescriptorNextHandle); }
	static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUSrvDescriptorNextHandle() { return(m_d3dSrvGPUDescriptorNextHandle); }

	float								m_fElapsedTime = 0.0f;
	XMFLOAT3							m_xmf3RotatePosition = XMFLOAT3(0.0f, 0.0f, 0.0f);

public:
	int									m_nHierarchicalGameObjects = 0;
	CGameObject**						m_ppHierarchicalGameObjects = NULL;

	int									m_nMissionObj = 0;
	CMissonOBJ**						m_ppMissionObj = NULL;

	int									m_nEnemy = 0;
	CGameObject**						m_ppEnemy = NULL;

	CGameObject*						m_pBoss = NULL;

	int									m_nShaders = 0;
	CShader**							m_ppShaders = NULL;

	CSkyBox*							m_pSkyBox = NULL;
	CHeightMapTerrain*					m_pTerrain = NULL;
	
	CUI*								m_pUI = NULL;
	bool								m_bUIOn = true;
	bool								m_bCaps = false;

	bool								m_bChangeScene = false;
	bool								Missionflag = false;

	LIGHT*								m_pLights = NULL;
	int									m_nLights = 0;

	XMFLOAT4							m_xmf4GlobalAmbient;

	ID3D12Resource*						m_pd3dcbLights = NULL;
	LIGHTS*								m_pcbMappedLights = NULL;

	DWORD								m_dwLastDirection;

	CDepthRenderShader*					m_pDepthRenderShader = NULL;

	int									m_nFloorObj = 0;
	CGameObject**						m_ppFloorObj = NULL;

	int m_nParticleObj = 0;
	CParticle** m_ppParticleObj = NULL;

public:
	virtual bool AllPlayerReady() { return false; }
	virtual int GetModelInfo() { return 0; }
};

