//**************************************************************************************************
//
// モーション処理(motion.cpp)
// Auther：唐﨑結斗
//
//**************************************************************************************************

//***************************************************************************
// インクルード
//***************************************************************************
#include <stdio.h>
#include <assert.h>

#include "application.h"
#include "motion.h"	
#include "model_manager.h"
#include "calculation.h"
#include "texture.h"

//=============================================================================
// コンストラクタ
// Author : 唐﨑結斗
// 概要 : インスタンス生成時に行う処理
//=============================================================================
CMotion::CMotion(const char * pFileName)
{
	// パーツ名の初期化
	memset(&m_partsFile, 0, sizeof(m_partsFile));

	// モーションの初期化
	m_motion = nullptr;

	// パーツの初期化
	m_parts = nullptr;

	// パーツ数の初期化
	m_nMaxParts = 0;

	// モーションの読み込み
	LoodSetMotion(pFileName);

	// 扱うモーション
	m_nNumMotion = 0;	

	// モーションを行うか
	m_bMotion = false;		

	// モーションブレンド
	m_bMotionBlend = false;	
}

//=============================================================================
// デストラクタ
// Author : 唐﨑結斗
// 概要 : インスタンス終了時に行う処理
//=============================================================================
CMotion::~CMotion()
{
}

//=============================================================================
// 初期化
// Author : 唐﨑結斗
// 概要 : パーツ情報のXファイル読み込み
//=============================================================================
void CMotion::Init(void)
{
	// モデルマネージャークラスの設定
	CModelManager *pModelManager = CApplication::GetModelManager();

	for (int nCntMotion = 0; nCntMotion < MAX_MOTION; nCntMotion++)
	{// カウントのリセット
		CntReset(nCntMotion);
	}

	for (int nCntParts = 0; nCntParts < m_nMaxParts; nCntParts++)
	{
		CMotion::Parts* parts = (m_parts + nCntParts);

		// 位置と向きの初期値を保存
		parts->posOrigin = parts->pos;
		parts->rotOrigin = parts->rot;

		// パーツ情報の初期化
		parts->mtxWorld = {};		// ワールドマトリックス

		// マテリアル情報の代入
		parts->material = pModelManager->GetModelMateria(parts->nType);
	}
}

//=============================================================================
// 更新
// Author : 唐﨑結斗
// 概要 : モーションの更新
//=============================================================================
void CMotion::Update()
{
	if (m_bMotionBlend)
	{
		MotionBlend();
	}
	else if (m_bMotion
		&& !m_bMotionBlend)
	{
		PlayMotion();
	}
}

//=============================================================================
// モーションの設定
// Author : 唐﨑結斗
// 概要 : モーションの初期位置に設定
//=============================================================================
void CMotion::SetMotion(const int nCntMotionSet)
{
	CMotion::MyMotion *motion = (m_motion + nCntMotionSet);

	for (int nCntParts = 0; nCntParts < m_nMaxParts; nCntParts++)
	{
		CMotion::Parts* parts = (m_parts + nCntParts);

		// 位置の設定
		parts->pos = (parts->posOrigin + motion->pKeySet[motion->nCntKeySet].pKey[nCntParts].pos);

		//	向きの設定
		parts->rot = (parts->rotOrigin + motion->pKeySet[motion->nCntKeySet].pKey[nCntParts].rot);

		// 角度の正規化
		parts->rot.x = RotNormalization(parts->rot.x);
		parts->rot.y = RotNormalization(parts->rot.y);
		parts->rot.z = RotNormalization(parts->rot.z);
	}
}

