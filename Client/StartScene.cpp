
#include "stdafx.h"
#include "StartScene.h"

void CStartScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, int myPlayernum)
{
	CScene::BuildObjects(pd3dDevice, pd3dCommandList, myPlayernum);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	m_pUI = new CStartSceneUI();

	//===============================//
	// SKY BOX (1)
	m_pSkyBox = NULL;


	//===============================//
	// TERRAIN
	XMFLOAT3 xmf3Scale(10.f, 10.f, 10.f);
	XMFLOAT4 xmf4Color(0.0f, 0.0f, 0.0f, 0.0f);
	m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, _T("Terrain/BaseTerrain.raw"), 257, 257, xmf3Scale, xmf4Color);
	m_pTerrain->SetPosition(-1000.f, 0, -1000.f);

	//===============================//
	m_nHierarchicalGameObjects = 0;
	m_nMissionObj = 0;
	m_nEnemy = 0;
	m_nShaders = 0;

	//===============================//
	// Player (3 / 1 - Corzim, 2 - Evan, 3 - Uranya)
	// [Present Setting]
	// 1	- Corzim (Player)
	// 2	- Evan
	// 3	- Corzim
	// Able Model - Evam, Corzim
	// Unable Model - Uranya

	//===============================//
	// Player
	m_nPlayer = 1;
	m_ppPlayer = new CPlayer * [m_nPlayer];
	m_ppModelInfoPlayer = new CLoadedModelInfo * [m_nPlayer]; // Play Character

	// 저장된 모델 바꿀 수 있음
	m_ppModelInfoPlayer[FIRST_PLAYER] = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), "Model/Player_1.bin", NULL);

	for (int i = 0; i < m_nPlayer; ++i) {
		CyborgPlayer* pPlayer = new CyborgPlayer(pd3dDevice, pd3dCommandList, GetGraphicsRootSignature(), m_pTerrain, m_ppModelInfoPlayer[i]);
		m_ppPlayer[i] = pPlayer;
		m_ppPlayer[i]->SetPlayerData(i);
	}

	m_pMyPlayer = m_ppPlayer[MY_PLAYER];
	m_pMyPlayer->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	m_pMyPlayer->ChangeCamera(THIRD_PERSON_CAMERA, 0.0f);
	m_pMyPlayer->m_bUnable = false;
}

void CStartScene::ReleaseObjects()
{
	CScene::ReleaseObjects();
}

bool CStartScene::ProcessInput(HWND m_hWnd, POINT m_ptOldCursorPos, UCHAR* pKeysBuffer)
{
	CScene::ProcessInput(m_hWnd, m_ptOldCursorPos, pKeysBuffer);

	if (pKeysBuffer[VK_SHIFT] & 0xF0) m_bCaps = true;
	else  m_bCaps = false;

	return(false);

}

std::string wcharToChar(const std::wstring& wstr) {
	// Get the length of the resulting string in bytes
	int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
	if (len == 0) {
		// Error handling if conversion fails
		throw std::runtime_error("Conversion failed");
	}

	// Allocate a buffer to hold the resulting string
	std::string str(len, '\0');

	// Perform the actual conversion
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, &str[0], len, nullptr, nullptr);

	return str;
}


