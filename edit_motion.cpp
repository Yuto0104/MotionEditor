//=============================================================================
//
// �ҏW�p���[�V�����L�����N�^�[�N���X(edit_motion.cpp)
// Author : �������l
// �T�v : �ҏW�p���[�V�����L�����N�^�[�������s��
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h
//*****************************************************************************
#include <assert.h>

#include "edit_motion.h"
#include "motion.h"
#include "renderer.h"
#include "application.h"
#include "keyboard.h"

//=============================================================================
// �C���X�^���X����
// Author : �������l
// �T�v : ���[�V�����L�����N�^�[3D�𐶐�����
//=============================================================================
CEditMotion * CEditMotion::Create(const char *pName)
{
	// �I�u�W�F�N�g�C���X�^���X
	CEditMotion *pEditMotion = nullptr;

	// �������̉��
	pEditMotion = new CEditMotion;

	// �������̊m�ۂ��ł��Ȃ�����
	assert(pEditMotion != nullptr);

	// ���l�̏�����
	pEditMotion->Init(pName);

	// �C���X�^���X��Ԃ�
	return pEditMotion;
}

//=============================================================================
// �R���X�g���N�^
// Author : �������l
// �T�v : �C���X�^���X�������ɍs������
//=============================================================================
CEditMotion::CEditMotion() : m_nNumMotion(0)
{

}

//=============================================================================
// �f�X�g���N�^
// Author : �������l
// �T�v : �C���X�^���X�I�����ɍs������
//=============================================================================
CEditMotion::~CEditMotion()
{

}

//=============================================================================
// ������
// Author : �������l
// �T�v : ���_�o�b�t�@�𐶐����A�����o�ϐ��̏����l��ݒ�
//=============================================================================
HRESULT CEditMotion::Init(const char *pName)
{
	// ������
	CMotionChar3D::Init(pName);

	// �����o�ϐ��̏�����
	m_nNumMotion = 0;
	m_nSelectParts = 0;

	// ���[�V�������s��
	m_bMotion = true;

	// ���[�V�������̎擾
	CMotion *pMotion = CMotionChar3D::GetMotion();

	// ���[�V�����̏����ݒ�
	pMotion->SetMotion(m_nNumMotion);

	// ���[�V�����ԍ��̐ݒ�
	pMotion->SetNumMotion(m_nNumMotion);

	return E_NOTIMPL;
}

//=============================================================================
// �I��
// Author : �������l
// �T�v : �e�N�X�`���̃|�C���^�ƒ��_�o�b�t�@�̉��
//=============================================================================
void CEditMotion::Uninit()
{// �I��
	CMotionChar3D::Uninit();
}

//=============================================================================
// �X�V
// Author : �������l
// �T�v : 2D�X�V���s��
//=============================================================================
void CEditMotion::Update()
{// �L�[�{�[�h�̎擾
	CKeyboard *pKeyboard = CApplication::GetKeyboard();

	// ���[�V�������̎擾
	CMotion *pMotion = CMotionChar3D::GetMotion();

	// ���[�h�̐؂�ւ�
	ModePosOrigin();

	if (m_bMotion)
	{
		// �j���[�g�������[�V�����̐ݒ�
		if (!pMotion->GetMotion())
		{
			pMotion->SetNumMotion(m_nNumMotion);
		}
	}
	else if (!m_bMotion)
	{
		pMotion->ResetSelectParts();

		if (pKeyboard->GetTrigger(DIK_UP))
		{
			m_nSelectParts--;

			if (m_nSelectParts < 0)
			{
				m_nSelectParts = pMotion->GetMaxParts() - 1;
			}
		}
		else if (pKeyboard->GetTrigger(DIK_DOWN))
		{
			m_nSelectParts++;

			if (m_nSelectParts >= pMotion->GetMaxParts())
			{
				m_nSelectParts = 0;
			}
		}

		pMotion->SelectParts(m_nSelectParts);

		// �p�[�c�̎擾
		D3DXVECTOR3 partsPosOrigin = pMotion->GetPosOrigin(m_nSelectParts);

		if (pKeyboard->GetPress(DIK_T))
		{
			partsPosOrigin.x += 1.0f;
		}
		else if (pKeyboard->GetPress(DIK_G))
		{
			partsPosOrigin.x -= 1.0f;
		}
		if (pKeyboard->GetPress(DIK_Y))
		{
			partsPosOrigin.y += 1.0f;
		}
		else if (pKeyboard->GetPress(DIK_H))
		{
			partsPosOrigin.y -= 1.0f;
		}
		if (pKeyboard->GetPress(DIK_U))
		{
			partsPosOrigin.z += 1.0f;
		}
		else if (pKeyboard->GetPress(DIK_J))
		{
			partsPosOrigin.z -= 1.0f;
		}

		pMotion->SetPosOrigin(m_nSelectParts, partsPosOrigin);
		pMotion->SetPartsOrigin();
	}

	// �X�V
	CMotionChar3D::Update();
}

//=============================================================================
// �`��
// Author : �������l
// �T�v : 2D�`����s��
//=============================================================================
void CEditMotion::Draw()
{// �`��
	CMotionChar3D::Draw();
}

//=============================================================================
// �p�[�c�̌��̈ʒu�̐ݒ胂�[�h�̐؂�ւ�
// Author : �������l
// �T�v : �p�[�c�̌��̈ʒu�̐ݒ胂�[�h�̐؂�ւ�
//=============================================================================
void CEditMotion::ModePosOrigin()
{// �L�[�{�[�h�̎擾
	CKeyboard *pKeyboard = CApplication::GetKeyboard();

	// ���[�V�������̎擾
	CMotion *pMotion = CMotionChar3D::GetMotion();

	if (pKeyboard->GetTrigger(DIK_F5))
	{
		m_bMotion ^= 1;

		if (m_bMotion)
		{
			// ���[�V�����̏����ݒ�
			pMotion->SetMotion(m_nNumMotion);

			// ���[�V�����ԍ��̐ݒ�
			pMotion->SetNumMotion(m_nNumMotion);
		}
		else
		{
			pMotion->SetUseMotion(false);
			pMotion->SetMotionBlend(false);
			pMotion->SetPartsOrigin();
		}

		pMotion->ResetSelectParts();
	}
}