//=============================================================================
// パーツの設定
// Author : 唐﨑結斗
// 概要 : 行列を利用して、パーツの親子関係と描画設定を行う
//=============================================================================
void CMotion::SetParts(D3DXMATRIX mtxWorld)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CApplication::GetRenderer()->GetDevice();

	// テクスチャポインタの取得
	CTexture *pTexture = CApplication::GetTexture();

	// 計算用マトリックス
	D3DXMATRIX mtxRot, mtxTrans;
	D3DMATERIAL9 matDef;
	D3DXMATERIAL *pMat = {};

	for (int nCntParts = 0; nCntParts < m_nMaxParts; nCntParts++)
	{// ワールドマトリックスの初期化
		CMotion::Parts* parts = (m_parts + nCntParts);

		D3DXMatrixIdentity(&parts->mtxWorld);			// 行列初期化関数

		// 向きの反映
		D3DXMatrixRotationYawPitchRoll(&mtxRot,
			parts->rot.y,
			parts->rot.x,
			parts->rot.z);								// 行列回転関数

		D3DXMatrixMultiply(&parts->mtxWorld,
			&parts->mtxWorld,
			&mtxRot);									// 行列掛け算関数 

		// 位置を反映
		D3DXMatrixTranslation(&mtxTrans,
			parts->pos.x,
			parts->pos.y,
			parts->pos.z);								// 行列移動関数

		D3DXMatrixMultiply(&parts->mtxWorld,
			&parts->mtxWorld,
			&mtxTrans);									// 行列掛け算関数

		// 親パーツのワールドマトリックスを保持
		D3DXMATRIX mtxParent;

		if (parts->nIdxModelParent == -1)
		{// 親モデルのインデックス数が-1の時
		 // 新規深度値とZバッファの深度値が同じ値ならテスト成功にする
			mtxParent = mtxWorld;
		}
		else
		{// 新規深度値とZバッファの深度値が同じ値ならテスト成功にする
			mtxParent = (m_parts + parts->nIdxModelParent)->mtxWorld;
		}

		// 自分の親マトリックスとの掛け算
		D3DXMatrixMultiply(&parts->mtxWorld, &parts->mtxWorld, &mtxParent);

		// ワールドマトリックスの設定
		pDevice->SetTransform(D3DTS_WORLD, &parts->mtxWorld);

		// 現在のマテリアルを保持
		pDevice->GetMaterial(&matDef);

		// マテリアルデータへのポインタを取得
		pMat = (D3DXMATERIAL*)parts->material.pBuffer->GetBufferPointer();

		for (int nCntMat = 0; nCntMat < (int)parts->material.nNumMat; nCntMat++)
		{
			// マテリアル情報の設定
			D3DMATERIAL9  matD3D = pMat[nCntMat].MatD3D;

			if (parts->bSelect)
			{// 引数を色に設定
				matD3D.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);
			}

			// マテリアルの設定
			pDevice->SetMaterial(&matD3D);

			// テクスチャの設定
			pDevice->SetTexture(0, pTexture->GetTexture(parts->material.pNumTex[nCntMat]));

			// プレイヤーパーツの描画
			parts->material.pMesh->DrawSubset(nCntMat);

			// テクスチャの設定
			pDevice->SetTexture(0, nullptr);
		}

		// 保していたマテリアルを戻す
		pDevice->SetMaterial(&matDef);
	}

	// 新規深度値とZバッファの深度値が同じ値ならテスト成功にする
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
}

