#pragma once

/*
 * 粒子系统，提供了雪花类
 */

#include "d3dUtils.h"
#include "camera.h"
#include <list>

namespace psys
{
	// 粒子的顶点格式
	struct Particle
	{
		D3DXVECTOR3 _position;
		D3DCOLOR    _color;
		static const DWORD FVF;
	};
	
	// 粒子的属性
	struct Attribute
	{
		Attribute()
		{
			_lifeTime = 0.0f;
			_age      = 0.0f;
			_isAlive  = true;
		}

		D3DXVECTOR3 _position;     // 位置
		D3DXVECTOR3 _velocity;     // 速度
		D3DXVECTOR3 _acceleration; // 加速度
		float       _lifeTime;     // 粒子的生存时间  
		float       _age;          // 当前的年龄  
		D3DXCOLOR   _color;        // 颜色  
		D3DXCOLOR   _colorFade;    // 颜色随时间的变化
		bool        _isAlive;    
	};


	class CPSystem
	{
	public:
		CPSystem();
		virtual ~CPSystem();

		virtual bool init(IDirect3DDevice9* device, TCHAR* texFileName);

		virtual void reset();	// 重置粒子系统，该方法会重置所有粒子的属性
		virtual void resetParticle(Attribute* attribute) = 0;	// 重置粒子属性，通常在粒子死亡时会重置
		virtual void addParticle();	// 增加一个粒子

		virtual void update(float timeDelta) = 0;

		virtual void preRender();
		virtual void Render();
		virtual void postRender();

		bool isEmpty();
		bool isDead();

	protected:
		virtual void removeDeadParticles();

	protected:
		IDirect3DDevice9*       _device;
		D3DXVECTOR3             _origin;
		d3d::BoundingBox        _boundingBox;// 粒子的活动范围，超出范围则杀死
		float                   _emitRate;   // rate new particles are added to system
		float                   _size;       // size of particles
		IDirect3DTexture9*      _tex;
		IDirect3DVertexBuffer9* _vb;
		std::list<Attribute>    _particles;
		int                     _maxParticles; // max allowed particles system can have


		// Following three data elements used for rendering the p-system efficiently
		DWORD _vbSize;      // size of vb
		DWORD _vbOffset;    // offset in vb to lock   
		DWORD _vbBatchSize; // number of vertices to lock starting at _vbOffset
	};

	class CSnow : public CPSystem
	{
	public:
		CSnow(d3d::BoundingBox* boundingBox, int numParticles);
		void resetParticle(Attribute* attribute);
		void update(float timeDelta);
	};
}