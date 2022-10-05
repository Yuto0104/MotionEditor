//=============================================================================
//
// アプリケーションクラス(application.h)
// Author : 唐﨑結斗
// 概要 : 各クラスの呼び出しを行うクラス
//
//=============================================================================

//*****************************************************************************
// インクルード
//*****************************************************************************
#include "application.h"

#include <assert.h>
#include <stdio.h>

#include "renderer.h"
#include "keyboard.h"
#include "mouse.h"
#include "texture.h"
#include "model_manager.h"
#include "camera_manager.h"
#include "camera.h"
#include "light.h"
#include "object.h"
#include "object3D.h"
#include "model3D.h"
#include "motion_char3D.h"
#include "edit_motion.h"
#include "mesh.h"
#include "sphere.h"

//*****************************************************************************
// 静的メンバ変数宣言
//*****************************************************************************
HWND CApplication::m_hWnd = nullptr;								// ウィンドウ
CRenderer *CApplication::m_pRenderer = nullptr;						// レンダラーインスタンス
CKeyboard *CApplication::m_pKeyboard = {};							// キーボードインスタンス
CMouse *CApplication::m_pMouse = {};								// マウスインスタンス
CTexture *CApplication::m_pTexture = nullptr;						// テクスチャインスタンス
CModelManager *CApplication::m_pModelManager = nullptr;				// モデルマネージャークラス
CCameraManager *CApplication::m_pCameraManager = nullptr;			// カメラマネージャークラス
CCamera *CApplication::m_pCamera = nullptr;							// カメラインスタンス

//=============================================================================
// スクリーン座標をワールド座標にキャストする
// Author : 唐﨑結斗
// 概要 : 
//=============================================================================
D3DXVECTOR3 CApplication::ScreenCastWorld(const D3DXVECTOR3 &pos)
{
	// 計算用マトリックス
	D3DXMATRIX mtx, mtxTrans, mtxView, mtxPrj, mtxViewPort;

	// 行列移動関数 (第一引数にX,Y,Z方向の移動行列を作成)
	D3DXMatrixTranslation(&mtxTrans, pos.x, pos.y, pos.z);

	// カメラのビューマトリックスの取得
	mtxView = m_pCamera->GetMtxView();

	// カメラのプロジェクションマトリックスの取得
	mtxPrj = m_pCamera->GetMtxProj();

	// マトリックスの乗算
	mtx = mtxTrans * mtxView * mtxPrj;

	// ビューポート行列（スクリーン行列）の作成
	float w = (float)CRenderer::SCREEN_WIDTH / 2.0f;
	float h = (float)CRenderer::SCREEN_HEIGHT / 2.0f;

	mtxViewPort = {
		w , 0 , 0 , 0 ,
		0 ,-h , 0 , 0 ,
		0 , 0 , 1 , 0 ,
		w , h , 0 , 1
	};

	// マトリックスのXYZ
	D3DXVECTOR3 vec = D3DXVECTOR3(mtx._41, mtx._42, mtx._43);

	D3DXVec3TransformCoord(&vec, &vec, &mtxViewPort);

	return vec;
}

//=============================================================================
// ワールド座標をスクリーン座標にキャストする
// Author : 唐﨑結斗
// 概要 : 
//=============================================================================
D3DXVECTOR3 CApplication::WorldCastScreen(const D3DXVECTOR3 &pos)
{
	// 計算用ベクトル
	D3DXVECTOR3 vec = pos;

	// 計算用マトリックス
	D3DXMATRIX mtx, mtxTrans, mtxView, mtxPrj, mtxViewPort;

	// 行列移動関数 (第一引数にX,Y,Z方向の移動行列を作成)
	D3DXMatrixTranslation(&mtxTrans, pos.x, pos.y, pos.z);

	// カメラのビューマトリックスの取得
	mtxView = m_pCamera->GetMtxView();

	// カメラのプロジェクションマトリックスの取得
	mtxPrj = m_pCamera->GetMtxProj();
	
	// ビューポート行列（スクリーン行列）の作成
	D3DXMatrixIdentity(&mtxViewPort);
	float w = (float)CRenderer::SCREEN_WIDTH / 2.0f;
	float h = (float)CRenderer::SCREEN_HEIGHT / 2.0f;
	mtxViewPort = {
		w , 0 , 0 , 0 ,
		0 ,-h , 0 , 0 ,
		0 , 0 , 1 , 0 ,
		w , h , 0 , 1
	};

	// 逆行列の算出
	D3DXMatrixInverse(&mtxView, NULL, &mtxView);
	D3DXMatrixInverse(&mtxPrj, NULL, &mtxPrj);
	D3DXMatrixInverse(&mtxViewPort, NULL, &mtxViewPort);

	// 逆変換
	mtx = mtxViewPort * mtxPrj * mtxView;
	D3DXVec3TransformCoord(&vec, &D3DXVECTOR3(vec.x, vec.y, vec.z), &mtx);

	return vec;
}

