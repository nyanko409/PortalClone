#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

#include "main.h"
#include "renderer.h"
#include "model.h"


std::vector<std::pair<ModelType, std::shared_ptr<CModel>>>  ModelManager::m_modelDatas;
std::vector<std::pair<ModelType, const char*>> ModelManager::m_modelPaths =
{
	{ModelType::MODEL_BULLET, "asset\\model\\Bullet.obj"},
	{ModelType::MODEL_PLAYER, "asset\\model\\gobleta.obj"},
};


void ModelManager::GetModel(ModelType type, std::shared_ptr<CModel>& pOutModel)
{
	// search for already loaded models
	for (int i = 0; i < m_modelDatas.size(); ++i)
	{
		// found the pointer to the mesh
		if (type == m_modelDatas[i].first)
		{
			// if the shared pointer has released the memory, delete the vector and create it again
			if (m_modelDatas[i].second == nullptr)
			{
				m_modelDatas.erase(m_modelDatas.begin() + i);
				break;
			}

			// assign the pointer and return
			pOutModel = m_modelDatas[i].second;
			return;
		}
	}

	// mesh not loaded yet, so load it and return the pointer to it
	pOutModel = Load(type);
}

void ModelManager::LoadModelIntoMemory(ModelType type)
{
	// return if the type is already loaded into memory
	if (IsLoaded(type))
		return;

	// else load
	Load(type);
}

bool ModelManager::IsLoaded(ModelType type)
{
	for (int i = 0; i < m_modelDatas.size(); ++i)
	{
		if (m_modelDatas[i].first == type)
			return true;
	}

	return false;
}

std::shared_ptr<CModel> ModelManager::Load(ModelType type)
{
	// load the model type into memory
	for (int i = 0; i < m_modelPaths.size(); ++i)
	{
		if (m_modelPaths[i].first == type)
		{
			m_modelDatas.emplace_back(std::make_pair(type, std::make_shared<CModel>(m_modelPaths[i].second)));
			return m_modelDatas.back().second;
		}
	}
}



void CModel::Draw()
{
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	CRenderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

	// インデックスバッファ設定
	CRenderer::GetDeviceContext()->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// プリミティブトポロジ設定
	CRenderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	for( unsigned int i = 0; i < m_SubsetNum; i++ )
	{
		// マテリアル設定
		CRenderer::SetMaterial( m_SubsetArray[i].Material.Material );

		// テクスチャ設定
		CRenderer::GetDeviceContext()->PSSetShaderResources( 0, 1, &m_SubsetArray[i].Material.Texture );

		// ポリゴン描画
		CRenderer::GetDeviceContext()->DrawIndexed( m_SubsetArray[i].IndexNum, m_SubsetArray[i].StartIndex, 0 );
	}
}

void CModel::Load( const char *FileName )
{

	Mesh model;
	LoadObj( FileName, &model );



	// 頂点バッファ生成
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory( &bd, sizeof(bd) );
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof( VERTEX_3D ) * model.VertexNum;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory( &sd, sizeof(sd) );
		sd.pSysMem = model.VertexArray;

		CRenderer::GetDevice()->CreateBuffer( &bd, &sd, &m_VertexBuffer );
	}


	// インデックスバッファ生成
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory( &bd, sizeof(bd) );
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof( unsigned int ) * model.IndexNum;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory( &sd, sizeof(sd) );
		sd.pSysMem = model.IndexArray;

		CRenderer::GetDevice()->CreateBuffer( &bd, &sd, &m_IndexBuffer );
	}

	// サブセット設定
	{
		m_SubsetArray = new SUBSET[ model.SubsetNum ];
		m_SubsetNum = model.SubsetNum;

		for( unsigned int i = 0; i < model.SubsetNum; i++ )
		{
			m_SubsetArray[i].StartIndex = model.SubsetArray[i].StartIndex;
			m_SubsetArray[i].IndexNum = model.SubsetArray[i].IndexNum;

			m_SubsetArray[i].Material.Material = model.SubsetArray[i].Material.Material;

			m_SubsetArray[i].Material.Texture = NULL;

			D3DX11CreateShaderResourceViewFromFile(CRenderer::GetDevice(),
				model.SubsetArray[i].Material.TextureName,
				NULL,
				NULL,
				&m_SubsetArray[i].Material.Texture,
				NULL);

			assert(m_SubsetArray[i].Material.Texture);

		}
	}

	delete[] model.VertexArray;
	delete[] model.IndexArray;
	delete[] model.SubsetArray;
}

void CModel::Unload()
{
	m_VertexBuffer->Release();
	m_IndexBuffer->Release();

	for (unsigned int i = 0; i < m_SubsetNum; i++)
	{
		m_SubsetArray[i].Material.Texture->Release();
	}

	delete[] m_SubsetArray;

}

