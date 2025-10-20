#pragma once
#include "Player.h"

class CyborgPlayer : public CPlayer
{
public:
	CyborgPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext = NULL, CLoadedModelInfo* pModel = NULL, DWORD dCamera = THIRD_PERSON_CAMERA, int nMaxAni = MAX_ANIMATION_TYPE);
	virtual ~CyborgPlayer();

	Player_Animation_ST GetCurrentAni() { return m_pasCurrentAni; }
	Player_Animation_ST GetNextAni() { return m_pasNextAni; }

public:
	virtual CCamera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);

	virtual void OnPlayerUpdateCallback(float fTimeElapsed);
	virtual void OnCameraUpdateCallback(float fTimeElapsed);

	virtual void Move(DWORD dwDirection, float fDistance, bool bVelocity = false);

	virtual void Update(float fTimeElapsed);
	virtual void Release();
	void UpdateBB();

	void AnimationBlending(Player_Animation_ST type1, Player_Animation_ST type2);

public:
	random_device m_RandomDevice;
	mt19937_64 m_m64RandomEngine{ m_RandomDevice() };

	Player_Animation_ST				m_pasCurrentAni;
	Player_Animation_ST				m_pasNextAni;

	float							m_fVelocitySpeed{};
	float							m_fStaminer{};
	float							m_fMaxStaminer{};
	float							m_fRepairSpeed{};
	float							m_fTakeOverSpeed{};

	int								m_nCharacter;

	bool							m_bIsRun{ false };
	bool							m_bIsCreep{ false };
	bool							m_bIsCreep_flag{ false };
	bool							m_bIsJump{ false };
	bool							m_bIsCrawl{ false };

	bool							m_bHasStaminer{ true };

private:
	CGameObject*					m_pMissionBoxObject = NULL;

	int								m_nMissionObject = NULL;
	CGameObject**					m_ppMissionAnswer = NULL;
	CGameObject**					m_ppMissionObjects = NULL;

	CGameObject**					m_ppSelectMission = NULL;
	int								m_nAnswer{ -1 };
	int								m_nAnswerCount{ 0 };
	bool							m_bSecurityKey{ false };
	int								m_HealTarget{ -1 };

	float							m_fKeyRotate{ 0.f };

public:
	bool GetSecurityKey() { return m_bSecurityKey; };
	void ChangeKeyState(bool state) { m_bSecurityKey = state; }

	int Random(int min, int max);
	int DuplicationCheck(int num, int* target, int end);

	virtual bool GetStaminer() { return m_bHasStaminer; };
	virtual float GetVelocitySpeed() { return m_fVelocitySpeed; };

	int GetHealTarget() { return m_HealTarget; }
	void SetHealTarget(int HealTarget) { m_HealTarget = HealTarget; }

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);

	bool StartKeyMission(int type);
	void MissionCheck(int num);

	void ExhaustionStaminer();
	void RestorationStaminer();

	bool GetCrawl() { return m_bIsCrawl; }

	// Animation
	virtual void SetRun(bool value);
	virtual void SetCreep();
	virtual void SetCreepFlag();
	virtual void SetJump();
	virtual void SetCrawl(bool IsCrawl);

	virtual void IsIdle();
	virtual void IsRun();
	virtual void IsCreep();
	virtual void IsCrawl();
	virtual void IsWalk();
	virtual void IsJump();

	// Data
	virtual void SetPlayerData(int type);

	//-------------------------------------
	void AnimationPacket(const Player_Animation_ST next_anim);
	//-------------------------------------
};

