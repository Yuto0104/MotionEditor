//**************************************************************************************************
//
// 計算処理(calculation.h)
// Auther：唐﨑結斗
//
//**************************************************************************************************
#ifndef _CALCULATION_H_			// このマクロ定義がされてなかったら
#define _CALCULATION_H_			// 二重インクルード防止のマクロ定義

//***************************************************************
// プロトタイプ宣言
//***************************************************************
// 制御関数
D3DXVECTOR3	WorldCastVtx(D3DXVECTOR3 vtx, D3DXVECTOR3 FormerPos, D3DXVECTOR3 FormerRot);				// ワールド座標にキャスト
D3DXVECTOR3	WorldCastVtx(D3DXVECTOR3 vtx, D3DXVECTOR3 FormerPos, D3DXQUATERNION FormeQuaternion);		// ワールド座標にキャスト
float RotNormalization(float fRot);																		// 角度の正規化
D3DXVECTOR3 WorldCastScreen(D3DXVECTOR3 *screenPos,														// スクリーン座標
	D3DXVECTOR3 screenSize,																				// スクリーンサイズ
	D3DXMATRIX* mtxView,																				// ビューマトリックス
	D3DXMATRIX* mtxProjection);																			// プロジェクションマトリックス
D3DXVECTOR3 ScreenCastWorld(D3DXVECTOR3 *screenPos,														// スクリーン座標
	D3DXVECTOR3 screenSize,																				// スクリーンサイズ
	D3DXMATRIX* mtxView,																				// ビューマトリックス
	D3DXMATRIX* mtxProjection,																			// プロジェクションマトリックス
	D3DXMATRIX *mtxWorld);																				// ワールド座標				

#endif
