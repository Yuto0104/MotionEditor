//**************************************************************************************************
//
// ���[�V��������(motion.h)
// Auther�F�������l
// �T�v : ���[�V�����N���X�̐ݒ�
//
//**************************************************************************************************
#ifndef _MOTION_H_			// ���̃}�N����`������ĂȂ�������
#define _MOTION_H_			// ��d�C���N���[�h�h�~�̃}�N����`

//***************************************************************************
// �C���N���[�h
//***************************************************************************
#include "main.h"
#include "model3D.h"
#include "renderer.h"

//=============================================================================
// ���[�V�����N���X
// Author : �������l
// �T�v : ���[�V�����ݒ���s���N���X
//=============================================================================
class CMotion
{
public:
	//***************************************************************************
	// �萔��`
	//***************************************************************************
	static const unsigned int MAX_MOTION = (128);					// ���[�V�������̍ő吔
	static const unsigned int MAX_MODEL_PARTS = (128);				// ���f�����̍ő吔
	static const unsigned int MAX_KEY = (64);						// �L�[�̍ő吔
	static const unsigned int MAX_KEYSET = (64);					// �L�[�ݒ�̍ő吔
	static const unsigned int MOTION_BLEND_FRAM = (12);				// ���[�V�����u�����h�̃t���[����	

	//***************************************************************
	// �L�[�\���̂��`
	//***************************************************************
	typedef struct
	{
		D3DXVECTOR3		pos;	// ���݈ʒu
		D3DXVECTOR3		rot;	// ���݂̌���
	}MyKey;

	//***************************************************************
	// �L�[�ݒ�\���̂��`
	//***************************************************************
	typedef struct
	{
		int		nFrame;			// �t���[����
		MyKey	*pKey;			// �L�[���
	}MyKeySet;

	//***************************************************************
	// ���[�V�����ݒ�\���̂��`
	//***************************************************************
	typedef struct
	{
		int				nNumKey;				// �L�[��
		int				nCntKeySet;				// �L�[�Z�b�g�J�E���g
		int				nCntFrame;				// �t���[���J�E���g
		bool			bLoop;					// ���[�v�g�p��
		bool			bMotion;				// ���[�V�������s���Ă��邩
		MyKeySet		*pKeySet;				// �L�[�ݒ���
	}MyMotion;

	//***************************************************************
	// ���f���p�[�c�\���̂��`
	//***************************************************************
	typedef struct
	{
		CModel3D::MODEL_MATERIAL		material;				// �}�e���A�����
		D3DXMATRIX						mtxWorld;				// ���[���h�}�g���b�N�X
		D3DXVECTOR3						posOrigin;				// ���̈ʒu
		D3DXVECTOR3						rotOrigin;				// ���̌���
		D3DXVECTOR3						pos;					// ���݂̈ʒu
		D3DXVECTOR3						rot;					// ���݂̌���
		D3DXVECTOR3						posDest;				// �ړI�̈ʒu
		D3DXVECTOR3						rotDest;				// �ړI�̌���
		int								nIdxModelParent;		// �e���f���̃C���f�b�N�X��
		int								nType;					// �p�[�c�̃^�C�v
		bool							bSelect;				// �w�肳��Ă��邩�ۂ�
	}Parts;

	//--------------------------------------------------------------------
	// �R���X�g���N�^�ƃf�X�g���N�^
	//--------------------------------------------------------------------
	CMotion(const char *pFileName);
	~CMotion();

	//--------------------------------------------------------------------
	// �����o�֐�
	//--------------------------------------------------------------------
	// �X�V
	void Update();

	// �I��
	void Uninit(void);

	// �p�[�c�����Ƃ̏ꏊ�ɔz�u����
	void SetPartsOrigin();

	// ���[�V�����̏����ݒ�
	void SetMotion(const int nCntMotionSet);

	// �p�[�c�̐ݒ�
	void SetParts(D3DXMATRIX mtxWorld);

	// �p�[�c�̐F�ݒ�
	void SetParts(D3DXMATRIX mtxWorld, const D3DXCOLOR& col);

	// �p�[�c�̃Z���N�g
	void SelectParts(const int nNumParts);

	// �p�[�c�Z���N�g�̃��Z�b�g
	void ResetSelectParts();

	// ���[�V�����̍ēǂݍ���
	void ReloadMotion(const char *pFileName);

	// ���[�V�����ԍ��̐ݒ�
	void SetNumMotion(const int nNumMotion);

	// �p�[�c�̌��̈ʒu�̐ݒ�
	void SetPosOrigin(const int nNumParts, const D3DXVECTOR3 pos);

	// ���[�V�������s���Ă��邩�ݒ�
	void SetUseMotion(bool bMotion) { m_bMotion = bMotion; }

	// ���[�V�����u�����h���s���Ă��邩�ݒ�
	void SetMotionBlend(bool bMotionBlend) { m_bMotionBlend = bMotionBlend; }

	// �p�[�c�̌��̈ʒu�̎擾
	D3DXVECTOR3 GetPosOrigin(const int nNumParts);

	// �p�[�c�̍ő吔�̎擾
	int GetMaxParts() { return m_nMaxParts; }

	// ���[�V�������s���Ă��邩�擾
	bool GetMotion() { return m_bMotion; }

	// ���[�V�����u�����h���s���Ă��邩�擾
	bool GetMotionBlend() { return m_bMotionBlend; }

private:
	//--------------------------------------------------------------------
	// �����o�֐�
	//--------------------------------------------------------------------
	// ������
	void Init();

	// ���[�V�����̍Đ�
	void PlayMotion();

	// ���[�V�����u�����h
	void MotionBlend();

	// ���[�V�����ǂݍ���
	void LoodSetMotion(const char *pFileName);

	// �J�E���g���Z�b�g
	void CntReset(const int nNumMotionOld);

	//--------------------------------------------------------------------
	// �����o�ϐ�
	//--------------------------------------------------------------------
	MyMotion	*m_motion;									// ���[�V����
	Parts		*m_parts;									// �p�[�c
	char		m_partsFile[MAX_MODEL_PARTS][0xff];			// �p�[�c��X�t�@�C����
	int			m_nMaxParts;								// �p�[�c��
	int			m_nNumMotion;								// �������[�V����
	bool		m_bMotion;									// ���[�V�������s����
	bool		m_bMotionBlend;								// ���[�V�����u�����h
};

#endif

