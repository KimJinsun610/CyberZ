
#include "stdafx.h"
#include "GUI.h"


CUI::CUI()
{
}

CUI::~CUI()
{
	
}

void CUI::Release()
{
	if (m_pd2dbrBackground) m_pd2dbrBackground->Release();
	if (m_pd2dbrBorder) m_pd2dbrBorder->Release();
	if (m_pdwFont) m_pdwFont->Release();
	if (m_pdwTextLayout) m_pdwTextLayout->Release();
	if (m_pd2dbrText) m_pd2dbrText->Release();

	if (m_pd2dDeviceContext) m_pd2dDeviceContext->Release();
	if (m_pd2dDevice) m_pd2dDevice->Release();
	if (m_pdWriteFactory) m_pdWriteFactory->Release();
	if (m_pd3d11On12Device) m_pd3d11On12Device->Release();
	if (m_pd3d11DeviceContext) m_pd3d11DeviceContext->Release();
	if (m_pd2dFactory) m_pd2dFactory->Release();

	for (int i = 0; i < m_nSwapChainBuffers; i++)
	{
		if (m_ppd3d11WrappedBackBuffers[i]) m_ppd3d11WrappedBackBuffers[i]->Release();
		if (m_ppd2dRenderTargets[i]) m_ppd2dRenderTargets[i]->Release();
	}

	// Asset
	if (m_ppButton)
	{
		for (int i = 0; i < m_nButton; i++) if (m_ppButton[i]) m_ppButton[i]->Release();
		delete[] m_ppButton;
	}

	if (m_ppTextInputBox)
	{
		for (int i = 0; i < m_nTextInputBox; i++) if (m_ppTextInputBox[i]) m_ppTextInputBox[i]->Release();
		delete[] m_ppTextInputBox;
	}

	//Image
	if (m_pd2dfxBitmapSource) m_pd2dfxBitmapSource->Release();
	if (m_pd2dsbDrawingState) m_pd2dsbDrawingState->Release();
	if (m_pwicFormatConverter) m_pwicFormatConverter->Release();
	if (m_pwicImagingFactory) m_pwicImagingFactory->Release();
}

void CUI::CreateDirect2DDevice(HWND m_hWnd, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12CommandQueue* m_pd3dCommandQueue, ID3D12Resource* m_ppd3dSwapChainBackBuffers[m_nSwapChainBuffers])
{
	UINT nD3D11DeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(_DEBUG) || defined(DBG)
	nD3D11DeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	ID3D11Device* pd3d11Device = NULL;
	ID3D12CommandQueue* ppd3dCommandQueues[] = { m_pd3dCommandQueue };
	HRESULT hResult = ::D3D11On12CreateDevice(pd3dDevice, nD3D11DeviceFlags, NULL, 0, reinterpret_cast<IUnknown**>(ppd3dCommandQueues), _countof(ppd3dCommandQueues), 0, &pd3d11Device, &m_pd3d11DeviceContext, NULL);
	hResult = pd3d11Device->QueryInterface(__uuidof(ID3D11On12Device), (void**)&m_pd3d11On12Device);
	if (pd3d11Device) pd3d11Device->Release();

	D2D1_FACTORY_OPTIONS nD2DFactoryOptions = { D2D1_DEBUG_LEVEL_NONE };
#if defined(_DEBUG) || defined(DBG)
	nD2DFactoryOptions.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
	ID3D12InfoQueue* pd3dInfoQueue = NULL;
	if (SUCCEEDED(pd3dDevice->QueryInterface(IID_PPV_ARGS(&pd3dInfoQueue))))
	{
		D3D12_MESSAGE_SEVERITY pd3dSeverities[] =
		{
			D3D12_MESSAGE_SEVERITY_INFO,
		};

		D3D12_MESSAGE_ID pd3dDenyIds[] =
		{
			D3D12_MESSAGE_ID_INVALID_DESCRIPTOR_HANDLE,
		};

		D3D12_INFO_QUEUE_FILTER d3dInforQueueFilter = { };
		d3dInforQueueFilter.DenyList.NumSeverities = _countof(pd3dSeverities);
		d3dInforQueueFilter.DenyList.pSeverityList = pd3dSeverities;
		d3dInforQueueFilter.DenyList.NumIDs = _countof(pd3dDenyIds);
		d3dInforQueueFilter.DenyList.pIDList = pd3dDenyIds;

		pd3dInfoQueue->PushStorageFilter(&d3dInforQueueFilter);
	}
	pd3dInfoQueue->Release();
#endif

	hResult = ::D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory3), &nD2DFactoryOptions, (void**)&m_pd2dFactory);

	IDXGIDevice* pdxgiDevice = NULL;
	hResult = m_pd3d11On12Device->QueryInterface(__uuidof(IDXGIDevice), (void**)&pdxgiDevice);
	hResult = m_pd2dFactory->CreateDevice(pdxgiDevice, &m_pd2dDevice);
	hResult = m_pd2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &m_pd2dDeviceContext);
	hResult = ::DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)&m_pdWriteFactory);
	if (pdxgiDevice) pdxgiDevice->Release();

	m_pd2dDeviceContext->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

	m_pd2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(0.3f, 0.0f, 0.0f, 0.5f), &m_pd2dbrBackground);
	m_pd2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White, 1.0f), &m_pd2dbrBorder);

	hResult = m_pdWriteFactory->CreateTextFormat(L"ComicSans", NULL, DWRITE_FONT_WEIGHT_DEMI_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 30.0f, L"en-US", &m_pdwFont);
	hResult = m_pdwFont->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	hResult = m_pdwFont->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	m_pd2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White, 1.0f), &m_pd2dbrText);
	hResult = m_pdWriteFactory->CreateTextLayout(L"텍스트 레이아웃", 8, m_pdwFont, 4096.0f, 4096.0f, &m_pdwTextLayout);

	float fDpi = (float)GetDpiForWindow(m_hWnd);
	D2D1_BITMAP_PROPERTIES1 d2dBitmapProperties = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW, D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED), fDpi, fDpi);

	for (UINT i = 0; i < m_nSwapChainBuffers; i++)
	{
		D3D11_RESOURCE_FLAGS d3d11Flags = { D3D11_BIND_RENDER_TARGET };
		m_pd3d11On12Device->CreateWrappedResource(m_ppd3dSwapChainBackBuffers[i], &d3d11Flags, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT, IID_PPV_ARGS(&m_ppd3d11WrappedBackBuffers[i]));
		IDXGISurface* pdxgiSurface = NULL;
		m_ppd3d11WrappedBackBuffers[i]->QueryInterface(__uuidof(IDXGISurface), (void**)&pdxgiSurface);
		m_pd2dDeviceContext->CreateBitmapFromDxgiSurface(pdxgiSurface, &d2dBitmapProperties, &m_ppd2dRenderTargets[i]);
		if (pdxgiSurface) pdxgiSurface->Release();
	}


	// Image Render
	CoInitialize(NULL);
	hResult = ::CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, __uuidof(IWICImagingFactory), (void**)&m_pwicImagingFactory);

	hResult = m_pd2dFactory->CreateDrawingStateBlock(&m_pd2dsbDrawingState);
	hResult = m_pd2dDeviceContext->CreateEffect(CLSID_D2D1BitmapSource, &m_pd2dfxBitmapSource);
	
	IWICBitmapDecoder* pwicBitmapDecoder;
	hResult = m_pwicImagingFactory->CreateDecoderFromFilename(L"Image/CardKey.png", NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &pwicBitmapDecoder);
	IWICBitmapFrameDecode* pwicFrameDecode;
	pwicBitmapDecoder->GetFrame(0, &pwicFrameDecode);
	m_pwicImagingFactory->CreateFormatConverter(&m_pwicFormatConverter);
	m_pwicFormatConverter->Initialize(pwicFrameDecode, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeCustom);
	m_pd2dfxBitmapSource->SetValue(D2D1_BITMAPSOURCE_PROP_WIC_BITMAP_SOURCE, m_pwicFormatConverter);

	m_nDrawEffectImage = 0;

	if (pwicBitmapDecoder) pwicBitmapDecoder->Release();
	if (pwicFrameDecode) pwicFrameDecode->Release();
}