//=============================================================================
// パーツの設定
// Author : 唐﨑結斗
// 概要 : 行列を利用して、パーツの親子関係と描画設定、色設定を行う
//=============================================================================
void CMotion::SetParts(D3DXMATRIX mtxWorld, const D3DXCOLOR& col)
{// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CApplication::GetRenderer()->GetDevice();

	// テクスチャポインタの取得
	CTexture *pTexture = CApplication::GetTexture();

	// 計算用マトリックス
	D3DXMATRIX mtxRot, mtxTrans;
	D3DMATERIAL9 matDef;
	D3DXMATERIAL *pMat = {};

	for (int nCntParts = 0; nCntParts < m_nMaxParts; nCntParts++)
	{// ワールドマトリックスの初期化
		CMotion::Parts* parts = (m_parts + nCntParts);

		D3DXMatrixIdentity(&parts->mtxWorld);			// 行列初期化関数

		// 向きの反映
		D3DXMatrixRotationYawPitchRoll(&mtxRot,
			parts->rot.y,
			parts->rot.x,
			parts->rot.z);								// 行列回転関数

		D3DXMatrixMultiply(&parts->mtxWorld,
			&parts->mtxWorld,
			&mtxRot);									// 行列掛け算関数 

		// 位置を反映
		D3DXMatrixTranslation(&mtxTrans,
			parts->pos.x,
			parts->pos.y,
			parts->pos.z);								// 行列移動関数

		D3DXMatrixMultiply(&parts->mtxWorld,
			&parts->mtxWorld,
			&mtxTrans);									// 行列掛け算関数

		// 親パーツのワールドマトリックスを保持
		D3DXMATRIX mtxParent;

		if (parts->nIdxModelParent == -1)
		{// 親モデルのインデックス数が-1の時
		 // 新規深度値とZバッファの深度値が同じ値ならテスト成功にする
			mtxParent = mtxWorld;
		}
		else
		{// 新規深度値とZバッファの深度値が同じ値ならテスト成功にする
			mtxParent = (m_parts + parts->nIdxModelParent)->mtxWorld;
		}

		// 自分の親マトリックスとの掛け算
		D3DXMatrixMultiply(&parts->mtxWorld, &parts->mtxWorld, &mtxParent);

		// ワールドマトリックスの設定
		pDevice->SetTransform(D3DTS_WORLD, &parts->mtxWorld);

		// 現在のマテリアルを保持
		pDevice->GetMaterial(&matDef);

		// マテリアルデータへのポインタを取得
		pMat = (D3DXMATERIAL*)parts->material.pBuffer->GetBufferPointer();

		for (int nCntMat = 0; nCntMat < (int)parts->material.nNumMat; nCntMat++)
		{// マテリアル情報の設定
			D3DMATERIAL9  matD3D = pMat[nCntMat].MatD3D;

			// 引数を色に設定
			matD3D.Diffuse = col;

			// マテリアルの設定
			pDevice->SetMaterial(&matD3D);

			// テクスチャの設定
			pDevice->SetTexture(0, pTexture->GetTexture(parts->material.pNumTex[nCntMat]));

			// プレイヤーパーツの描画
			parts->material.pMesh->DrawSubset(nCntMat);

			// テクスチャの設定
			pDevice->SetTexture(0, nullptr);
		}

		// 保していたマテリアルを戻す
		pDevice->SetMaterial(&matDef);
	}

	// 新規深度値とZバッファの深度値が同じ値ならテスト成功にする
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
}

//=============================================================================
// パーツの選択
// Author : 唐﨑結斗
// 概要 : パーツの選択を行う
//=============================================================================
void CMotion::SelectParts(const int nNumParts)
{
	CMotion::Parts* parts = (m_parts + nNumParts);
	parts->bSelect = true;
}

//=============================================================================
// パーツ選択のリセット
// Author : 唐﨑結斗
// 概要 : パーツ選択のリセットを行う
//=============================================================================
void CMotion::ResetSelectParts()
{
	for (int nCntParts = 0; nCntParts < m_nMaxParts; nCntParts++)
	{
		CMotion::Parts* parts = (m_parts + nCntParts);
		parts->bSelect = false;
	}
}

