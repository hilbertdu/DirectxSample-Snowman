#pragma once

/*
 * d3d实用公共库，定义了若干顶点数据结构，颜色，材质，模板函数，向量操作等
 */

#include "stdafx.h"
//#include "Camera.h"
//#include "SkyBox.h"

#pragma comment(lib,"d3d9.lib") 
#pragma comment(lib,"winmm.lib") 
#pragma comment(lib,"d3dx9.lib")

namespace d3d
{
    template<class T> void Release(T t)
    {
        if (t)
        {
            t->Release();
            t = 0;
        }
    }

    template<class T> void Delete(T t)
    {
        if (t)
        {
            delete t;
            t = 0;
        }
    }

    template<class T> void Render(T element)
    {
        if (element)
        {
            element->Render();
        }
    }

    // 不同的顶点格式
    struct TVertex
    {
        TVertex(){}
        TVertex(
            float x, float y, float z,
            float u, float v)
        {
            _x = x;  _y = y;  _z = z;
            _u = u;  _v = v;
        }
        float _x, _y, _z;
        float _u, _v; // 纹理坐标

        static const DWORD FVF;
    };

    struct NTVertex
    {
        NTVertex(){}
        NTVertex(float x, float y, float z,
            float nx, float ny, float nz,
            float u, float v)
        {
            _x = x;  _y = y;  _z = z;
            _nx = nx; _ny = ny; _nz = nz;
            _u = u;  _v = v;
        }
        float _x, _y, _z;
        float _nx, _ny, _nz;
        float _u, _v;

        static const DWORD FVF;
    };

    bool DefaultDisplay(float timeDelta);
    void BeginRender(IDirect3DDevice9* _d3dDevice9);
    void EndRender(IDirect3DDevice9* _d3dDevice9);

    // 颜色
    const D3DXCOLOR      WHITE(D3DCOLOR_XRGB(255, 255, 255));
    const D3DXCOLOR      BLACK(D3DCOLOR_XRGB(0, 0, 0));
    const D3DXCOLOR        RED(D3DCOLOR_XRGB(255, 0, 0));
    const D3DXCOLOR      GREEN(D3DCOLOR_XRGB(0, 255, 0));
    const D3DXCOLOR       BLUE(D3DCOLOR_XRGB(0, 0, 255));
    const D3DXCOLOR     YELLOW(D3DCOLOR_XRGB(255, 255, 0));
    const D3DXCOLOR       CYAN(D3DCOLOR_XRGB(0, 255, 255));
    const D3DXCOLOR    MAGENTA(D3DCOLOR_XRGB(255, 0, 255));
    const D3DXCOLOR    DARK(D3DCOLOR_XRGB(64, 64, 64));

    const D3DXCOLOR BEACH_SAND(D3DCOLOR_XRGB(255, 249, 157));
    const D3DXCOLOR DESERT_SAND(D3DCOLOR_XRGB(250, 205, 135));

    const D3DXCOLOR LIGHTGREEN(D3DCOLOR_XRGB(60, 184, 120));
    const D3DXCOLOR  PUREGREEN(D3DCOLOR_XRGB(0, 166, 81));
    const D3DXCOLOR  DARKGREEN(D3DCOLOR_XRGB(0, 114, 54));

    const D3DXCOLOR LIGHT_YELLOW_GREEN(D3DCOLOR_XRGB(124, 197, 118));
    const D3DXCOLOR  PURE_YELLOW_GREEN(D3DCOLOR_XRGB(57, 181, 74));
    const D3DXCOLOR  DARK_YELLOW_GREEN(D3DCOLOR_XRGB(25, 123, 48));

    const D3DXCOLOR LIGHTBROWN(D3DCOLOR_XRGB(198, 156, 109));
    const D3DXCOLOR DARKBROWN(D3DCOLOR_XRGB(115, 100, 87));

    // 光照
    D3DLIGHT9 InitDirectionalLight(D3DXVECTOR3* direction, D3DXCOLOR* color);
    D3DLIGHT9 InitPointLight(D3DXVECTOR3* position, D3DXCOLOR* color);
    D3DLIGHT9 InitSpotLight(D3DXVECTOR3* position, D3DXVECTOR3* direction, D3DXCOLOR* color);

    // 材质
    D3DMATERIAL9 InitMtrl(D3DXCOLOR a, D3DXCOLOR d, D3DXCOLOR s, D3DXCOLOR e, float p);

    const D3DMATERIAL9 WHITE_MTRL = InitMtrl(WHITE, WHITE, WHITE, BLACK, 2.0f);
    const D3DMATERIAL9 RED_MTRL = InitMtrl(RED, RED, RED, BLACK, 2.0f);
    const D3DMATERIAL9 GREEN_MTRL = InitMtrl(GREEN, GREEN, GREEN, BLACK, 2.0f);
    const D3DMATERIAL9 BLUE_MTRL = InitMtrl(BLUE, BLUE, BLUE, BLACK, 2.0f);
    const D3DMATERIAL9 YELLOW_MTRL = InitMtrl(YELLOW, YELLOW, YELLOW, BLACK, 2.0f);

    // Bounding Objects
    struct BoundingBox
    {
        BoundingBox();

        bool isPointInside(D3DXVECTOR3& p);

        D3DXVECTOR3 _min;
        D3DXVECTOR3 _max;
    };

    struct BoundingSphere
    {
        BoundingSphere();

        D3DXVECTOR3 _center;
        float       _radius;
    };

    // Constants
    // const float INFINITY = FLT_MAX;
    const float EPSILON = 0.001f;

    // Randomness
    // Desc: Return random float in [lowBound, highBound] interval.
    float GetRandomFloat(float lowBound, float highBound);

    // Desc: Returns a random vector in the bounds specified by min and max.
    void GetRandomVector(D3DXVECTOR3* out, D3DXVECTOR3* min, D3DXVECTOR3* max);

    // Conversion
    DWORD FtoDw(float f);

    // Interpolation
    float Lerp(float a, float b, float t);

    // 计算法线
    void ComputeNormal(D3DXVECTOR3* p0, D3DXVECTOR3* p1, D3DXVECTOR3* p2, D3DXVECTOR3* out);
}