void CUI::LoadUIImage(const wchar_t* filename, IWICImagingFactory* pwicImagingFactory, ID2D1Effect* pd2dfxBitmapSource)
{
	IWICBitmapDecoder* pwicBitmapDecoder;
	HRESULT hResult = pwicImagingFactory->CreateDecoderFromFilename(filename, NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &pwicBitmapDecoder);
	if (FAILED(hResult)) return;  // 오류 처리 추가

	IWICBitmapFrameDecode* pwicFrameDecode;
	hResult = pwicBitmapDecoder->GetFrame(0, &pwicFrameDecode);
	if (FAILED(hResult)) return;  // 오류 처리 추가

	IWICFormatConverter* pwicFormatConverter;
	hResult = pwicImagingFactory->CreateFormatConverter(&pwicFormatConverter);
	if (FAILED(hResult)) return;  // 오류 처리 추가

	hResult = pwicFormatConverter->Initialize(pwicFrameDecode, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeCustom);
	if (FAILED(hResult)) return;  // 오류 처리 추가

	// pd2dfxBitmapSource의 입력 비트맵을 새로운 이미지로 업데이트
	pd2dfxBitmapSource->SetValue(D2D1_BITMAPSOURCE_PROP_WIC_BITMAP_SOURCE, pwicFormatConverter);

	// 해제
	pwicFormatConverter->Release();
	pwicFrameDecode->Release();
	pwicBitmapDecoder->Release();
}

void CUI::DrawUI(UINT m_nSwapChainBufferIndex)
{
	//Direct2D Drawing

	m_pd2dDeviceContext->SetTarget(m_ppd2dRenderTargets[m_nSwapChainBufferIndex]);
	ID3D11Resource* ppd3dResources[] = { m_ppd3d11WrappedBackBuffers[m_nSwapChainBufferIndex] };
	m_pd3d11On12Device->AcquireWrappedResources(ppd3dResources, _countof(ppd3dResources));

	m_pd2dDeviceContext->BeginDraw();

	m_pd2dDeviceContext->SetTransform(D2D1::Matrix3x2F::Identity());

	//-----------------------------

	D2D1_SIZE_F szRenderTarget = m_ppd2dRenderTargets[m_nSwapChainBufferIndex]->GetSize();

	D2D1_RECT_F rcUpperText = D2D1::RectF(0, 0, szRenderTarget.width, szRenderTarget.height * 0.25f);
	m_pd2dDeviceContext->DrawTextW(L"testUI", (UINT32)wcslen(L"testUI"), m_pdwFont, &rcUpperText, m_pd2dbrText);

	m_pd2dDeviceContext->EndDraw();

	//-----------------------------

	m_pd3d11On12Device->ReleaseWrappedResources(ppd3dResources, _countof(ppd3dResources));
	m_pd3d11DeviceContext->Flush();

}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void CPrepareRoomSceneUI::DrawUI(UINT m_nSwapChainBufferIndex)
{

	m_pd2dDeviceContext->SetTarget(m_ppd2dRenderTargets[m_nSwapChainBufferIndex]);
	ID3D11Resource* ppd3dResources[] = { m_ppd3d11WrappedBackBuffers[m_nSwapChainBufferIndex] };
	m_pd3d11On12Device->AcquireWrappedResources(ppd3dResources, _countof(ppd3dResources));

	m_pd2dDeviceContext->BeginDraw();
	m_pd2dDeviceContext->SetTransform(D2D1::Matrix3x2F::Identity());

	//Direct2D Drawing
	RenderUIElements(m_nSwapChainBufferIndex);

	m_pd2dDeviceContext->EndDraw();
	m_pd3d11On12Device->ReleaseWrappedResources(ppd3dResources, _countof(ppd3dResources));
	m_pd3d11DeviceContext->Flush();
}

void CPrepareRoomSceneUI::RenderUIElements(UINT m_nSwapChainBufferIndex)
{
	D2D1_SIZE_F szRenderTarget = m_ppd2dRenderTargets[m_nSwapChainBufferIndex]->GetSize();

	CheckEnter();
	InfoText();
}

