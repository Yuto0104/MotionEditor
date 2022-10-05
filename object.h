//=============================================================================
//
// オブジェクトクラス(object.h)
// Author : 唐﨑結斗
// 概要 : オブジェクト生成を行う
//
//=============================================================================
#ifndef _OBJECT_H_		// このマクロ定義がされてなかったら
#define _OBJECT_H_		// 二重インクルード防止のマクロ定義

//*****************************************************************************
// インクルード
//*****************************************************************************
#include "main.h"

//=============================================================================
// オブジェクトクラス
// Author : 唐﨑結斗
// 概要 : オブジェクト生成を行うクラス
//=============================================================================
class CObject
{
public:
	//--------------------------------------------------------------------
	// オブジェクトの種別の列挙型
	// Author : 唐﨑結斗
	// 概要 : オブジェクトの種別情報を格納
	//--------------------------------------------------------------------
	enum EObjectType
	{
		OBJTYPE_NONE = 0,			// タイプ無し
		OBJTYPE_2DPOLYGON,			// 2Dポリゴン
		OBJTYPE_2DPLAYER,			// プレイヤー
		OBJTYPE_2DBULLET,			// バレット
		OBJTYPE_2DENEMY,			// エネミー
		OBJTYPE_2DEXPLOSION,		// 爆発
		OBJTYPE_3DPLAYER,			// 3Dプレイヤー
		OBJTYPE_3DENEMY,			// 3Dエネミー
		OBJTYPE_3DBOSS,				// 3Dボス
		OBJTYPE_3DBULLET,			// 3Dバレット
		OBJTYPE_FADE,				// フェード
		OBJTYPE_PAUSE,				// ポーズ
		MAX_OBJTYPE,				// 種別の最大数
	};

	//--------------------------------------------------------------------
	// プライオリティのレベル
	// Author : 唐﨑結斗
	// 概要 : プライオリティのレベルを設定
	//--------------------------------------------------------------------
	enum PRIORITY_LEVEL
	{
		PRIORITY_LEVEL0 = 0,	// レベル0
		PRIORITY_LEVEL1,		// レベル1
		PRIORITY_LEVEL2,		// レベル2
		PRIORITY_LEVEL3,		// レベル3
		PRIORITY_LEVEL4,		// レベル4
		MAX_LEVEL				// 最大レベル
	};

	//--------------------------------------------------------------------
	// 定数定義
	//--------------------------------------------------------------------
	static const unsigned int MAX_OBJECT = (0x1000) / MAX_LEVEL;

	//--------------------------------------------------------------------
	// 静的メンバ関数
	//--------------------------------------------------------------------
	static void ReleaseAll(void);																						// すべてのオブジェクトの解放
	static void ReleaseScene(void);																						// 遷移のオブジェクトの解放
	static void UpdateAll(void);																						// すべてのオブジェクトの更新
	static void DrawAll();																								// すべてのオブジェクトの描画
	static CObject *MyGetObject(const int nPriority, const int nNumID) { return m_apObject[nPriority][nNumID]; }		// オブジェクト情報のゲッター
	static void SetPause(const bool bPause) { m_bPause = bPause; }														// ポーズ状態のセッター

	//--------------------------------------------------------------------
	// コンストラクタとデストラクタ
	//--------------------------------------------------------------------
	CObject(int nPriority = PRIORITY_LEVEL0);
	virtual ~CObject();

	//--------------------------------------------------------------------
	// 純粋仮想関数
	//--------------------------------------------------------------------
	virtual HRESULT Init() = 0;																			// 初期化
	virtual void Uninit() = 0;																			// 終了
	virtual void Update() = 0;																			// 更新
	virtual void Draw() = 0;																			// 描画
	virtual void SetPos(const D3DXVECTOR3 &pos) = 0;													// 位置のセッター
	virtual void SetPosOld(const D3DXVECTOR3 &posOld) = 0;												// 過去位置のセッタ
	virtual void SetRot(const D3DXVECTOR3 &rot) = 0;													// 向きのセッター
	virtual void SetSize(const D3DXVECTOR3 &size) = 0;													// 大きさのセッター
	virtual D3DXVECTOR3 GetPos() = 0;																	// 位置のゲッター
	virtual D3DXVECTOR3 GetPosOld() = 0;																// 過去位置のゲッタ
	virtual D3DXVECTOR3 GetRot() = 0;																	// 向きのゲッター
	virtual D3DXVECTOR3 GetSize() = 0;																	// 大きさのゲッター
	void SetObjType(EObjectType objectType) { m_objectType = objectType; }								// オブジェクトの種別設定
	EObjectType GetObjType() { return m_objectType; }													// オブジェクトの種別設定
	void SetColisonPos(const D3DXVECTOR3 &colisonPos) { m_colisonPos = colisonPos; }					// あたり判定の位置のセッター
	D3DXVECTOR3 GetColisonPos() { return m_colisonPos; }												// あたり判定の位置の取得
	void SetColisonSize(const D3DXVECTOR3 &colisonSize) { m_colisonSize = colisonSize; }				// あたり判定の大きさのセッター
	D3DXVECTOR3 GetColisonSize() { return m_colisonSize; }												// あたり判定の大きさの取得

protected:
	//--------------------------------------------------------------------
	// メンバ関数
	//--------------------------------------------------------------------
	void Release(void);																					// オブジェクトの解放
	bool ColisonRange2D(CObject *target);																// 範囲の判定(2D)
	bool ColisonRectangle2D(CObject *target,bool bExtrude);												// 矩形の判定(2D)
	bool ColisonCircle2D(CObject *target, bool bExtrude);												// 円の判定(2D)
	bool ColisonRectangle3D(CObject *target, bool bExtrude);											// 矩形の判定(3D)
	bool ColisonSphere3D(CObject *target, D3DXVECTOR3 size, D3DXVECTOR3 targetSize, bool bExtrude);		// 球の判定(3D)

private:
	//--------------------------------------------------------------------
	// 静的メンバ変数
	//--------------------------------------------------------------------
	static CObject *m_apObject[MAX_LEVEL][MAX_OBJECT];		// オブジェクト情報
	static bool m_bPause;									// ポーズを使用しているかどうか

	//--------------------------------------------------------------------
	// メンバ変数
	//--------------------------------------------------------------------
	EObjectType			m_objectType;		// オブジェクトの種別
	D3DXVECTOR3			m_colisonPos;		// あたり判定の位置
	D3DXVECTOR3			m_colisonSize;		// あたり判定の大きさ
	int					m_nNumID;			// 格納先の番号
	int					m_nLevPriority;		// プライオリティのレベル
};

#endif

