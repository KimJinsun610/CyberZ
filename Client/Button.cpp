
#include "stdafx.h"
#include "Button.h"

CButton::CButton(float x, float y, float width, float height, const wchar_t* text)
{
	m_fLeft = x;
	m_fRight = x + width;
	m_fTop = y;
	m_fBottom = y + height;

	m_rButtonBox = { m_fLeft , m_fTop, m_fRight, m_fBottom };
	
	m_text = text;
}

CButton::~CButton()
{
}

void CButton::Release()
{
	if (m_pTextFormat) m_pTextFormat->Release();
}


void CButton::SetPosition(float x, float y)
{
	m_fLeft = x;
	m_fTop = y;
}

void CButton::SetSize(float width, float height)
{
	m_fRight = m_fLeft + width;
	m_fBottom = m_fTop + height;
}

void CButton::SetText(const wchar_t* text)
{
	m_text = text;
}

void CButton::CreateTextFormat(IDWriteFactory* pDWriteFactory, float fontSize)
{
	if (!m_pTextFormat)
	{
		pDWriteFactory->CreateTextFormat(
			L"Arial",                // Font family name
			NULL,                    // Font collection (NULL sets it to use the system font collection)
			DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			fontSize,
			L"en-us",
			&m_pTextFormat
		);
	}
}

void CButton::Draw(ID2D1DeviceContext2* pd2dDeviceContext, ID2D1SolidColorBrush* pd2dbrText, ID2D1SolidColorBrush* pd2dbrBorder)
{
	m_rTextInput = { m_fLeft + 5, m_fTop, m_fRight, m_fBottom };
	m_rButtonBox = { m_fLeft, m_fTop, m_fRight, m_fBottom };
	m_rRoundButtonBox = { m_rButtonBox , 5.f, 5.f};

	//Box
	if (m_bSelected) pd2dbrBorder->SetColor(D2D1::ColorF(D2D1::ColorF::White, 0.9f));
	else pd2dbrBorder->SetColor(D2D1::ColorF(D2D1::ColorF::GreenYellow, 0.9f));

	pd2dDeviceContext->FillRoundedRectangle(m_rRoundButtonBox, pd2dbrBorder);

	//text
	if (m_pTextFormat)
	{
		m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		if (m_bSelected) pd2dbrText->SetColor(D2D1::ColorF(D2D1::ColorF::Black, 1.0f));
		else pd2dbrText->SetColor(D2D1::ColorF(D2D1::ColorF::Black, 1.0f));
		
		pd2dDeviceContext->DrawTextW(m_text.c_str(), static_cast<UINT32>(m_text.length()), m_pTextFormat, m_rTextInput, pd2dbrText);
	}
}

bool CButton::CheckChlick(HWND hWnd, POINT CursorPos)
{
	m_bSelected = false;

	// 마우스 커서 위치 계산
	RECT WindowRect;
	GetWindowRect(hWnd, &WindowRect);

	POINT mousePos;
#ifdef FULL_SCREEN

	mousePos.x = CursorPos.x - WindowRect.left;
	mousePos.y = CursorPos.y - WindowRect.top;

#else
	// 좌표 변환
	mousePos.x = CursorPos.x - WindowRect.left;
	mousePos.y = CursorPos.y - WindowRect.top - 30;

#endif // FULL_SCREEN

	// 충돌확인
	if (mousePos.x > m_fLeft && mousePos.x < m_fRight)
	{
		if (mousePos.y > m_fTop && mousePos.y < m_fBottom)
		{
			m_bSelected = true;
		}
	}

	return(m_bSelected);
}