void CPrepareRoomSceneUI::CheckEnter()
{
	
#ifdef SMALL_WINDOW_SCREEN
	//Small window
	m_pdWriteFactory->CreateTextFormat(L"ComicSans", NULL, DWRITE_FONT_WEIGHT_DEMI_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 20.0f, L"en-US", &m_pdwFont);
	
	top = FRAME_BUFFER_HEIGHT / 3 + 130;
	left = FRAME_BUFFER_WIDTH / 3 - 45;
	width = 80;
	height = 10;
	gab = 220;

#else
	m_pdWriteFactory->CreateTextFormat(L"ComicSans", NULL, DWRITE_FONT_WEIGHT_DEMI_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 40.0f, L"en-US", &m_pdwFont);

	top = FRAME_BUFFER_HEIGHT / 3 + 200;
	left = FRAME_BUFFER_WIDTH / 3 - 90;
	width = 180;
	height = 10;
	gab = 325;

#endif // SMALL_WINDOW_SCREEN

	
	if (m_bPlayerOn[0])
	{
		// WCHAR InfoText[] = L"Player 1";
		D2D1_RECT_F rcPlayer0 = D2D1::RectF(left, top, left + width, top + height);
		m_pd2dDeviceContext->DrawTextW(m_text[0].c_str(), static_cast<UINT32>(m_text[0].length()), m_pdwFont, &rcPlayer0, m_pd2dbrText);
	}

	if (m_bPlayerOn[1])
	{
		// WCHAR InfoText1[] = L"Player 2";
		D2D1_RECT_F rcPlayer1 = D2D1::RectF(left + gab, top, left + width + gab, top + height);
		m_pd2dDeviceContext->DrawTextW(m_text[1].c_str(), static_cast<UINT32>(m_text[1].length()), m_pdwFont, &rcPlayer1, m_pd2dbrText);
	}

	if (m_bPlayerOn[2])
	{
		D2D1_RECT_F rcPlayer2 = D2D1::RectF(left + gab * 2, top, left + width + gab * 2, top + height);
		m_pd2dDeviceContext->DrawTextW(m_text[2].c_str(), static_cast<UINT32>(m_text[2].length()), m_pdwFont, &rcPlayer2, m_pd2dbrText);
	}

}

void CPrepareRoomSceneUI::InfoText()
{
	m_pdwFont->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	m_pdwFont->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

#ifdef SMALL_WINDOW_SCREEN

	m_pdWriteFactory->CreateTextFormat(L"ComicSans", NULL, DWRITE_FONT_WEIGHT_DEMI_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 25.0f, L"en-US", &m_pdwFont);
	top = 12;
	left = FRAME_BUFFER_WIDTH / 2 - 300;
	width = 1000;
	height = 20;
#else
	m_pdWriteFactory->CreateTextFormat(L"ComicSans", NULL, DWRITE_FONT_WEIGHT_DEMI_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 40.0f, L"en-US", &m_pdwFont);

	top = 23;
	left = FRAME_BUFFER_WIDTH / 2 - 480;
	width = 1400;
	height = 55;

#endif // SMALL_WINDOW_SCREEN

	WCHAR selectText2[] = L"캐릭터를 선택해 주세요. / 플레이어1 R키 입력시 시작";
	D2D1_RECT_F rcPlayer2 = D2D1::RectF(left, top, left + width, top + height);
	m_pd2dDeviceContext->DrawTextW(selectText2, (UINT32)wcslen(selectText2), m_pdwFont, &rcPlayer2, m_pd2dbrText);


}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//========================================================================================//
// First Round

void CFirstRoundSceneUI::DrawUI(UINT m_nSwapChainBufferIndex)
{
	m_pd2dDeviceContext->SetTarget(m_ppd2dRenderTargets[m_nSwapChainBufferIndex]);
	ID3D11Resource* ppd3dResources[] = { m_ppd3d11WrappedBackBuffers[m_nSwapChainBufferIndex] };
	m_pd3d11On12Device->AcquireWrappedResources(ppd3dResources, _countof(ppd3dResources));

	m_pd2dDeviceContext->BeginDraw();
	m_pd2dDeviceContext->SetTransform(D2D1::Matrix3x2F::Identity());

	//Direct2D Drawing
	RenderUIElements(m_nSwapChainBufferIndex);

	m_pd2dDeviceContext->EndDraw();
	m_pd3d11On12Device->ReleaseWrappedResources(ppd3dResources, _countof(ppd3dResources));
	m_pd3d11DeviceContext->Flush();

}

void CFirstRoundSceneUI::RenderUIElements(UINT m_nSwapChainBufferIndex)
{
	D2D1_SIZE_F szRenderTarget = m_ppd2dRenderTargets[m_nSwapChainBufferIndex]->GetSize();

	MissionText();
	for (int i = 0; i < 3; i++)	MissionProgressBar(i);
	if (m_bStaminaBarOn) StaminaBarUI();
	kyecardUI();
	
}

void CFirstRoundSceneUI::MissionText()
{ 
	float gab = 35.5;

	D2D1_RECT_F rcUpperText = D2D1::RectF(100.f, 30.f, 300.f, 60.f);
	WCHAR MissionText[] = L"Area 1";
	m_pd2dbrText->SetColor(D2D1::ColorF(D2D1::ColorF::White, 1.0f));
	m_pd2dDeviceContext->DrawTextW(MissionText, (UINT32)wcslen(MissionText), m_pdwFont, &rcUpperText, m_pd2dbrText);

	D2D1_RECT_F rcUpperText2 = D2D1::RectF(100.f, 30.f + (gab * 1), 300.f, 60.f + (gab * 1));
	WCHAR MissionText1[] = L"Area 2";
	m_pd2dbrText->SetColor(D2D1::ColorF(D2D1::ColorF::White, 1.0f));
	m_pd2dDeviceContext->DrawTextW(MissionText1, (UINT32)wcslen(MissionText1), m_pdwFont, &rcUpperText2, m_pd2dbrText);

	D2D1_RECT_F rcUpperText3 = D2D1::RectF(100.f, 30.f + (gab * 2), 300.f, 60.f + (gab * 2));
	WCHAR MissionText2[] = L"Area 3";
	m_pd2dbrText->SetColor(D2D1::ColorF(D2D1::ColorF::White, 1.0f));
	m_pd2dDeviceContext->DrawTextW(MissionText2, (UINT32)wcslen(MissionText2), m_pdwFont, &rcUpperText3, m_pd2dbrText);

}

void CFirstRoundSceneUI::MissionProgressBar(int MissionNum)
{
	float gab = 35.5;

	float width = 30.5;
	float height = 30.f;

	//게이지 바
	D2D1_RECT_F* rcMissionBar;
	rcMissionBar = new D2D1_RECT_F;
	rcMissionBar->top = 30.5f + (MissionNum * gab);
	rcMissionBar->left = 30.f;
	rcMissionBar->right = 30.f + m_fMissionGauge[MissionNum];
	rcMissionBar->bottom = 59.5f + (MissionNum * gab);

	m_pd2dbrBorder->SetColor(D2D1::ColorF(0x00ff00, 0.6f));
	m_pd2dDeviceContext->FillRectangle(rcMissionBar, m_pd2dbrBorder);

	// 게이지 바 프레임
	D2D1_RECT_F* rcMissionBarFrame;
	rcMissionBarFrame = new D2D1_RECT_F;
	rcMissionBarFrame->top = 30.f + (MissionNum * gab);
	rcMissionBarFrame->left = 30.0f;
	rcMissionBarFrame->right = 400.f;
	rcMissionBarFrame->bottom = 60.0f + (MissionNum * gab);

	m_pd2dbrBorder->SetColor(D2D1::ColorF(D2D1::ColorF::AliceBlue, 1.0f));
	m_pd2dDeviceContext->DrawRectangle(rcMissionBarFrame, m_pd2dbrBorder);

	delete rcMissionBar;
	delete rcMissionBarFrame;

}