//=============================================================================
// 角度の正規化処理
// Author : 唐﨑結斗
// 概要 : 角度が円周率の2倍を超えたときに範囲内に戻す
//=============================================================================
float CApplication::RotNormalization(float fRot)
{
	if (fRot >= D3DX_PI)
	{// 移動方向の正規化
		fRot -= D3DX_PI * 2;
	}
	else if (fRot <= -D3DX_PI)
	{// 移動方向の正規化
		fRot += D3DX_PI * 2;
	}

	return fRot;
}

//=============================================================================
// 角度の正規化処理
// Author : 唐﨑結斗
// 概要 : 角度が円周率の2倍を超えたときに範囲内に戻す
//=============================================================================
float CApplication::RotNormalization(float fRot, float fMin, float fMax)
{
	if (fRot >= fMax)
	{// 移動方向の正規化
		fRot -= D3DX_PI * 2;
	}
	else if (fRot <= fMin)
	{// 移動方向の正規化
		fRot += D3DX_PI * 2;
	}

	return fRot;
}

//=============================================================================
// コンストラクタ
// Author : 唐﨑結斗
// 概要 : インスタンス生成時に行う処理
//=============================================================================
CApplication::CApplication()
{

}

//=============================================================================
// デストラクタ
// Author : 唐﨑結斗
// 概要 : インスタンス終了時に行う処理
//=============================================================================
CApplication::~CApplication()
{
	assert(m_pRenderer == nullptr);
	assert(m_pKeyboard == nullptr);
	assert(m_pMouse == nullptr);
	assert(m_pTexture == nullptr);
	assert(m_pModelManager == nullptr);
	assert(m_pCameraManager == nullptr);
	assert(m_pCamera == nullptr);
}