bool CStartScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{

	CScene::OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);


	if (m_bInputID){
		switch (nMessageID)
		{
		case WM_KEYDOWN: {
			switch (wParam) {
			case VK_BACK:
			{
				if (!reinterpret_cast<CStartSceneUI*>(m_pUI)->m_ID.empty())
				{
					reinterpret_cast<CStartSceneUI*>(m_pUI)->m_ID.pop_back();
				}
				break;
			}
			default:
				if (reinterpret_cast<CStartSceneUI*>(m_pUI)->m_ID.size() < TextMax)
				{

					if ((wParam < 123 && wParam > 64)) {
						if (m_bCaps) reinterpret_cast<CStartSceneUI*>(m_pUI)->m_ID += wParam;
						else reinterpret_cast<CStartSceneUI*>(m_pUI)->m_ID += wParam + 32;
					}
					if ((wParam < 58 && wParam > 47))
					{
						reinterpret_cast<CStartSceneUI*>(m_pUI)->m_ID += wParam;
					}
				}
				break;
			}
		}
		case WM_KEYUP: {
			switch (wParam) {
			default:
				break;
			}
		}
		}
		
	}
	else if (m_bInputPW) {
		switch (nMessageID)
		{
		case WM_KEYDOWN: {
			switch (wParam) {
			case VK_BACK:
			{
				if (!reinterpret_cast<CStartSceneUI*>(m_pUI)->m_PW.empty())
				{
					reinterpret_cast<CStartSceneUI*>(m_pUI)->m_PW.pop_back();
				}
				break;
			}
			default:
				if (reinterpret_cast<CStartSceneUI*>(m_pUI)->m_PW.size() < TextMax)
				{
					if ((wParam < 123 && wParam > 64)) {
						if (m_bCaps) reinterpret_cast<CStartSceneUI*>(m_pUI)->m_PW += wParam;
						else reinterpret_cast<CStartSceneUI*>(m_pUI)->m_PW += wParam + 32;
					}
					if ((wParam < 58 && wParam > 47))
					{
						reinterpret_cast<CStartSceneUI*>(m_pUI)->m_PW += wParam;
					}
				}
				break;
			}
		}
		}

	}
	else {
		switch (nMessageID)
		{
		case WM_KEYDOWN: {
			switch (wParam) {
			case 'A':
			{
				if (reinterpret_cast<CStartSceneUI*>(m_pUI)->m_CheckInfo == ALL_CORRET)
				{
					std::cout << "Change Scene" << endl;
					m_bChangeScene = true;
				}
				break;
			}
			default:
				break;
			}
		}
		}
		
	}

	//상시 입력
	switch (nMessageID)
	{
	case WM_KEYDOWN: {
		switch (wParam) {
		case VK_RETURN:
		{
			reinterpret_cast<CStartSceneUI*>(m_pUI)->m_CheckInfo = ALL_CORRET;

			m_ID = wcharToChar(reinterpret_cast<CStartSceneUI*>(m_pUI)->m_ID);
			m_PW = wcharToChar(reinterpret_cast<CStartSceneUI*>(m_pUI)->m_PW);

			Login_Id = m_ID;
			Login_PassWord = m_PW;

			cout << "ID:" << m_ID.c_str() <<", PW:" <<m_PW.c_str() <<endl;
			Send_Login();
			break;
		}
		default:
			break;
		}
	}
	case WM_KEYUP: {
		switch (wParam) {
		
		default:
			break;
		}
	}

	}
	return(false);

}

bool CStartScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		::SetCapture(hWnd);
		::GetCursorPos(&m_ptOldCursorPos);

		m_bInputID = reinterpret_cast<CStartSceneUI*>(m_pUI)->m_ppTextInputBox[0]->CheckChlick(hWnd, m_ptOldCursorPos);
		m_bInputPW = reinterpret_cast<CStartSceneUI*>(m_pUI)->m_ppTextInputBox[1]->CheckChlick(hWnd, m_ptOldCursorPos);
		
		reinterpret_cast<CStartSceneUI*>(m_pUI)->m_CheckInfo = ALL_CORRET;

		m_ID = wcharToChar(reinterpret_cast<CStartSceneUI*>(m_pUI)->m_ID);
		m_PW = wcharToChar(reinterpret_cast<CStartSceneUI*>(m_pUI)->m_PW);

		Login_Id = m_ID;
		Login_PassWord = m_PW;
		
		if(reinterpret_cast<CStartSceneUI*>(m_pUI)->m_ppButton[0]->CheckChlick(hWnd, m_ptOldCursorPos))
		{
			m_bSignUP = true;
			Send_SignUp();
		}
		
		if (reinterpret_cast<CStartSceneUI*>(m_pUI)->m_ppButton[1]->CheckChlick(hWnd, m_ptOldCursorPos))
		{
			m_bLogin = true;
			Send_Login();
		}

		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		m_bSignUP = reinterpret_cast<CStartSceneUI*>(m_pUI)->m_ppButton[0]->m_bSelected = false;
		m_bLogin = reinterpret_cast<CStartSceneUI*>(m_pUI)->m_ppButton[1]->m_bSelected = false;

		::ReleaseCapture();
		break;
	case WM_MOUSEMOVE:
		break;
	default:
		break;
	}
	return(true);

}

void CStartScene::Send_Login()
{
	CS_LOGIN_PACKET p;
	p.size = sizeof(p);
	p.type = CS_LOGIN;
	strcpy_s(p.name, Login_Id.c_str());
	strcpy_s(p.PW, Login_PassWord.c_str());
	send_packet(&p);
}

void CStartScene::Send_SignUp()
{
	CS_SIGNUP_PACKET p;
	p.size = sizeof(p);
	p.type = CS_SIGNUP;
	strcpy_s(p.name, Login_Id.c_str());
	strcpy_s(p.PW, Login_PassWord.c_str());
	send_packet(&p);
	
}