void CFirstRoundSceneUI::kyecardUI()
{

	D2D1_RECT_F rcKyeCardRect = { 0.0f, 0.0f, 200.0f, 130.0f };
	D2D_POINT_2F lKeyCard = { 15.f, 130.f };

	if (m_bcard) LoadUIImage(L"Image/CardKey.png", m_pwicImagingFactory, m_pd2dfxBitmapSource);
	else LoadUIImage(L"Image/Card_off.png", m_pwicImagingFactory, m_pd2dfxBitmapSource);
	
	m_pd2dDeviceContext->DrawImage(m_pd2dfxBitmapSource, &lKeyCard, &rcKyeCardRect);

}

void CFirstRoundSceneUI::StaminaBarUI()
{
	float halfsize = m_fMaxStamina / 2;


#ifdef SMALL_WINDOW_SCREEN
	//게이지 바
	D2D1_RECT_F* rcStaminaBar;
	rcStaminaBar = new D2D1_RECT_F;
	rcStaminaBar->top = FRAME_BUFFER_HEIGHT - 90.f;
	rcStaminaBar->left = FRAME_BUFFER_WIDTH - 240.0f - m_fStaminaRange * 3;
	rcStaminaBar->right = FRAME_BUFFER_WIDTH - 240.0f;
	rcStaminaBar->bottom = FRAME_BUFFER_HEIGHT - 40.0f;

	m_pd2dbrBorder->SetColor(D2D1::ColorF(D2D1::ColorF::LightGoldenrodYellow, 1.0f));
	m_pd2dDeviceContext->FillRectangle(rcStaminaBar, m_pd2dbrBorder);

	// 게이지 바 프레임
	D2D1_RECT_F* rcStaminaBarFrame;
	rcStaminaBarFrame = new D2D1_RECT_F;
	rcStaminaBarFrame->top = FRAME_BUFFER_HEIGHT - 90.f;
	rcStaminaBarFrame->left = FRAME_BUFFER_WIDTH - 240.0f - m_fMaxStamina * 3;
	rcStaminaBarFrame->right = FRAME_BUFFER_WIDTH - 240.0f;
	rcStaminaBarFrame->bottom = FRAME_BUFFER_HEIGHT - 40.0f;

	m_pd2dbrBorder->SetColor(D2D1::ColorF(D2D1::ColorF::AliceBlue, 1.0f));
	m_pd2dDeviceContext->DrawRectangle(rcStaminaBarFrame, m_pd2dbrBorder);

#else

	//게이지 바
	D2D1_RECT_F* rcStaminaBar;
	rcStaminaBar = new D2D1_RECT_F;
	rcStaminaBar->top = FRAME_BUFFER_HEIGHT - 90.f;
	rcStaminaBar->left = FRAME_BUFFER_WIDTH - 240.0f - m_fStaminaRange*3;
	rcStaminaBar->right = FRAME_BUFFER_WIDTH - 240.0f ;
	rcStaminaBar->bottom = FRAME_BUFFER_HEIGHT - 40.0f;

	m_pd2dbrBorder->SetColor(D2D1::ColorF(D2D1::ColorF::LightGoldenrodYellow, 1.0f));
	m_pd2dDeviceContext->FillRectangle(rcStaminaBar, m_pd2dbrBorder);

	// 게이지 바 프레임
	D2D1_RECT_F* rcStaminaBarFrame;
	rcStaminaBarFrame = new D2D1_RECT_F;
	rcStaminaBarFrame->top = FRAME_BUFFER_HEIGHT - 90.f;
	rcStaminaBarFrame->left = FRAME_BUFFER_WIDTH - 240.0f - m_fMaxStamina*3;
	rcStaminaBarFrame->right = FRAME_BUFFER_WIDTH - 240.0f;
	rcStaminaBarFrame->bottom = FRAME_BUFFER_HEIGHT - 40.0f;

	m_pd2dbrBorder->SetColor(D2D1::ColorF(D2D1::ColorF::AliceBlue, 1.0f));
	m_pd2dDeviceContext->DrawRectangle(rcStaminaBarFrame, m_pd2dbrBorder);

#endif //SMALL_WINDOW_SCREEN

	delete rcStaminaBar;
	delete rcStaminaBarFrame;

}


//========================================================================================//
// Second Round

CSecondRoundSceneUI::CSecondRoundSceneUI()
{
	m_ntagButton = 1;
	m_ppTagButton = new CTagButton * [m_ntagButton];

	m_ppTagButton[0] = new CTagButton(900, 500, 0, 0);

	m_nProgressBar = 3;
	m_ppProgressBar = new CProgressBar * [m_nProgressBar];
	for (int i = 0; i < m_nProgressBar; i++) m_ppProgressBar[i] = new CProgressBar();

	m_nMachine = 5;
	m_ppMachine = new CMachine * [m_nMachine];
	for (int i = 0; i < m_nMachine; i++) m_ppMachine[i] = new CMachine();
}

CSecondRoundSceneUI::~CSecondRoundSceneUI()
{
	delete[] m_ppTagButton;
	delete[] m_ppProgressBar;
	delete[] m_ppMachine;
}

void CSecondRoundSceneUI::DrawUI(UINT m_nSwapChainBufferIndex)
{
	m_pd2dDeviceContext->SetTarget(m_ppd2dRenderTargets[m_nSwapChainBufferIndex]);
	ID3D11Resource* ppd3dResources[] = { m_ppd3d11WrappedBackBuffers[m_nSwapChainBufferIndex] };
	m_pd3d11On12Device->AcquireWrappedResources(ppd3dResources, _countof(ppd3dResources));

	m_pd2dDeviceContext->BeginDraw();
	m_pd2dDeviceContext->SetTransform(D2D1::Matrix3x2F::Identity());

	//Direct2D Drawing
	RenderUIElements(m_nSwapChainBufferIndex);

	m_pd2dDeviceContext->EndDraw();
	m_pd3d11On12Device->ReleaseWrappedResources(ppd3dResources, _countof(ppd3dResources));
	m_pd3d11DeviceContext->Flush();
}

void CSecondRoundSceneUI::RenderUIElements(UINT m_nSwapChainBufferIndex)
{
	D2D1_SIZE_F szRenderTarget = m_ppd2dRenderTargets[m_nSwapChainBufferIndex]->GetSize();

	if (m_bStaminaBarOn) StaminaBarUI();
	BossUI();
	MachineUI();

	if (m_bMyOn) MissionUI();
	if (m_bEnd) Ending();
}


