#pragma once
/*
 * 摄像机类，负责摄像机自身坐标的平移，旋转等操作
 */
#include "stdafx.h"

class CCamera
{
public:
    enum CameraType { LANDOBJECT, AIRCRAFT };

    CCamera(void);
    CCamera(CameraType cameraType);
    ~CCamera(void);

    void strafe(float units); // right轴平移
    void fly(float units);    // up轴平移
    void walk(float units);   // look轴平移

    void pitch(float angle); // 绕right轴旋转
    void yaw(float angle);   // 绕up轴旋转
    void roll(float angle);  // 绕look轴旋转

    void rotation(D3DXVECTOR3* axis, float angle); // 绕任意轴旋转

    void getViewMatrix(D3DXMATRIX* V);
    void setCameraType(CameraType cameraType);
    //void getCameraType(CameraType* cameraType);
    void getPosition(D3DXVECTOR3* pos);
    void setPosition(D3DXVECTOR3* pos);
    void setPosition(float x, float y, float z);

    void getRight(D3DXVECTOR3* right);
    void getUp(D3DXVECTOR3* up);
    void getLook(D3DXVECTOR3* look);

private:
    CameraType	_cameraType;
    D3DXVECTOR3	_right;
    D3DXVECTOR3	_up;
    D3DXVECTOR3	_look;
    D3DXVECTOR3	_pos;
};