//=============================================================================
// モーションの再生
// Author : 唐﨑結斗
// 概要 : 目的の位置まで特定のフレーム数で到着する処理をパーツごとに行う
//=============================================================================
void CMotion::PlayMotion()
{
	CMotion::MyMotion *motion = (m_motion + m_nNumMotion);

	for (int nCntParts = 0; nCntParts < m_nMaxParts; nCntParts++)
	{
		CMotion::Parts* parts = (m_parts + nCntParts);

		if (motion->nCntFrame == 0)
		{// フレームカウントが0の時
			// 目的の位置と向きの算出
			parts->posDest = (parts->posOrigin + motion->pKeySet[motion->nCntKeySet].pKey[nCntParts].pos) - parts->pos;
			parts->rotDest = (parts->rotOrigin + motion->pKeySet[motion->nCntKeySet].pKey[nCntParts].rot) - parts->rot;

			// 角度の正規化
			parts->rotDest.x = RotNormalization(parts->rotDest.x);
			parts->rotDest.y = RotNormalization(parts->rotDest.y);
			parts->rotDest.z = RotNormalization(parts->rotDest.z);
		}

		// 変数宣言
		D3DXVECTOR3 addPos = D3DXVECTOR3(parts->posDest / (float)(motion->pKeySet[motion->nCntKeySet].nFrame));
		D3DXVECTOR3 addRot = D3DXVECTOR3(parts->rotDest / (float)(motion->pKeySet[motion->nCntKeySet].nFrame));

		// 位置の加算
		parts->pos += addPos;

		//	向きの加算
		parts->rot += addRot;

		// 角度の正規化
		parts->rot.x = RotNormalization(parts->rot.x);
		parts->rot.y = RotNormalization(parts->rot.y);
		parts->rot.z = RotNormalization(parts->rot.z);
	}

	// フレームカウントの加算
	motion->nCntFrame++;

	if (motion->nCntFrame >= motion->pKeySet[motion->nCntKeySet].nFrame)
	{// フレームカウントが指定のフレーム数を超えた場合
		// フレーム数の初期化
		motion->nCntFrame = 0;

		// 再生中のキー番号数の加算
		motion->nCntKeySet++;

		if (motion->nCntKeySet >= motion->nNumKey && motion->bLoop)
		{// 再生中のキー数カウントがキー数の最大値を超えたとき、そのモーションがループを使用している
			// 再生中のキー数カウントを初期化
			motion->nCntKeySet = 0;

		}
		else if (motion->nCntKeySet >= motion->nNumKey)
		{
			motion->nCntKeySet = 0;
			m_bMotion = false;
		}
	}
}

//=============================================================================
// モーションブレンド
// Author : 唐﨑結斗
// 概要 : モーションとモーションのつなぎ目を調整する
// 返り値 : モーションブレンド使用の可否
//=============================================================================
void CMotion::MotionBlend()
{
	CMotion::MyMotion* motion = (m_motion + m_nNumMotion);

	for (int nCntParts = 0; nCntParts < m_nMaxParts; nCntParts++)
	{
		CMotion::Parts* parts = (m_parts + nCntParts);

		if (motion->nCntFrame == 0)
		{// フレームカウントが0の時
			// 目的の位置と向きの算出
			CMotion::MyKey myKey = motion->pKeySet[motion->nCntKeySet].pKey[nCntParts];
			parts->posDest = parts->posOrigin + myKey.pos - parts->pos;
			parts->rotDest = parts->rotOrigin + myKey.rot - parts->rot;

			// 角度の正規化
			parts->rotDest.x = RotNormalization(parts->rotDest.x);
			parts->rotDest.y = RotNormalization(parts->rotDest.y);
			parts->rotDest.z = RotNormalization(parts->rotDest.z);
		}

		// 変数宣言
		D3DXVECTOR3 addPos = D3DXVECTOR3(parts->posDest / (float)(MOTION_BLEND_FRAM));
		D3DXVECTOR3 addRot = D3DXVECTOR3(parts->rotDest / (float)(MOTION_BLEND_FRAM));

		// 位置の加算
		parts->pos += addPos;

		//	向きの加算
		parts->rot += addRot;

		// 角度の正規化
		parts->rotDest.x = RotNormalization(parts->rotDest.x);
		parts->rotDest.y = RotNormalization(parts->rotDest.y);
		parts->rotDest.z = RotNormalization(parts->rotDest.z);
	}

	// フレームカウントの加算
	motion->nCntFrame++;

	if (motion->nCntFrame >= MOTION_BLEND_FRAM)
	{// フレームカウントが指定のフレーム数を超えた場合

		motion->nCntFrame = 0;	// フレーム数の初期化
		motion->nCntKeySet++;	// 再生中のキー番号数の加算

		m_bMotionBlend = false;
	}
}

