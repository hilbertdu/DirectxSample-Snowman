#include "stdafx.h"
#include "ShadowVolume.h"

//-----------------------------------------------------------------------------
// Name: render
// Desc:
//-----------------------------------------------------------------------------
HRESULT CShadowVolume::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
    pd3dDevice->SetFVF( D3DFVF_XYZ );

    return pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLELIST, m_dwNumVertices/3,
                                        m_pVertices, sizeof(D3DXVECTOR3) );
}

//-----------------------------------------------------------------------------
// Name: buildShadowVolume()
// Desc: Takes a mesh as input, and uses it to build a shadow volume. The
//       technique used considers each triangle of the mesh, and adds it's
//       edges to a temporary list. The edge list is maintained, such that
//       only silohuette edges are kept. Finally, the silohuette edges are
//       extruded to make the shadow volume vertex list.
//-----------------------------------------------------------------------------
HRESULT CShadowVolume::BuildShadowVolume( LPD3DXMESH pMesh, D3DXVECTOR3 vLight )
{
	// pMesh的顶点结构
	struct MeshVertex {
		D3DXVECTOR3 p;
		D3DXVECTOR3 n;
		float u, v;
	};

    MeshVertex *pVertices;
    WORD       *pIndices;

    // 锁缓存
    pMesh->LockVertexBuffer( 0L, (LPVOID*)&pVertices );
    pMesh->LockIndexBuffer( 0L, (LPVOID*)&pIndices );
    DWORD dwNumFaces    = pMesh->GetNumFaces();

    // 分配一个临时的索引数组
    WORD *pEdges = new WORD[dwNumFaces*6];

    if( pEdges == NULL )
    {
        pMesh->UnlockVertexBuffer();
        pMesh->UnlockIndexBuffer();
        return E_OUTOFMEMORY;
    }
	
    DWORD dwNumEdges = 0;

    // 对每个片面进行计算
    for( DWORD i = 0; i < dwNumFaces; ++i )
    {
        WORD wFace0 = pIndices[3*i+0];
        WORD wFace1 = pIndices[3*i+1];
        WORD wFace2 = pIndices[3*i+2];

		// 一个面的三个顶点坐标
        D3DXVECTOR3 v0 = pVertices[wFace0].p;
        D3DXVECTOR3 v1 = pVertices[wFace1].p;
        D3DXVECTOR3 v2 = pVertices[wFace2].p;

        // 计算法线是否向光
        D3DXVECTOR3 vCross1(v2-v1);
        D3DXVECTOR3 vCross2(v1-v0);
        D3DXVECTOR3 vNormal;
        D3DXVec3Cross( &vNormal, &vCross1, &vCross2 );

        if( D3DXVec3Dot( &vNormal, &vLight ) >= 0.0f )
        {
            AddEdge( pEdges, dwNumEdges, wFace0, wFace1 );
            AddEdge( pEdges, dwNumEdges, wFace1, wFace2 );
            AddEdge( pEdges, dwNumEdges, wFace2, wFace0 );
        }
    }

	// pEdges中仅剩pMesh的边缘顶点，对每条边的两个顶点按光照的方向进行延伸
	// 最终构建一个完整的阴影体
    for( DWORD i = 0; i < dwNumEdges; ++i )
    {
        D3DXVECTOR3 v1 = pVertices[pEdges[2*i+0]].p;
        D3DXVECTOR3 v2 = pVertices[pEdges[2*i+1]].p;
        D3DXVECTOR3 v3 = v1 + vLight*200;
        D3DXVECTOR3 v4 = v2 + vLight*200;

        // 封边操作
        m_pVertices[m_dwNumVertices++] = v1;
        m_pVertices[m_dwNumVertices++] = v2;
        m_pVertices[m_dwNumVertices++] = v3;

        m_pVertices[m_dwNumVertices++] = v2;
        m_pVertices[m_dwNumVertices++] = v4;
        m_pVertices[m_dwNumVertices++] = v3;
    }

    // Delete the temporary edge list
    delete[] pEdges;

    // Unlock the geometry buffers
    pMesh->UnlockVertexBuffer();
    pMesh->UnlockIndexBuffer();

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: addEdge()
// Desc: Adds an edge to a list of silohuette edges of a shadow volume.
//-----------------------------------------------------------------------------
void CShadowVolume::AddEdge( WORD* pEdges, DWORD& dwNumEdges, WORD v0, WORD v1 )
{
    // 去掉重复增加的边
    for( DWORD i = 0; i < dwNumEdges; ++i )
    {
        if( ( pEdges[2*i+0] == v0 && pEdges[2*i+1] == v1 ) ||
            ( pEdges[2*i+0] == v1 && pEdges[2*i+1] == v0 ) )
        {
            if( dwNumEdges > 1 )
            {
                pEdges[2*i+0] = pEdges[2*(dwNumEdges-1)+0];
                pEdges[2*i+1] = pEdges[2*(dwNumEdges-1)+1];
            }

            --dwNumEdges;
            return;
        }
    }

    pEdges[2*dwNumEdges+0] = v0;
    pEdges[2*dwNumEdges+1] = v1;
    dwNumEdges++;
}