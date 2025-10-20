#pragma once

#include "Button.h"
#include "TextInput.h"
#include "TagButton.h"
#include "ProgressBar.h"

#include "Machine.h"


class CUI
{
public:
	CUI();
	~CUI();

	void Release();

public:
	static const UINT			m_nSwapChainBuffers = 2;

	ID3D11On12Device* m_pd3d11On12Device = NULL;
	ID3D11DeviceContext* m_pd3d11DeviceContext = NULL;
	ID2D1Factory3* m_pd2dFactory = NULL;
	IDWriteFactory* m_pdWriteFactory = NULL;
	ID2D1Device2* m_pd2dDevice = NULL;
	ID2D1DeviceContext2* m_pd2dDeviceContext = NULL;

	ID3D11Resource* m_ppd3d11WrappedBackBuffers[m_nSwapChainBuffers];
	ID2D1Bitmap1* m_ppd2dRenderTargets[m_nSwapChainBuffers];

	ID2D1SolidColorBrush* m_pd2dbrBackground = NULL;
	ID2D1SolidColorBrush* m_pd2dbrBorder = NULL;
	IDWriteTextFormat* m_pdwFont = NULL;
	IDWriteTextLayout* m_pdwTextLayout = NULL;
	ID2D1SolidColorBrush* m_pd2dbrText = NULL;


	//Image
	IWICImagingFactory* m_pwicImagingFactory = NULL;

	ID2D1Effect* m_pd2dfxBitmapSource = NULL;
	
	ID2D1DrawingStateBlock1* m_pd2dsbDrawingState = NULL;
	IWICFormatConverter* m_pwicFormatConverter = NULL;
	int							m_nDrawEffectImage = 0;

public:
	int m_nMissions = 3;
	float m_fMissionGauge[3] = { 0 };

	bool m_bcard = false;

	int				m_nButton = 0;
	CButton**		m_ppButton = NULL;

	int				m_nTextInputBox = 0;
	CTextInput**	m_ppTextInputBox = NULL;

public:
	void CreateDirect2DDevice(HWND m_hWnd, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12CommandQueue* m_pd3dCommandQueue, ID3D12Resource* m_ppd3dSwapChainBackBuffers[m_nSwapChainBuffers]);
	virtual void DrawUI(UINT m_nSwapChainBufferIndex);

	void LoadUIImage(const wchar_t* filename, IWICImagingFactory* pwicImagingFactory, ID2D1Effect* pd2dfxBitmapSource);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CPrepareRoomSceneUI : public CUI
{
public:
	CPrepareRoomSceneUI() {}
	~CPrepareRoomSceneUI() {}

public:

	float	top = 0.f;
	float	left = 0.f;
	float	width = 0.f;
	float	height = 0.f;
	float	gab = 0.f;


	void DrawUI(UINT m_nSwapChainBufferIndex);
	void RenderUIElements(UINT m_nSwapChainBufferIndex);

	std::wstring m_text[3];

	bool m_bPlayerOn[3] = {false};

	void CheckEnter();
	void InfoText();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
class CFirstRoundSceneUI : public CUI
{
public:
	CFirstRoundSceneUI() {}
	~CFirstRoundSceneUI() {}

public:

	float		m_fStaminaRange = 100.f;
	float		m_fMaxStamina = 100.f;
	bool		m_bStaminaBarOn = false;

	void MissionText();
	void MissionProgressBar(int MissionNum);
	void kyecardUI();
	void StaminaBarUI();

public:
	void DrawUI(UINT m_nSwapChainBufferIndex);
	void RenderUIElements(UINT m_nSwapChainBufferIndex);

};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
struct MovCircle {
	D2D1_POINT_2F position;  // 위치
	D2D1_POINT_2F velocity;  // 이동 속도
	float radius;            // 반지름
};

class CSecondRoundSceneUI : public CUI
{
public:
	CSecondRoundSceneUI();
	~CSecondRoundSceneUI();

public:

	float		m_fStaminaRange = 100.f;
	float		m_fMaxStamina = 100.f;
	bool		m_bStaminaBarOn = true;
	bool		m_bMyOn = false;


	float		m_fMissionRange[3] = { 0 };

	int					m_ntagButton = 0;
	CTagButton			**m_ppTagButton = NULL;

	std::vector<MovCircle> mCircles;

	int					m_nProgressBar = 0;
	CProgressBar**		m_ppProgressBar = NULL;

	int					m_nMachine = 0;
	CMachine**			m_ppMachine = NULL;

	bool				m_bEnd = false;

	void ItemUI();
	void StaminaBarUI();

	void BossUI();
	void MachineUI();
	void MissionUI();
	void Ending();
	void SetProgress(int progressNum, float figure) { m_fMissionRange[progressNum] = m_fMissionRange[progressNum] + figure; }


public:
	void RecMove(D2D1_RECT_F missionBox);
	void CirclesMove(D2D1_RECT_F missionBox);
	float dx = 2, dy = 2;

	float m_fRecSizeX = 85;
	float m_fRecSizeY = 85;

public:
	void DrawUI(UINT m_nSwapChainBufferIndex);
	void RenderUIElements(UINT m_nSwapChainBufferIndex);

};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
enum IDnPW_INPUT_STATE
{
	EMPTY,
	ALL_CORRET,
	ID_ERROR,
	PW_ERROR,
};

class CStartSceneUI : public CUI
{
public:
	CStartSceneUI();
	~CStartSceneUI() {}

	std::wstring			m_ID = L"testID";
	std::wstring			m_PW = L"testPW";

	IDnPW_INPUT_STATE					m_CheckInfo = EMPTY;
public:
	void DrawUI(UINT m_nSwapChainBufferIndex);
	void UISet_Small(UINT m_nSwapChainBufferIndex);
	void UISet_Full(UINT m_nSwapChainBufferIndex);


};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///

class CLoadingUI : public CUI
{
public: 
	CLoadingUI() {}
	~CLoadingUI() {}

public:
	void DrawUI(UINT m_nSwapChainBufferIndex);
	void RenderUIElements(UINT m_nSwapChainBufferIndex);

};