//=============================================================================
// パーツとモーションの読み込み
// Author : 唐﨑結斗
// 概要 : パーツとモーションの読み込み、初期化を呼び出す
//=============================================================================
void CMotion::LoodSetMotion(const char *pFileName)
{
	// モデルマネージャークラスの設定
	CModelManager *pModelManager = CApplication::GetModelManager();

	// 変数宣言
	char aString[128] = {};			// 文字列比較用の変数
	char g_aEqual[128] = {};		// ＝読み込み用変数
	int	nCntKeySet = 0;				// KeySetカウント
	int	nCntKey = 0;				// Keyカウント
	int nCntFileName = 0;			// ファイル名
	int nCntParts = 0;				// パーツ数のカウント
	int nCntMotion = 0;				// モーション数のカウント

	// ファイルポインタの宣言
	FILE * pFile;

	//ファイルを開く
	pFile = fopen(pFileName, "r");

	if (pFile != NULL)
	{//ファイルが開いた場合
		fscanf(pFile, "%s", &aString);

		while (strncmp(&aString[0], "SCRIPT", 6) != 0)
		{// 文字列が一致した場合
			aString[0] = {};
			// 文字列の読み込み
			fscanf(pFile, "%s", &aString[0]);
		}
		while (strncmp(&aString[0], "END_SCRIPT", 10) != 0)
		{
			fscanf(pFile, "%s", &aString[0]);

			if (strncmp(&aString[0], "#", 1) == 0)
			{// 一列読み込む
				fgets(&aString[0], sizeof(aString), pFile);
			}

			if (strcmp(&aString[0], "MODEL_FILENAME") == 0)
			{// ファイル名の読み込み
				fscanf(pFile, "%s", &g_aEqual[0]);
				fscanf(pFile, "%s", &m_partsFile[nCntFileName][0]);
				nCntFileName++;
			}

			if (strcmp(&aString[0], "CHARACTERSET") == 0)
			{// キャラクターの読み込み
				while (strcmp(&aString[0], "END_CHARACTERSET") != 0)
				{
					fscanf(pFile, "%s", &aString[0]);

					if (strncmp(&aString[0], "#", 1) == 0)
					{// 一列読み込む
						fgets(&aString[0], sizeof(aString), pFile);
					}

					if (strcmp(&aString[0], "NUM_PARTS") == 0)
					{// 読み込むパーツ数
						fscanf(pFile, "%s", &g_aEqual[0]);
						fscanf(pFile, "%d", &m_nMaxParts);

						// メモリの解放
						m_parts = new Parts[m_nMaxParts];
						m_motion = new MyMotion[MAX_MOTION];
						assert(m_parts != nullptr && m_motion != nullptr);
						ResetSelectParts();
					}

					if (strcmp(&aString[0], "PARTSSET") == 0)
					{// パーツの読み込み
						while (strcmp(&aString[0], "END_PARTSSET") != 0)
						{
							fscanf(pFile, "%s", &aString[0]);

							if (strncmp(&aString[0], "#", 1) == 0)
							{// 一列読み込む
								fgets(&aString[0], sizeof(aString), pFile);
							}

							if (strcmp(&aString[0], "INDEX") == 0)
							{// タイプの読み込み
								fscanf(pFile, "%s", &g_aEqual[0]);
								fscanf(pFile, "%d", &m_parts[nCntParts].nType);

								for (int nCntModel = 0; nCntModel < pModelManager->GetMaxModelMaterial(); nCntModel++)
								{
									if (strcmp(&m_partsFile[m_parts[nCntParts].nType][0], pModelManager->GetFileName(nCntModel)) == 0)
									{
										m_parts[nCntParts].nType = nCntModel;
										break;
									}
								}
							}
							if (strcmp(&aString[0], "PARENT") == 0)
							{// 親の読み込み
								fscanf(pFile, "%s", &g_aEqual[0]);
								fscanf(pFile, "%d", &m_parts[nCntParts].nIdxModelParent);
							}
							if (strcmp(&aString[0], "POS") == 0)
							{// 位置の読み込み
								fscanf(pFile, "%s", &g_aEqual[0]);
								fscanf(pFile, "%f", &m_parts[nCntParts].pos.x);
								fscanf(pFile, "%f", &m_parts[nCntParts].pos.y);
								fscanf(pFile, "%f", &m_parts[nCntParts].pos.z);
							}
							if (strcmp(&aString[0], "ROT") == 0)
							{// 向きの読み込み
								fscanf(pFile, "%s", &g_aEqual[0]);
								fscanf(pFile, "%f", &m_parts[nCntParts].rot.x);
								fscanf(pFile, "%f", &m_parts[nCntParts].rot.y);
								fscanf(pFile, "%f", &m_parts[nCntParts].rot.z);
							}
						}

						// パーツカウントのインクリメント
						nCntParts++;
					}
				}
			}
			if (strcmp(&aString[0], "MOTIONSET") == 0)
			{// モーションの読み込み
				while (strcmp(&aString[0], "END_MOTIONSET") != 0)
				{
					fscanf(pFile, "%s", &aString[0]);

					if (strncmp(&aString[0], "#", 1) == 0)
					{// 一列読み込む
						fgets(&aString[0], sizeof(aString), pFile);
					}

					if (strcmp(&aString[0], "LOOP") == 0)
					{// ループ有無の読み込み
						fscanf(pFile, "%s", &g_aEqual[0]);
						fscanf(pFile, "%d", (int*)(&m_motion[nCntMotion].bLoop));
					}
					if (strcmp(&aString[0], "NUM_KEY") == 0)
					{// キー数の読み込み
						fscanf(pFile, "%s", &g_aEqual[0]);
						fscanf(pFile, "%d", &m_motion[nCntMotion].nNumKey);

						// メモリの確保
						m_motion[nCntMotion].pKeySet = new MyKeySet[m_motion[nCntMotion].nNumKey];
						assert(m_motion[nCntMotion].pKeySet != nullptr);

						for (int nCntKeySet = 0; nCntKeySet < m_motion[nCntMotion].nNumKey; nCntKeySet++)
						{
							m_motion[nCntMotion].pKeySet[nCntKeySet].pKey = new MyKey[m_nMaxParts];
							assert(m_motion[nCntMotion].pKeySet[nCntKeySet].pKey != nullptr);
						}
					}
					if (strcmp(&aString[0], "KEYSET") == 0)
					{// キーセットの読み込み
						while (strcmp(&aString[0], "END_KEYSET") != 0)
						{
							fscanf(pFile, "%s", &aString[0]);

							if (strncmp(&aString[0], "#", 1) == 0)
							{// 一列読み込む
								fgets(&aString[0], sizeof(aString), pFile);
							}

							if (strcmp(&aString[0], "FRAME") == 0)
							{// フレーム数の読み込み
								fscanf(pFile, "%s", &g_aEqual[0]);
								fscanf(pFile, "%d", &m_motion[nCntMotion].pKeySet[nCntKeySet].nFrame);
							}
							if (strcmp(&aString[0], "KEY") == 0)
							{// キーの読み込み
								while (strcmp(&aString[0], "END_KEY") != 0)
								{
									fscanf(pFile, "%s", &aString[0]);

									if (strncmp(&aString[0], "#", 1) == 0)
									{// 一列読み込む
										fgets(&aString[0], sizeof(aString), pFile);
									}

									if (strcmp(&aString[0], "POS") == 0)
									{// 位置の読み込み
										fscanf(pFile, "%s", &g_aEqual[0]);
										fscanf(pFile, "%f", &m_motion[nCntMotion].pKeySet[nCntKeySet].pKey[nCntKey].pos.x);
										fscanf(pFile, "%f", &m_motion[nCntMotion].pKeySet[nCntKeySet].pKey[nCntKey].pos.y);
										fscanf(pFile, "%f", &m_motion[nCntMotion].pKeySet[nCntKeySet].pKey[nCntKey].pos.z);
									}
									if (strcmp(&aString[0], "ROT") == 0)
									{// 向きの読み込み
										fscanf(pFile, "%s", &g_aEqual[0]);
										fscanf(pFile, "%f", &m_motion[nCntMotion].pKeySet[nCntKeySet].pKey[nCntKey].rot.x);
										fscanf(pFile, "%f", &m_motion[nCntMotion].pKeySet[nCntKeySet].pKey[nCntKey].rot.y);
										fscanf(pFile, "%f", &m_motion[nCntMotion].pKeySet[nCntKeySet].pKey[nCntKey].rot.z);
									}
								}

								// キーカウントのインクリメント
								nCntKey++;
							}
						}

						// キーカウントの初期化
						nCntKey = 0;

						// キーセットカウントのインクリメント
						nCntKeySet++;
					}
				}
				// キーセットカウントの初期化
				nCntKeySet = 0;

				// パーツ情報のインクリメント
				nCntMotion++;
			}
		}

		//ファイルを閉じる
		fclose(pFile);
	}
	else
	{//ファイルが開けない場合
		assert(false);
	}

	// 初期化
	Init();
}