//モデル読込////////////////////////////////////////////
void CModel::LoadObj( const char *FileName, Mesh *mesh )
{
	char dir[MAX_PATH];
	strcpy (dir, FileName );
	PathRemoveFileSpec(dir);

	dx::XMFLOAT3	*positionArray;
	dx::XMFLOAT3	*normalArray;
	dx::XMFLOAT2	*texcoordArray;

	unsigned int	positionNum = 0;
	unsigned int	normalNum = 0;
	unsigned int	texcoordNum = 0;
	unsigned int	vertexNum = 0;
	unsigned int	indexNum = 0;
	unsigned int	in = 0;
	unsigned int	subsetNum = 0;

	MODEL_MATERIAL	*materialArray = NULL;
	unsigned int	materialNum = 0;

	char str[256];
	char *s;
	char c;


	FILE *file;
	file = fopen( FileName, "rt" );
	assert(file);



	//要素数カウント
	while( true )
	{
		fscanf( file, "%s", str );

		if( feof( file ) != 0 )
			break;

		if( strcmp( str, "v" ) == 0 )
		{
			positionNum++;
		}
		else if( strcmp( str, "vn" ) == 0 )
		{
			normalNum++;
		}
		else if( strcmp( str, "vt" ) == 0 )
		{
			texcoordNum++;
		}
		else if( strcmp( str, "usemtl" ) == 0 )
		{
			subsetNum++;
		}
		else if( strcmp( str, "f" ) == 0 )
		{
			in = 0;

			do
			{
				fscanf( file, "%s", str );
				vertexNum++;
				in++;
				c = fgetc( file );
			}
			while( c != '\n' && c!= '\r' );

			//四角は三角に分割
			if( in == 4 )
				in = 6;

			indexNum += in;
		}
	}


	//メモリ確保
	positionArray = new dx::XMFLOAT3[ positionNum ];
	normalArray = new dx::XMFLOAT3[ normalNum ];
	texcoordArray = new dx::XMFLOAT2[ texcoordNum ];


	mesh->VertexArray = new VERTEX_3D[ vertexNum ];
	mesh->VertexNum = vertexNum;

	mesh->IndexArray = new unsigned int[ indexNum ];
	mesh->IndexNum = indexNum;

	mesh->SubsetArray = new SUBSET[ subsetNum ];
	mesh->SubsetNum = subsetNum;




	//要素読込
	dx::XMFLOAT3 *position = positionArray;
	dx::XMFLOAT3 *normal = normalArray;
	dx::XMFLOAT2 *texcoord = texcoordArray;

	unsigned int vc = 0;
	unsigned int ic = 0;
	unsigned int sc = 0;


	fseek( file, 0, SEEK_SET );

	while( true )
	{
		fscanf( file, "%s", str );

		if( feof( file ) != 0 )
			break;

		if( strcmp( str, "mtllib" ) == 0 )
		{
			//マテリアルファイル
			fscanf( file, "%s", str );

			char path[256];
			strcpy( path, dir );
			strcat( path, "\\" );
			strcat( path, str );

			LoadMaterial( path, &materialArray, &materialNum );
		}
		else if( strcmp( str, "o" ) == 0 )
		{
			//オブジェクト名
			fscanf( file, "%s", str );
		}
		else if( strcmp( str, "v" ) == 0 )
		{
			//頂点座標
			fscanf( file, "%f", &position->x );
			fscanf( file, "%f", &position->y );
			fscanf( file, "%f", &position->z );
			position++;
		}
		else if( strcmp( str, "vn" ) == 0 )
		{
			//法線
			fscanf( file, "%f", &normal->x );
			fscanf( file, "%f", &normal->y );
			fscanf( file, "%f", &normal->z );
			normal++;
		}
		else if( strcmp( str, "vt" ) == 0 )
		{
			//テクスチャ座標
			fscanf( file, "%f", &texcoord->x );
			fscanf( file, "%f", &texcoord->y );
			texcoord->y = 1.0f - texcoord->y;
			texcoord++;
		}
		else if( strcmp( str, "usemtl" ) == 0 )
		{
			//マテリアル
			fscanf( file, "%s", str );

			if( sc != 0 )
				mesh->SubsetArray[ sc - 1 ].IndexNum = ic - mesh->SubsetArray[ sc - 1 ].StartIndex;

			mesh->SubsetArray[ sc ].StartIndex = ic;


			for( unsigned int i = 0; i < materialNum; i++ )
			{
				if( strcmp( str, materialArray[i].Name ) == 0 )
				{
					mesh->SubsetArray[ sc ].Material.Material = materialArray[i].Material;
					strcpy( mesh->SubsetArray[ sc ].Material.TextureName, materialArray[i].TextureName );
					strcpy( mesh->SubsetArray[ sc ].Material.Name, materialArray[i].Name );

					break;
				}
			}

			sc++;
			
		}
		else if( strcmp( str, "f" ) == 0 )
		{
			//面
			in = 0;

			do
			{
				fscanf( file, "%s", str );

				s = strtok( str, "/" );	
				mesh->VertexArray[vc].Position = positionArray[ atoi( s ) - 1 ];
				if( s[ strlen( s ) + 1 ] != '/' )
				{
					//テクスチャ座標が存在しない場合もある
					s = strtok( NULL, "/" );
					mesh->VertexArray[vc].TexCoord = texcoordArray[ atoi( s ) - 1 ];
				}
				s = strtok( NULL, "/" );	
				mesh->VertexArray[vc].Normal = normalArray[ atoi( s ) - 1 ];

				mesh->VertexArray[vc].Diffuse = dx::XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );

				mesh->IndexArray[ic] = vc;
				ic++;
				vc++;

				in++;
				c = fgetc( file );
			}
			while( c != '\n' && c != '\r' );

			//四角は三角に分割
			if( in == 4 )
			{
				mesh->IndexArray[ic] = vc - 4;
				ic++;
				mesh->IndexArray[ic] = vc - 2;
				ic++;
			}
		}
	}


	if( sc != 0 )
		mesh->SubsetArray[ sc - 1 ].IndexNum = ic - mesh->SubsetArray[ sc - 1 ].StartIndex;

	fclose(file);

	delete[] positionArray;
	delete[] normalArray;
	delete[] texcoordArray;
	delete[] materialArray;
}

