//=============================================================================
//
// アプリケーションクラス(application.h)
// Author : 唐﨑結斗
// 概要 : 各クラスの呼び出しを行うクラス
//
//=============================================================================
#ifndef _APPLICATION_H_		// このマクロ定義がされてなかったら
#define _APPLICATION_H_		// 二重インクルード防止のマクロ定義

//*****************************************************************************
// インクルード
//*****************************************************************************
#include "main.h"

//*****************************************************************************
// 前方宣言
//*****************************************************************************
class CRenderer;
class CKeyboard;
class CMouse;
class CTexture;
class CModelManager;
class CCameraManager;
class CCamera;

//=============================================================================
// アプリケーションクラス
// Author : 唐﨑結斗
// 概要 : 各クラスの呼び出しを行うクラス
//=============================================================================
class CApplication
{
public:
	//--------------------------------------------------------------------
	// 静的メンバ関数
	//--------------------------------------------------------------------
	static HWND GetWnd() { return m_hWnd; }												// ウィンドウの取得
	static CRenderer *GetRenderer() { return m_pRenderer; }								// レンダラーのゲッター
	static CKeyboard *GetKeyboard() { return m_pKeyboard; }								// キーボードのゲッター
	static CMouse *GetMouse() { return m_pMouse; }										// マウスのゲッター
	static CTexture *GetTexture() { return m_pTexture; }								// テクスチャのゲッター
	static CModelManager *GetModelManager() { return m_pModelManager; }					// モデルマネージャーのゲッター
	static CCameraManager *GetCameraManager() { return m_pCameraManager; }				// カメラマネージャーのゲッター
	static CCamera *GetCamera() { return m_pCamera; }									// カメラのゲッター
	static D3DXVECTOR3 ScreenCastWorld(const D3DXVECTOR3 &pos);							// ワールド座標をスクリーン座標にキャストする
	static D3DXVECTOR3 WorldCastScreen(const D3DXVECTOR3 &pos);							// ワールド座標をスクリーン座標にキャストする
	static float RotNormalization(float fRot);											// 角度の設定
	static float RotNormalization(float fRot,float fMin,float fMax);					// 角度の設定

	//--------------------------------------------------------------------
	// コンストラクタとデストラクタ
	//--------------------------------------------------------------------
	CApplication();
	~CApplication();

	//--------------------------------------------------------------------
	// メンバ関数
	//--------------------------------------------------------------------
	HRESULT Init(HINSTANCE hInstance, HWND hWnd);					// 初期化
	void Uninit();													// 終了
	void Update();													// 更新
	void Draw();													// 描画

private:
	//--------------------------------------------------------------------
	// 静的メンバ変数
	//--------------------------------------------------------------------
	static HWND	m_hWnd;							// ウィンドウ
	static CRenderer *m_pRenderer;				// rendererクラス
	static CKeyboard *m_pKeyboard;				// キーボードクラス
	static CMouse *m_pMouse;					// マウスクラス
	static CTexture *m_pTexture;				// テクスチャクラス
	static CModelManager *m_pModelManager;		// モデルマネージャークラス
	static CCameraManager *m_pCameraManager;	// カメラマネージャークラス
	static CCamera *m_pCamera;					// ゲームカメラクラス
};

#endif