//=============================================================================
// 終了
// Author : 唐﨑結斗
// 概要 : メモリの解放
//=============================================================================
void CMotion::Uninit(void)
{
	if (m_parts != nullptr
		&& m_motion != nullptr)
	{// メモリの解放
		delete[] m_parts;
		delete[] m_motion;
		m_parts = nullptr;
	}
}
//=============================================================================
// パーツの位置をもとの位置に戻す
// Author : 唐﨑結斗
// 概要 : パーツの位置をもとの位置に戻す
//=============================================================================
void CMotion::SetPartsOrigin()
{
	for (int nCntParts = 0; nCntParts < m_nMaxParts; nCntParts++)
	{
		CMotion::Parts* parts = (m_parts + nCntParts);

		// 位置の設定
		parts->pos = parts->posOrigin;

		//	向きの設定
		parts->rot = parts->rotOrigin;
	}
}

//=============================================================================
// カウントのリセット
// Author : 唐﨑結斗
// 概要 : カウントのリセット
//=============================================================================
void CMotion::CntReset(const int nNumMotionOld)
{
	m_motion[nNumMotionOld].nCntFrame = 0;
	m_motion[nNumMotionOld].nCntKeySet = 0;
}

//=============================================================================
// モーションの再読み込み
// Author : 唐﨑結斗
// 概要 : 現在のモーションを破棄して、別のモーションを読み込む
//=============================================================================
void CMotion::ReloadMotion(const char * pFileName)
{// 終了
	Uninit();

	// パーツ名の初期化
	memset(&m_partsFile, 0, sizeof(m_partsFile));

	// モーションの初期化
	m_motion = nullptr;

	// パーツの初期化
	m_parts = nullptr;

	// パーツ数の初期化
	m_nMaxParts = 0;

	// モーションの読み込み
	LoodSetMotion(pFileName);
}

//=============================================================================
// モーション番号の設定
// Author : 唐﨑結斗
// 概要 : モーションの番号の設定
//=============================================================================
void CMotion::SetNumMotion(const int nNumMotion)
{
	// モーションカウントのリセット
	CntReset(m_nNumMotion);

	// モーション番号の設定
	m_nNumMotion = nNumMotion;

	// モーションブレンドを行う
	m_bMotionBlend = true;
	m_bMotion = true;
}

//=============================================================================
// パーツの元の位置の設定
// Author : 唐﨑結斗
// 概要 : 引数番目のパーツの元の位置を設定する
//=============================================================================
void CMotion::SetPosOrigin(const int nNumParts, const D3DXVECTOR3 pos)
{// パーツ情報の取得
	CMotion::Parts* parts = (m_parts + nNumParts);

	parts->posOrigin = pos;
}

//=============================================================================
// パーツの元の位置の取得
// Author : 唐﨑結斗
// 概要 : 引数番目のパーツの元の位置を取得する
//=============================================================================
D3DXVECTOR3 CMotion::GetPosOrigin(const int nNumParts)
{// パーツ情報の取得
	CMotion::Parts* parts = (m_parts + nNumParts);

	return parts->posOrigin;
}