void CSecondRoundSceneUI::ItemUI()
{
	D2D1_RECT_F* rcItemFrame;
	rcItemFrame = new D2D1_RECT_F;
	rcItemFrame->top = FRAME_BUFFER_HEIGHT - 200.f;
	rcItemFrame->left = FRAME_BUFFER_WIDTH - 270.0f;
	rcItemFrame->right = FRAME_BUFFER_WIDTH - 50.f;
	rcItemFrame->bottom = FRAME_BUFFER_HEIGHT - 50.0f;

	m_pd2dbrBorder->SetColor(D2D1::ColorF(D2D1::ColorF::GreenYellow, 1.0f));
	m_pd2dDeviceContext->DrawRectangle(rcItemFrame, m_pd2dbrBorder);

	delete rcItemFrame;

	float left = 1660.f;
	float top = 880.f;
	float gab = 100.f;

	D2D1_RECT_F rcUpperText = D2D1::RectF(left, top, left + gab, top + gab);
	WCHAR MissionText[] = L"ITEM";
	m_pdWriteFactory->CreateTextFormat(L"Arial", NULL, DWRITE_FONT_WEIGHT_DEMI_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 30.f, L"en-US", &m_pdwFont);
	m_pd2dbrText->SetColor(D2D1::ColorF(D2D1::ColorF::White, 1.0f));
	m_pd2dDeviceContext->DrawTextW(MissionText, (UINT32)wcslen(MissionText), m_pdwFont, &rcUpperText, m_pd2dbrText);

}

void CSecondRoundSceneUI::StaminaBarUI()
{

#ifdef SMALL_WINDOW_SCREEN
	//게이지 바
	D2D1_RECT_F* rcStaminaBar;
	rcStaminaBar = new D2D1_RECT_F;
	rcStaminaBar->top = FRAME_BUFFER_HEIGHT - 90.f;
	rcStaminaBar->left = FRAME_BUFFER_WIDTH - 240.0f - m_fStaminaRange * 3;
	rcStaminaBar->right = FRAME_BUFFER_WIDTH - 240.0f;
	rcStaminaBar->bottom = FRAME_BUFFER_HEIGHT - 40.0f;

	m_pd2dbrBorder->SetColor(D2D1::ColorF(D2D1::ColorF::LightGoldenrodYellow, 1.0f));
	m_pd2dDeviceContext->FillRectangle(rcStaminaBar, m_pd2dbrBorder);

	// 게이지 바 프레임
	D2D1_RECT_F* rcStaminaBarFrame;
	rcStaminaBarFrame = new D2D1_RECT_F;
	rcStaminaBarFrame->top = FRAME_BUFFER_HEIGHT - 90.f;
	rcStaminaBarFrame->left = FRAME_BUFFER_WIDTH - 240.0f - m_fMaxStamina * 3;
	rcStaminaBarFrame->right = FRAME_BUFFER_WIDTH - 240.0f;
	rcStaminaBarFrame->bottom = FRAME_BUFFER_HEIGHT - 40.0f;

	m_pd2dbrBorder->SetColor(D2D1::ColorF(D2D1::ColorF::AliceBlue, 1.0f));
	m_pd2dDeviceContext->DrawRectangle(rcStaminaBarFrame, m_pd2dbrBorder);

#else

	//게이지 바
	D2D1_RECT_F* rcStaminaBar;
	rcStaminaBar = new D2D1_RECT_F;
	rcStaminaBar->top = FRAME_BUFFER_HEIGHT - 90.f;
	rcStaminaBar->left = FRAME_BUFFER_WIDTH - 240.0f - m_fStaminaRange * 3;
	rcStaminaBar->right = FRAME_BUFFER_WIDTH - 240.0f;
	rcStaminaBar->bottom = FRAME_BUFFER_HEIGHT - 40.0f;

	m_pd2dbrBorder->SetColor(D2D1::ColorF(D2D1::ColorF::LightGoldenrodYellow, 1.0f));
	m_pd2dDeviceContext->FillRectangle(rcStaminaBar, m_pd2dbrBorder);

	// 게이지 바 프레임
	D2D1_RECT_F* rcStaminaBarFrame;
	rcStaminaBarFrame = new D2D1_RECT_F;
	rcStaminaBarFrame->top = FRAME_BUFFER_HEIGHT - 90.f;
	rcStaminaBarFrame->left = FRAME_BUFFER_WIDTH - 240.0f - m_fMaxStamina * 3;
	rcStaminaBarFrame->right = FRAME_BUFFER_WIDTH - 240.0f;
	rcStaminaBarFrame->bottom = FRAME_BUFFER_HEIGHT - 40.0f;

	m_pd2dbrBorder->SetColor(D2D1::ColorF(D2D1::ColorF::AliceBlue, 1.0f));
	m_pd2dDeviceContext->DrawRectangle(rcStaminaBarFrame, m_pd2dbrBorder);

#endif //SMALL_WINDOW_SCREEN

	delete rcStaminaBar;
	delete rcStaminaBarFrame;
}

void CSecondRoundSceneUI::BossUI()
{

	D2D1_RECT_F progressFrame[3];
	D2D1_RECT_F progress[3];

	D2D1_COLOR_F			FrameColor = D2D1::ColorF(0.0f, 0.0f, 0.0f, 1.0f);
	D2D1_COLOR_F			ProcessColor = D2D1::ColorF(0.0f, 1.0f, 0.0f, 0.9f);

	float x, y;
	float gab = 400;
	float progressWidth, progressheight;

	// Progress bar
	progressWidth = 400; progressheight = 30; x = FRAME_BUFFER_WIDTH/2 - 600; y = 40;

	for (int i = 0; i < 3; i++)
	{
		progress[i] = { x + gab * i, y, 0, 0};
		m_ppProgressBar[i]->SetColor(FrameColor, ProcessColor);
		m_ppProgressBar[i]->SetPosition(progress[i].left, progress[i].top);
		m_ppProgressBar[i]->SetSize(progressWidth, progressheight);
		m_ppProgressBar[i]->SetProcess(m_fMissionRange[i]);
		m_ppProgressBar[i]->Draw(m_pd2dDeviceContext, m_pd2dbrText, m_pd2dbrBorder);
	}
}

