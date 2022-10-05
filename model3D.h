//=============================================================================
//
// 3Dモデルクラス(model3D.h)
// Author : 唐﨑結斗
// 概要 : 3Dモデル生成を行う
//
//=============================================================================
#ifndef _MODEL_H_			// このマクロ定義がされてなかったら
#define _MODEL_H_			// 二重インクルード防止のマクロ定義

//*****************************************************************************
// インクルード
//*****************************************************************************
#include "object.h"
#include "main.h"

//=============================================================================
// 3Dモデルクラス
// Author : 唐﨑結斗
// 概要 : 3Dモデル生成を行うクラス
//=============================================================================
class CModel3D : public CObject
{
public:
	//--------------------------------------------------------------------
	// モデルのマテリアル情報
	// Author : 唐﨑結斗
	// 概要 : モデルのマテリアル情報の設定
	//--------------------------------------------------------------------
	struct MODEL_MATERIAL
	{
		LPD3DXMESH		pMesh;				// メッシュ情報へのポインタ
		LPD3DXBUFFER	pBuffer;			// マテリアル情報へのポインタ
		DWORD			nNumMat;			// マテリアル情報の数
		int				*pNumTex;			// テクスチャタイプ
		char			aFileName[0xff];	// Xファイルのパス
	};

	//--------------------------------------------------------------------
	// 静的メンバ関数
	//--------------------------------------------------------------------
	static CModel3D *Create(const int nModelNam);			// 3Dモデルの生成

	//--------------------------------------------------------------------
	// コンストラクタとデストラクタ
	//--------------------------------------------------------------------
	CModel3D();
	~CModel3D();

	//--------------------------------------------------------------------
	// メンバ変数
	//--------------------------------------------------------------------
	HRESULT Init() override;																		// 初期化
	HRESULT Init(const int nModelNam);																// 初期化
	void Uninit() override;																			// 終了
	void Update() override;																			// 更新
	void Draw() override;																			// 描画
	void SetPos(const D3DXVECTOR3 &pos) override;													// 位置のセッター
	void SetPosOld(const D3DXVECTOR3 &posOld) override { m_posOld = posOld; }						// 過去位置のセッター
	void SetRot(const D3DXVECTOR3 &rot) override;													// 向きのセッター
	void SetSize(const D3DXVECTOR3 &size) override;													// 大きさのセッター
	void SetMtxWorld(D3DXMATRIX mtxWorld) { m_mtxWorld = mtxWorld; }								// ワールドマトリックスの設定
	D3DXVECTOR3 GetPos() override { return m_pos; }													// 位置のゲッター
	D3DXVECTOR3 GetPosOld()  override { return m_posOld; }											// 過去位置のゲッター
	D3DXVECTOR3 GetRot()  override { return m_rot; }												// 向きのゲッター
	D3DXVECTOR3 GetSize()  override { return m_size; }												// 大きさのゲッター
	D3DXMATRIX GetMtxWorld() { return m_mtxWorld; }													// ワールドマトリックスの取得
	D3DXVECTOR3	WorldCastVtx(D3DXVECTOR3 vtx, D3DXVECTOR3 FormerPos, D3DXVECTOR3 FormerRot);		// ローカル座標をワールド座標に変換する

private:
	//--------------------------------------------------------------------
	// メンバ変数
	//--------------------------------------------------------------------
	MODEL_MATERIAL		m_material;			// マテリアル情報
	D3DXMATRIX			m_mtxWorld;			// ワールドマトリックス
	D3DXVECTOR3			m_pos;				// 位置
	D3DXVECTOR3			m_posOld;			// 過去位置
	D3DXVECTOR3			m_rot;				// 向き
	D3DXVECTOR3			m_size;				// 大きさ
};

#endif