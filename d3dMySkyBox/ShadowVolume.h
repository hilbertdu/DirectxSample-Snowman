#pragma once

//-----------------------------------------------------------------------------
//           Name: shadow_volume.h
//         Author: Mostly from a DX9 SDK sample
// Last Modified: 02/01/05
// Description: Simple class for grouping together as that's needed to 
//                 create and use a shadow volume.
//
// Note: The buildShadowVolume() method contains a data type called MeshVertex,
//       which is basically hard-coded to work with my sample's teapot mesh.
//       If you want to use another mesh instead of the teapot, you'll need to
//       find out what the FVF layout is for its vertex type and substitute it
//       there for MeshVertex.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Modified by Du Li. 2014.02.18 - 修改于2014.02.18
//-----------------------------------------------------------------------------

#include "stdafx.h"

class CShadowVolume
{
public:
    HRESULT BuildShadowVolume( LPD3DXMESH pObject, D3DXVECTOR3 vLight );
	HRESULT Render( LPDIRECT3DDEVICE9 pd3dDevice );

	void    Reset() { m_dwNumVertices = 0L; }

private:
	void AddEdge( WORD* pEdges, DWORD& dwNumEdges, WORD v0, WORD v1 );

	D3DXVECTOR3 m_pVertices[32000]; // 阴影体的顶点坐标
    DWORD       m_dwNumVertices;
};