void CSecondRoundSceneUI::MachineUI()
{
	float x, y, width, height;
	float gab = 110;
	width = 130; height = 130; x = 10; y = 200;
	D2D1_RECT_F image = { 0, 0, 130, 110 };

	for (int i = 0; i < m_nMachine; i++)
	{
		S2_COM_STATE comState =  m_ppMachine[i]->GetState();
		
		switch (comState)
		{
		case TURNON:
			LoadUIImage(L"Image/machine.png", m_pwicImagingFactory, m_pd2dfxBitmapSource);
			break;
		case TURNOFF:
			LoadUIImage(L"Image/machine_gray.png", m_pwicImagingFactory, m_pd2dfxBitmapSource);
			break;
		case UNABLE:
			LoadUIImage(L"Image/machine_X.png", m_pwicImagingFactory, m_pd2dfxBitmapSource);
			break;
		default:
			break;
		}	
		m_ppMachine[i]->SetPosition(x, y + gab*i);
		m_ppMachine[i]->Draw(m_pd2dDeviceContext, m_pd2dfxBitmapSource, m_pd2dbrBorder);
	}
}

void CSecondRoundSceneUI::MissionUI()
{

	float x, y, width, height;

	D2D1_RECT_F background = { 0,0,0,0 };
	D2D1_RECT_F missionbackground = { 0,0,0,0 };

	float x_mission, y_mission, width_mission, height_mission;
	float Missionfontsize = 15;
	D2D1_RECT_F rcMissionInfoText = { 0, 0, 0, 0 };


	float fontsize = 15;

	//BG
	width = 900; height = 900; x = (FRAME_BUFFER_WIDTH - width) / 2; y = (FRAME_BUFFER_HEIGHT - height) / 2;
	background = { x, y, x + width, y + height };

	//mission
	width_mission = 700; height_mission = 700; x_mission = (FRAME_BUFFER_WIDTH - width_mission) / 2; y_mission = (FRAME_BUFFER_HEIGHT - height_mission) / 2;
	missionbackground = { x_mission, y_mission, x_mission+width_mission, y_mission+height_mission };
	Missionfontsize = 40;
	rcMissionInfoText = { x_mission, y_mission-40, x_mission + width_mission, y_mission };

	//BG
	m_pd2dbrBorder->SetColor(D2D1::ColorF(D2D1::ColorF::RoyalBlue, 0.85f));
	m_pd2dDeviceContext->FillRectangle(background, m_pd2dbrBorder);

	//TagMission
	WCHAR MissioninfoText[] = L"Chase Moving Box";
	m_pd2dbrText->SetColor(D2D1::ColorF(D2D1::ColorF::Black, 1.0f));
	m_pdWriteFactory->CreateTextFormat(L"Arial", NULL, DWRITE_FONT_WEIGHT_DEMI_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, Missionfontsize, L"en-US", &m_pdwFont);
	m_pd2dDeviceContext->DrawTextW(MissioninfoText, (UINT32)wcslen(MissioninfoText), m_pdwFont, &rcMissionInfoText, m_pd2dbrText);

	m_pd2dbrBorder->SetColor(D2D1::ColorF(D2D1::ColorF::White, 1.0f));
	m_pd2dDeviceContext->FillRectangle(missionbackground, m_pd2dbrBorder);

	//tag rect
	m_ppTagButton[0]->SetPosition(400, 500);
	m_ppTagButton[0]->SetSize(m_fRecSizeX, m_fRecSizeY);
	RecMove(missionbackground);
	m_ppTagButton[0]->Draw(m_pd2dDeviceContext, m_pd2dbrBorder, 0);


	CirclesMove(missionbackground);
}

void CSecondRoundSceneUI::Ending()
{

	D2D1_RECT_F background = { 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT };

	//BG
	m_pd2dbrBorder->SetColor(D2D1::ColorF(D2D1::ColorF::Black, 0.78f));
	m_pd2dDeviceContext->FillRectangle(background, m_pd2dbrBorder);


	D2D1_RECT_F rcEndingRect = { 0.0f, 0.0f, 1920.0f, 1080.0f };
	D2D_POINT_2F f2EndPoint = { 0.f, 0.f };
	LoadUIImage(L"Image/end.png", m_pwicImagingFactory, m_pd2dfxBitmapSource);
	m_pd2dDeviceContext->DrawImage(m_pd2dfxBitmapSource, &f2EndPoint, &rcEndingRect);


}

void CSecondRoundSceneUI::RecMove(D2D1_RECT_F missionBox)
{
	D2D1_RECT_F tempRec= m_ppTagButton[0]->GetRect();
	tempRec.left += dx;
	tempRec.top += dy;

	if (tempRec.left < missionBox.left || tempRec.left + m_fRecSizeX > missionBox.right) dx = -dx;
	if (tempRec.top  < missionBox.top || tempRec.top + m_fRecSizeY > missionBox.bottom) dy = -dy;

	m_ppTagButton[0]->SetPosition(tempRec.left, tempRec.top);
}