//マテリアル読み込み///////////////////////////////////////////////////////////////////
void CModel::LoadMaterial( const char *FileName, MODEL_MATERIAL **MaterialArray, unsigned int *MaterialNum )
{

	char dir[MAX_PATH];
	strcpy(dir, FileName);
	PathRemoveFileSpec(dir);



	char str[256];

	FILE *file;
	file = fopen( FileName, "rt" );
	assert(file);

	MODEL_MATERIAL *materialArray;
	unsigned int materialNum = 0;

	//要素数カウント
	while( true )
	{
		fscanf( file, "%s", str );

		if( feof( file ) != 0 )
			break;


		if( strcmp( str, "newmtl" ) == 0 )
		{
			materialNum++;
		}
	}


	//メモリ確保
	materialArray = new MODEL_MATERIAL[ materialNum ];


	//要素読込
	int mc = -1;

	fseek( file, 0, SEEK_SET );

	while( true )
	{
		fscanf( file, "%s", str );

		if( feof( file ) != 0 )
			break;


		if( strcmp( str, "newmtl" ) == 0 )
		{
			//マテリアル名
			mc++;
			fscanf( file, "%s", materialArray[ mc ].Name );
			strcpy( materialArray[ mc ].TextureName, "" );

			materialArray[mc].Material.Emission.x = 0.0f;
			materialArray[mc].Material.Emission.y = 0.0f;
			materialArray[mc].Material.Emission.z = 0.0f;
			materialArray[mc].Material.Emission.w = 0.0f;
		}
		else if( strcmp( str, "Ka" ) == 0 )
		{
			//アンビエント
			fscanf( file, "%f", &materialArray[ mc ].Material.Ambient.x );
			fscanf( file, "%f", &materialArray[ mc ].Material.Ambient.y );
			fscanf( file, "%f", &materialArray[ mc ].Material.Ambient.z );
			materialArray[ mc ].Material.Ambient.w = 1.0f;
		}
		else if( strcmp( str, "Kd" ) == 0 )
		{
			//ディフューズ
			fscanf( file, "%f", &materialArray[ mc ].Material.Diffuse.x );
			fscanf( file, "%f", &materialArray[ mc ].Material.Diffuse.y );
			fscanf( file, "%f", &materialArray[ mc ].Material.Diffuse.z );
			materialArray[ mc ].Material.Diffuse.w = 1.0f;
		}
		else if( strcmp( str, "Ks" ) == 0 )
		{
			//スペキュラ
			fscanf( file, "%f", &materialArray[ mc ].Material.Specular.x );
			fscanf( file, "%f", &materialArray[ mc ].Material.Specular.y );
			fscanf( file, "%f", &materialArray[ mc ].Material.Specular.z );
			materialArray[ mc ].Material.Specular.w = 1.0f;
		}
		else if( strcmp( str, "Ns" ) == 0 )
		{
			//スペキュラ強度
			fscanf( file, "%f", &materialArray[ mc ].Material.Shininess );
		}
		else if( strcmp( str, "d" ) == 0 )
		{
			//アルファ
			fscanf( file, "%f", &materialArray[ mc ].Material.Diffuse.w);
		}
		else if( strcmp( str, "map_Kd" ) == 0 )
		{
			//テクスチャ
			fscanf( file, "%s", str );

			char path[256];
			strcpy( path, dir );
			strcat( path, "\\texture\\" );
			strcat( path, str );

			strcat( materialArray[ mc ].TextureName, path );
		}
	}

	fclose(file);

	*MaterialArray = materialArray;
	*MaterialNum = materialNum;
}
