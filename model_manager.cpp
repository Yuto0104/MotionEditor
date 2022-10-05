//=============================================================================
//
// モデルマネージャー処理(model_manager.cpp)
// Author : 唐﨑結斗
// 概要 : モデルマネージャーの設定を行う
//
//=============================================================================

//*****************************************************************************
// インクルード
//*****************************************************************************
#include <assert.h>
#include <stdio.h>

#include "model_manager.h"
#include "renderer.h" 
#include "application.h"
#include "texture.h"

//=============================================================================
// コンストラクタ
// Author : 唐﨑結斗
// 概要 : インスタンス生成時に行う処理
//=============================================================================
CModelManager::CModelManager()
{
	m_pModelMaterial = nullptr;		// モデルのマテリアル情報
	m_nMaxModelMaterial = 0;			// モデルの種別数
}

//=============================================================================
// デストラクタ
// Author : 唐﨑結斗
// 概要 : インスタンス終了時に行う処理
//=============================================================================
CModelManager::~CModelManager()
{

}

//=============================================================================
// 初期化
// Author : 唐﨑結斗
// 概要 : 貼り付けるテクスチャ情報を格納する
//=============================================================================
void CModelManager::Init(void)
{// レンダラーのゲット
	CRenderer *pRenderer = CApplication::GetRenderer();

	// テクスチャポインタの取得
	CTexture *pTexture = CApplication::GetTexture();

	// テクスチャ情報の取得
	CTexture::TEXTURE *pTextureData = pTexture->GetTextureData();

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice();

	// ファイル読み込み
	LoadFile("data/FILE/data.txt");

	// 背景モデルの設置
	LoadFile("data/FILE/BG_model.txt");

	for (int nCntModel = 0; nCntModel < m_nMaxModelMaterial; nCntModel++)
	{// Xファイルの読み込み
		D3DXLoadMeshFromX(&m_pModelMaterial[nCntModel].aFileName[0],
			D3DXMESH_SYSTEMMEM,
			pDevice,
			NULL,
			&m_pModelMaterial[nCntModel].pBuffer,
			NULL,
			&m_pModelMaterial[nCntModel].nNumMat,
			&m_pModelMaterial[nCntModel].pMesh);

		m_pModelMaterial[nCntModel].pNumTex = new int[m_pModelMaterial[nCntModel].nNumMat];
		assert(m_pModelMaterial[nCntModel].pNumTex != nullptr);
		memset(&m_pModelMaterial[nCntModel].pNumTex[0], -1, sizeof(m_pModelMaterial[nCntModel].pNumTex));

		// バッファの先頭ポインタをD3DXMATERIALにキャストして取得
		D3DXMATERIAL *pMat = (D3DXMATERIAL*)m_pModelMaterial[nCntModel].pBuffer->GetBufferPointer();

		// 各メッシュのマテリアル情報を取得する
		for (int nCntMat = 0; nCntMat < (int)m_pModelMaterial[nCntModel].nNumMat; nCntMat++)
		{
			m_pModelMaterial[nCntModel].pNumTex[nCntMat] = -1;

			if (pMat[nCntMat].pTextureFilename != NULL)
			{
				int nMaxTex = pTexture->GetMaxTexture();
				for (int nCntTexture = 0; nCntTexture < nMaxTex; nCntTexture++)
				{
					if (strcmp(pMat[nCntMat].pTextureFilename, pTextureData[nCntTexture].aFileName) == 0)
					{
						m_pModelMaterial[nCntModel].pNumTex[nCntMat] = nCntTexture;
						break;
					}
				}
			}
		}
	}
}

//=============================================================================
// 終了
// Author : 唐﨑結斗
// 概要 : テクスチャの解放
//=============================================================================
void CModelManager::Uninit(void)
{
	for (int nCnt = 0; nCnt < m_nMaxModelMaterial; nCnt++)
	{
		// メッシュの破棄
		if (m_pModelMaterial[nCnt].pMesh != nullptr)
		{
			m_pModelMaterial[nCnt].pMesh->Release();
			m_pModelMaterial[nCnt].pMesh = nullptr;
		}

		// マテリアルの破棄
		if (m_pModelMaterial[nCnt].pBuffer != nullptr)
		{
			m_pModelMaterial[nCnt].pBuffer->Release();
			m_pModelMaterial[nCnt].pBuffer = nullptr;
		}

		// メモリの解放
		delete[] m_pModelMaterial[nCnt].pNumTex;
		m_pModelMaterial[nCnt].pNumTex = nullptr;
	}

	// メモリの解放
	delete[] m_pModelMaterial;
	m_pModelMaterial = nullptr;
}