void CSecondRoundSceneUI::CirclesMove(D2D1_RECT_F missionBox)
{

	float speedMultiplie = 15;

	if (mCircles.empty()) {
		float margin = 50.0f; 
		D2D1_RECT_F spawnBox = {
			missionBox.left + margin,
			missionBox.top + margin,
			missionBox.right - margin,
			missionBox.bottom - margin
		};

		for (int i = 0; i < 10; ++i) {
			MovCircle circle;
			circle.position = {
				spawnBox.left + (rand() % (int)(spawnBox.right - spawnBox.left)),
				spawnBox.top + (rand() % (int)(spawnBox.bottom - spawnBox.top))
			};
			circle.velocity = {
				static_cast<float>((rand() % 200 - 100) / 100.0f) * speedMultiplie, // 속도는 -1.0f ~ 1.0f 곱하기 speedMultiplier
				static_cast<float>((rand() % 200 - 100) / 100.0f) * speedMultiplie
			};
			circle.radius = 35.0f;
			mCircles.push_back(circle);
		}
	}

	for (auto& circle : mCircles) {
		// 원의 위치 업데이트
		circle.position.x += circle.velocity.x;
		circle.position.y += circle.velocity.y;

		// 벽과 충돌 처리 (missionBox 범위를 벗어나면 반사)
		if (circle.position.x - circle.radius < missionBox.left || circle.position.x + circle.radius > missionBox.right) {
			circle.velocity.x *= -1; // x축 속도 반전
		}
		if (circle.position.y - circle.radius < missionBox.top || circle.position.y + circle.radius > missionBox.bottom) {
			circle.velocity.y *= -1; // y축 속도 반전
		}

		// 원 그리기
		D2D1_ELLIPSE mCircle = { circle.position, circle.radius, circle.radius };
		m_pd2dbrBorder->SetColor(D2D1::ColorF(0.0f, 0.5f, 0.5f, 0.85f));
		m_pd2dDeviceContext->FillEllipse(mCircle, m_pd2dbrBorder);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///

CStartSceneUI::CStartSceneUI() {

	// set Button
	m_nButton = 2;
	m_ppButton = new CButton * [m_nButton];
	
	m_ppButton[0] = new CButton(0, 0, 100, 100, L"test");
	m_ppButton[1] = new CButton(0, 0, 100, 100, L"test");

	// set TextInputBox
	m_nTextInputBox = 2;
	m_ppTextInputBox = new CTextInput * [m_nTextInputBox];

	m_ppTextInputBox[0] = new CTextInput(100, 200, 100, 20, L"test");
	m_ppTextInputBox[1] = new CTextInput(100, 250, 100, 20, L"test");
}

void CStartSceneUI::DrawUI(UINT m_nSwapChainBufferIndex)
{
	m_pd2dDeviceContext->SetTarget(m_ppd2dRenderTargets[m_nSwapChainBufferIndex]);
	ID3D11Resource* ppd3dResources[] = { m_ppd3d11WrappedBackBuffers[m_nSwapChainBufferIndex] };
	m_pd3d11On12Device->AcquireWrappedResources(ppd3dResources, _countof(ppd3dResources));

	m_pd2dDeviceContext->BeginDraw();
	m_pd2dDeviceContext->SetTransform(D2D1::Matrix3x2F::Identity());

	//Direct2D Drawing
#ifdef SMALL_WINDOW_SCREEN

	UISet_Small(m_nSwapChainBufferIndex);
#else
	UISet_Full(m_nSwapChainBufferIndex);
#endif // SMALL_WINDOW_SCREEN


	m_pd2dDeviceContext->EndDraw();
	m_pd3d11On12Device->ReleaseWrappedResources(ppd3dResources, _countof(ppd3dResources));
	m_pd3d11DeviceContext->Flush();
}

void CStartSceneUI::UISet_Small(UINT m_nSwapChainBufferIndex)
{
	D2D1_SIZE_F szRenderTarget = m_ppd2dRenderTargets[m_nSwapChainBufferIndex]->GetSize();

	//=================================================
	// Background Image
	D2D_POINT_2F d2dPoint = { 0.f, 0.f };
	D2D_RECT_F d2dRect = { 0.0f, 0.0f, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT };
	LoadUIImage(L"Image/StartBG_Small.jpg", m_pwicImagingFactory, m_pd2dfxBitmapSource);
	m_pd2dDeviceContext->DrawImage(m_pd2dfxBitmapSource, &d2dPoint, &d2dRect);
	
	//=================================================
	// Partition Rect

	D2D1_RECT_F		PartitionRect = { 0.f , 0.f, 550.f, 800.f };
	m_pd2dbrBorder->SetColor(D2D1::ColorF(0x000000, 0.8f));
	m_pd2dDeviceContext->FillRectangle(PartitionRect, m_pd2dbrBorder);

	//=================================================
	//Button 

	m_ppButton[0]->SetPosition(10, FRAME_BUFFER_HEIGHT / 2 + 120);
	m_ppButton[0]->SetSize(160, 60);
	m_ppButton[0]->CreateTextFormat(m_pdWriteFactory, 20.f);
	m_ppButton[0]->SetText(L"Sign Up");
	m_ppButton[0]->Draw(m_pd2dDeviceContext, m_pd2dbrText, m_pd2dbrBorder);

	m_ppButton[1]->SetPosition(180, FRAME_BUFFER_HEIGHT / 2 + 120);
	m_ppButton[1]->SetSize(160, 60);
	m_ppButton[1]->CreateTextFormat(m_pdWriteFactory, 20.f);
	m_ppButton[1]->SetText(L"Login");
	m_ppButton[1]->Draw(m_pd2dDeviceContext, m_pd2dbrText, m_pd2dbrBorder);

	//=================================================
	// TextBox (ID/PW)
	
	//ID
	m_ppTextInputBox[0]->SetPosition(10, FRAME_BUFFER_HEIGHT / 2 + 30);
	m_ppTextInputBox[0]->SetSize(400, 40);
	m_ppTextInputBox[0]->CreateTextFormat(m_pdWriteFactory, 15.f);
	m_ppTextInputBox[0]->SetText(m_ID.c_str());
	m_ppTextInputBox[0]->Draw(m_pd2dDeviceContext, m_pd2dbrText, m_pd2dbrBorder);

	//PW
	m_ppTextInputBox[1]->SetPosition(10, FRAME_BUFFER_HEIGHT / 2 + 73);
	m_ppTextInputBox[1]->SetSize(400, 40);
	m_ppTextInputBox[1]->CreateTextFormat(m_pdWriteFactory, 15.f);
	m_ppTextInputBox[1]->SetText(m_PW.c_str());
	m_ppTextInputBox[1]->Draw(m_pd2dDeviceContext, m_pd2dbrText, m_pd2dbrBorder);


	//=================================================
	// Tiltle

	D2D1_RECT_F rcTitleRect = { 0.0f, 0.0f, 460.0f, 350.0f };
	D2D_POINT_2F lTitle_Position = { 15.f, 0.f };

	LoadUIImage(L"Image/Title_small.png", m_pwicImagingFactory, m_pd2dfxBitmapSource);
	m_pd2dDeviceContext->DrawImage(m_pd2dfxBitmapSource, &lTitle_Position, &rcTitleRect);

	// Explane text
	m_pdWriteFactory->CreateTextFormat(L"ComicSans", NULL, DWRITE_FONT_WEIGHT_DEMI_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 15.0f, L"en-US", &m_pdwFont);

	float left = 20;
	float top = 350;
	float width = 150.f;
	float height = 50.f;

	D2D1_RECT_F rcPressText = D2D1::RectF(left, top, left + width, top + height);
	
	std::wstring SatrtInfo;

	switch (m_CheckInfo)
	{
	case EMPTY:
		SatrtInfo = L"Enter ID and PW";
		break;
	case ALL_CORRET:
		SatrtInfo = L"Press 'A' to Start";
		break;
	case ID_ERROR:
		SatrtInfo = L"ID ERROR";
		break;
	case PW_ERROR:
		SatrtInfo = L"PW ERROR";
		break;
	default:
		break;
	}
	
	m_pd2dbrText->SetColor(D2D1::ColorF(D2D1::ColorF::White, 1.0f));
	m_pd2dDeviceContext->DrawTextW(SatrtInfo.c_str(), static_cast<UINT32>(SatrtInfo.length()), m_pdwFont, &rcPressText, m_pd2dbrText);
}

void CStartSceneUI::UISet_Full(UINT m_nSwapChainBufferIndex)
{
	D2D1_SIZE_F szRenderTarget = m_ppd2dRenderTargets[m_nSwapChainBufferIndex]->GetSize();

	//=================================================
	// Background Image
	LoadUIImage(L"Image/StartBG.png", m_pwicImagingFactory, m_pd2dfxBitmapSource);
	D2D_POINT_2F d2dPoint = { 0.f, 0.f };
	D2D_RECT_F d2dRect = { 0.0f, 0.0f, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT };
	m_pd2dDeviceContext->DrawImage(m_pd2dfxBitmapSource, &d2dPoint, &d2dRect);

	//=================================================
	// partition Rect

	D2D1_RECT_F		PartitionRect = { 0.f , 0.f, 700.f, 1090.f };
	m_pd2dbrBorder->SetColor(D2D1::ColorF(0x000000, 0.8f));
	m_pd2dDeviceContext->FillRectangle(PartitionRect, m_pd2dbrBorder);

	//=================================================
	//Button 

	m_ppButton[0]->SetPosition(40, FRAME_BUFFER_HEIGHT / 3 + 220);
	m_ppButton[0]->SetSize(130, 50);
	m_ppButton[0]->CreateTextFormat(m_pdWriteFactory, 20.f);
	m_ppButton[0]->SetText(L"Sign Up");
	m_ppButton[0]->Draw(m_pd2dDeviceContext, m_pd2dbrText, m_pd2dbrBorder);

	m_ppButton[1]->SetPosition(200, FRAME_BUFFER_HEIGHT / 3 + 220);
	m_ppButton[1]->SetSize(130, 50);
	m_ppButton[1]->CreateTextFormat(m_pdWriteFactory, 20.f);
	m_ppButton[1]->SetText(L"Login");
	m_ppButton[1]->Draw(m_pd2dDeviceContext, m_pd2dbrText, m_pd2dbrBorder);

	//=================================================
	// TextBox (ID/PW)

	//ID
	m_ppTextInputBox[0]->SetPosition(40, FRAME_BUFFER_HEIGHT / 3 +100);
	m_ppTextInputBox[0]->SetSize(500, 40);
	m_ppTextInputBox[0]->CreateTextFormat(m_pdWriteFactory, 30.f);
	m_ppTextInputBox[0]->SetText(m_ID.c_str());
	m_ppTextInputBox[0]->Draw(m_pd2dDeviceContext, m_pd2dbrText, m_pd2dbrBorder);

	//PW
	m_ppTextInputBox[1]->SetPosition(40, FRAME_BUFFER_HEIGHT / 3 + 160);
	m_ppTextInputBox[1]->SetSize(500, 40);
	m_ppTextInputBox[1]->CreateTextFormat(m_pdWriteFactory, 30.f);
	m_ppTextInputBox[1]->SetText(m_PW.c_str());
	m_ppTextInputBox[1]->Draw(m_pd2dDeviceContext, m_pd2dbrText, m_pd2dbrBorder);


	//=================================================
	// Tiltle

	D2D1_RECT_F rcTitleRect = { 0.0f, 0.0f, 460.0f, 350.0f };
	D2D_POINT_2F lTitle_Position = { 60.f, 20.f };

	LoadUIImage(L"Image/Title.png", m_pwicImagingFactory, m_pd2dfxBitmapSource);
	m_pd2dDeviceContext->DrawImage(m_pd2dfxBitmapSource, &lTitle_Position, &rcTitleRect);


	// 로그인창 아래 안내 문구용
	m_pdWriteFactory->CreateTextFormat(L"ComicSans", NULL, DWRITE_FONT_WEIGHT_DEMI_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 20.0f, L"en-US", &m_pdwFont);
	m_pdwFont->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	float top = FRAME_BUFFER_HEIGHT / 3 + 70;
	float left = 40;
	float width = 200.f;
	float height = 10.f;

	D2D1_RECT_F rcInfoText = D2D1::RectF(left, top, left + width, top + height);
	std::wstring SatrtInfo;
	switch (m_CheckInfo)
	{
	case EMPTY:
		SatrtInfo = L"* Enter ID and PW";
		break;
	case ALL_CORRET:
		SatrtInfo = L"* Press 'A' to Start";
		break;
	case ID_ERROR:
		SatrtInfo = L"* ID ERROR";
		break;
	case PW_ERROR:
		SatrtInfo = L"* PW ERROR";
		break;
	default:
		break;
	}

	m_pd2dbrText->SetColor(D2D1::ColorF(D2D1::ColorF::White, 1.0f));
	m_pd2dDeviceContext->DrawTextW(SatrtInfo.c_str(), static_cast<UINT32>(SatrtInfo.length()), m_pdwFont, &rcInfoText, m_pd2dbrText);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///

void CLoadingUI::DrawUI(UINT m_nSwapChainBufferIndex)
{
	m_pd2dDeviceContext->SetTarget(m_ppd2dRenderTargets[m_nSwapChainBufferIndex]);
	ID3D11Resource* ppd3dResources[] = { m_ppd3d11WrappedBackBuffers[m_nSwapChainBufferIndex] };
	m_pd3d11On12Device->AcquireWrappedResources(ppd3dResources, _countof(ppd3dResources));

	m_pd2dDeviceContext->BeginDraw();
	m_pd2dDeviceContext->SetTransform(D2D1::Matrix3x2F::Identity());

	//Direct2D Drawing
	RenderUIElements(m_nSwapChainBufferIndex);

	m_pd2dDeviceContext->EndDraw();
	m_pd3d11On12Device->ReleaseWrappedResources(ppd3dResources, _countof(ppd3dResources));
	m_pd3d11DeviceContext->Flush();
}

void CLoadingUI::RenderUIElements(UINT m_nSwapChainBufferIndex)
{
	D2D1_SIZE_F szRenderTarget = m_ppd2dRenderTargets[m_nSwapChainBufferIndex]->GetSize();


#ifdef SMALL_WINDOW_SCREEN

	D2D1_RECT_F rcTitleRect = { 0.0f, 0.0f, 1344.0f, 756.0f };
	D2D_POINT_2F lTitle_Position = { 0.f, 0.f };

	LoadUIImage(L"Image/Loading_s.png", m_pwicImagingFactory, m_pd2dfxBitmapSource);
	m_pd2dDeviceContext->DrawImage(m_pd2dfxBitmapSource, &lTitle_Position, &rcTitleRect);

#else

	D2D1_RECT_F rcTitleRect = { 0.0f, 0.0f, 1920.0f, 1080.0f };
	D2D_POINT_2F lTitle_Position = { 0.f, 0.f };

	LoadUIImage(L"Image/Loading.png", m_pwicImagingFactory, m_pd2dfxBitmapSource);
	m_pd2dDeviceContext->DrawImage(m_pd2dfxBitmapSource, &lTitle_Position, &rcTitleRect);


#endif // SMALL_WINDOW_SCREEN

}

