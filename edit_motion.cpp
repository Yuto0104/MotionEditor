//=============================================================================
//
// 編集用モーションキャラクタークラス(edit_motion.cpp)
// Author : 唐﨑結斗
// 概要 : 編集用モーションキャラクター生成を行う
//
//=============================================================================

//*****************************************************************************
// インクルード
//*****************************************************************************
#include <assert.h>

#include "edit_motion.h"
#include "motion.h"
#include "renderer.h"
#include "application.h"
#include "keyboard.h"

//=============================================================================
// インスタンス生成
// Author : 唐﨑結斗
// 概要 : モーションキャラクター3Dを生成する
//=============================================================================
CEditMotion * CEditMotion::Create(const char *pName)
{
	// オブジェクトインスタンス
	CEditMotion *pEditMotion = nullptr;

	// メモリの解放
	pEditMotion = new CEditMotion;

	// メモリの確保ができなかった
	assert(pEditMotion != nullptr);

	// 数値の初期化
	pEditMotion->Init(pName);

	// インスタンスを返す
	return pEditMotion;
}

//=============================================================================
// コンストラクタ
// Author : 唐﨑結斗
// 概要 : インスタンス生成時に行う処理
//=============================================================================
CEditMotion::CEditMotion() : m_nNumMotion(0)
{

}

//=============================================================================
// デストラクタ
// Author : 唐﨑結斗
// 概要 : インスタンス終了時に行う処理
//=============================================================================
CEditMotion::~CEditMotion()
{

}

//=============================================================================
// 初期化
// Author : 唐﨑結斗
// 概要 : 頂点バッファを生成し、メンバ変数の初期値を設定
//=============================================================================
HRESULT CEditMotion::Init(const char *pName)
{
	// 初期化
	CMotionChar3D::Init(pName);

	// メンバ変数の初期化
	m_nNumMotion = 0;
	m_nSelectParts = 0;

	// モーションを行う
	m_bMotion = true;

	// モーション情報の取得
	CMotion *pMotion = CMotionChar3D::GetMotion();

	// モーションの初期設定
	pMotion->SetMotion(m_nNumMotion);

	// モーション番号の設定
	pMotion->SetNumMotion(m_nNumMotion);

	return E_NOTIMPL;
}

//=============================================================================
// 終了
// Author : 唐﨑結斗
// 概要 : テクスチャのポインタと頂点バッファの解放
//=============================================================================
void CEditMotion::Uninit()
{// 終了
	CMotionChar3D::Uninit();
}

//=============================================================================
// 更新
// Author : 唐﨑結斗
// 概要 : 2D更新を行う
//=============================================================================
void CEditMotion::Update()
{// キーボードの取得
	CKeyboard *pKeyboard = CApplication::GetKeyboard();

	// モーション情報の取得
	CMotion *pMotion = CMotionChar3D::GetMotion();

	// モードの切り替え
	ModePosOrigin();

	if (m_bMotion)
	{
		// ニュートラルモーションの設定
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

		// パーツの取得
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

	// 更新
	CMotionChar3D::Update();
}

//=============================================================================
// 描画
// Author : 唐﨑結斗
// 概要 : 2D描画を行う
//=============================================================================
void CEditMotion::Draw()
{// 描画
	CMotionChar3D::Draw();
}

//=============================================================================
// パーツの元の位置の設定モードの切り替え
// Author : 唐﨑結斗
// 概要 : パーツの元の位置の設定モードの切り替え
//=============================================================================
void CEditMotion::ModePosOrigin()
{// キーボードの取得
	CKeyboard *pKeyboard = CApplication::GetKeyboard();

	// モーション情報の取得
	CMotion *pMotion = CMotionChar3D::GetMotion();

	if (pKeyboard->GetTrigger(DIK_F5))
	{
		m_bMotion ^= 1;

		if (m_bMotion)
		{
			// モーションの初期設定
			pMotion->SetMotion(m_nNumMotion);

			// モーション番号の設定
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