//=============================================================================
// ファイルの読み込み
// Author : 唐﨑結斗
// 概要 : ファイルの読み込み
//=============================================================================
void CModelManager::LoadFile(const char *pFileName)
{
	// 変数宣言
	char aStr[128];
	int nCntModel = 0;
	int nCntSetModel = 0;

	// ファイルの読み込み
	FILE *pFile = fopen(pFileName, "r");

	if (pFile != nullptr)
	{
		while (fscanf(pFile, "%s", &aStr[0]) != EOF)
		{// "EOF"を読み込むまで 
			if (strncmp(&aStr[0], "#", 1) == 0)
			{// 一列読み込む
				fgets(&aStr[0], sizeof(aStr), pFile);
			}

			if (strstr(&aStr[0], "MAX_TYPE") != NULL)
			{
				fscanf(pFile, "%s", &aStr[0]);
				fscanf(pFile, "%d", &m_nMaxModelMaterial);
				m_pModelMaterial = new CModel3D::MODEL_MATERIAL[m_nMaxModelMaterial];
				assert(m_pModelMaterial != nullptr);
				memset(&m_pModelMaterial[0], 0, sizeof(CModel3D::MODEL_MATERIAL));
			}

			if (strstr(&aStr[0], "NUM_MODEL") != NULL)
			{
				fscanf(pFile, "%s", &aStr[0]);
				fscanf(pFile, "%d", &m_nMaxSetModelBG);
				m_pObjectSeter = new OBJECT_SETER[m_nMaxSetModelBG];
				assert(m_pObjectSeter != nullptr);
				memset(&m_pObjectSeter[0], 0, sizeof(OBJECT_SETER));
			}

			if (strstr(&aStr[0], "MODEL_FILENAME") != NULL)
			{
				fscanf(pFile, "%s", &aStr[0]);
				fscanf(pFile, "%s", &m_pModelMaterial[nCntModel].aFileName[0]);
				nCntModel++;
			}

			if (strstr(&aStr[0], "MODELSET") != NULL)
			{
				while (strstr(&aStr[0], "END_MODELSET") == NULL)
				{
					fscanf(pFile, "%s", &aStr[0]);

					if (strncmp(&aStr[0], "#", 1) == 0)
					{// 一列読み込む
						fgets(&aStr[0], sizeof(aStr), pFile);
					}

					if (strstr(&aStr[0], "POS") != NULL)
					{// モデルのファイル名の設定
						fscanf(pFile, "%s", &aStr[0]);
						fscanf(pFile, "%f", &m_pObjectSeter[nCntSetModel].pos.x);
						fscanf(pFile, "%f", &m_pObjectSeter[nCntSetModel].pos.y);
						fscanf(pFile, "%f", &m_pObjectSeter[nCntSetModel].pos.z);
					}

					if (strstr(&aStr[0], "ROT") != NULL)
					{// モデルのファイル名の設定
						fscanf(pFile, "%s", &aStr[0]);
						fscanf(pFile, "%f", &m_pObjectSeter[nCntSetModel].rot.x);
						fscanf(pFile, "%f", &m_pObjectSeter[nCntSetModel].rot.y);
						fscanf(pFile, "%f", &m_pObjectSeter[nCntSetModel].rot.z);
					}

					if (strstr(&aStr[0], "SCALE") != NULL)
					{// モデルのファイル名の設定
						fscanf(pFile, "%s", &aStr[0]);
						fscanf(pFile, "%f", &m_pObjectSeter[nCntSetModel].size.x);
						fscanf(pFile, "%f", &m_pObjectSeter[nCntSetModel].size.y);
						fscanf(pFile, "%f", &m_pObjectSeter[nCntSetModel].size.z);
					}

					if (strcmp(&aStr[0], "TYPE") == 0)
					{// キー数の読み込み
						fscanf(pFile, "%s", &aStr[0]);
						fscanf(pFile, "%d", &m_pObjectSeter[nCntSetModel].nID);
					}
				}

				nCntSetModel++;
			}
		}
	}
	else
	{
		assert(false);
	}
}

//=============================================================================
// モデルの設定
// Author : 唐﨑結斗
// 概要 : モデルの設定を行う
//=============================================================================
void CModelManager::SetModelBG()
{
	CModel3D *pModel = nullptr;

	for (int nCntSetModel = 0; nCntSetModel < m_nMaxSetModelBG; nCntSetModel++)
	{
		// インスタン生成
		pModel = CModel3D::Create(m_pObjectSeter[nCntSetModel].nID);
		pModel->SetPos(m_pObjectSeter[nCntSetModel].pos);
		pModel->SetRot(m_pObjectSeter[nCntSetModel].rot);
		pModel->SetSize(m_pObjectSeter[nCntSetModel].size);
	}
}