//=============================================================================
// 初期化
// Author : 唐﨑結斗
// 概要 : メンバ変数を解放し、他クラスのクリエイト関数を呼び出す
//=============================================================================
HRESULT CApplication::Init(HINSTANCE hInstance, HWND hWnd)
{
	// ウィンドウ
	m_hWnd = hWnd;

	// メモリの確保	
	m_pRenderer = new CRenderer;
	m_pTexture = new CTexture;
	m_pModelManager = new CModelManager;
	m_pCameraManager = new CCameraManager;
	m_pCamera = new CCamera;

	// 入力デバイスのメモリ確保
	m_pKeyboard = new CKeyboard;
	m_pMouse = new CMouse;

	// 初期化処理
	assert(m_pRenderer != nullptr);
#ifdef _DEBUG
	if (FAILED(m_pRenderer->Init(m_hWnd, TRUE)) != 0)
	{//初期化処理が失敗した場合
		return-1;
	}
#else
	if (FAILED(m_pRenderer->Init(m_hWnd, FALSE)) != 0)
	{//初期化処理が失敗した場合
		return-1;
	}
#endif

	// 初期化処理
	assert(m_pTexture != nullptr);
	m_pTexture->Init();

	// 初期化処理
	assert(m_pModelManager != nullptr);
	m_pModelManager->Init();

	// 初期化処理
	assert(m_pCameraManager != nullptr);
	m_pCameraManager->Init();

	// 初期化処理
	assert(m_pCamera != nullptr);
	m_pCamera->Init();
	m_pCamera->SetViewType(CCamera::TYPE_CLAIRVOYANCE);

	// 初期化処理
	assert(m_pKeyboard != nullptr);
	if (FAILED(m_pKeyboard->Init(hInstance, m_hWnd)))
	{
		return E_FAIL;
	}

	// 初期化処理
	assert(m_pMouse != nullptr);
	if (FAILED(m_pMouse->Init(hInstance, m_hWnd)))
	{
		return E_FAIL;
	}

	// ライトの作成
	CLight::Create(D3DXVECTOR3(0.2f, -0.8f, -0.4f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	CLight::Create(D3DXVECTOR3(0.2f, 0.8f, -0.4f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	CLight::Create(D3DXVECTOR3(-0.2f, -0.5f, 0.7f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

	CMesh3D *pMesh3D = CMesh3D::Create();
	pMesh3D->SetRot(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	pMesh3D->SetSize(D3DXVECTOR3(100000.0f, 0, 100000.0f));
	pMesh3D->SetBlock(CMesh3D::DOUBLE_INT(100, 100));
	pMesh3D->SetSplitTex(true);
	pMesh3D->LoadTex(13);

	CSphere *pSphere = CSphere::Create();
	pSphere->SetRot(D3DXVECTOR3(D3DX_PI, 0.0f, 0.0f));
	pSphere->SetSize(D3DXVECTOR3(100.0f, 0, 100.0f));
	pSphere->SetBlock(CMesh3D::DOUBLE_INT(100, 100));
	pSphere->SetRadius(50000.0f);
	pSphere->SetSphereRange(D3DXVECTOR2(D3DX_PI * 2.0f, D3DX_PI * -0.5f));
	pSphere->LoadTex(12);

	CEditMotion *pEditMotion = CEditMotion::Create("data/MOTION/motion.txt");
	pEditMotion ->SetPos(D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	return S_OK;
}

//=============================================================================
// 終了
// Author : 唐﨑結斗
// 概要 : メモリの解放とリリース関数を呼び出す
//=============================================================================
void CApplication::Uninit()
{
	// オブジェクトの解放
	CObject::ReleaseAll();

	if (m_pRenderer != nullptr)
	{// 終了処理
		m_pRenderer->Uninit();

		// メモリの解放
		delete m_pRenderer;
		m_pRenderer = nullptr;
	}

	if (m_pKeyboard != nullptr)
	{// 終了処理
		m_pKeyboard->Uninit();

		// メモリの解放
		delete m_pKeyboard;
		m_pKeyboard = nullptr;
	}

	if (m_pMouse != nullptr)
	{// 終了処理
		m_pMouse->Uninit();

		// メモリの解放
		delete m_pMouse;
		m_pMouse = nullptr;
	}

	if (m_pTexture != nullptr)
	{// 終了処理
		m_pTexture->Uninit();

		// メモリの解放
		delete m_pTexture;
		m_pTexture = nullptr;
	}

	if (m_pModelManager != nullptr)
	{// 終了処理
		m_pModelManager->Uninit();

		// メモリの解放
		delete m_pModelManager;
		m_pModelManager = nullptr;
	}

	if (m_pCameraManager != nullptr)
	{// 終了処理
		m_pCameraManager->Uninit();

		// メモリの解放
		delete m_pCameraManager;
		m_pCameraManager = nullptr;
	}

	if (m_pCamera != nullptr)
	{// 終了処理
		m_pCamera->Uninit();

		// メモリの解放
		delete m_pCamera;
		m_pCamera = nullptr;
	}

	// ライトの解放
	CLight::ReleaseAll();
}

//=============================================================================
// 更新
// Author : 唐﨑結斗
// 概要 : 更新
//=============================================================================
void CApplication::Update()
{
	m_pKeyboard->Update();
	m_pMouse->Update();
	m_pCamera->Update();

	m_pRenderer->Update();
}

//=============================================================================
// 描画
// Author : 唐﨑結斗
// 概要 : 描画
//=============================================================================
void CApplication::Draw()
{
	m_pRenderer->Draw();
}
