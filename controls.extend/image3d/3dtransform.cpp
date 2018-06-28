#include "stdafx.h"
#include "3dtransform.h"
#include "PerspectiveTransform.h"
#include "3dlib.h"
#include <float.h>
#include <malloc.h>

bool  g_bInitSinCosTable = false;

#define STRIDELEN(WID,BITPIXEL) ((WID*BITPIXEL+31)/32*4)
namespace IMAGE3D
{

	C3DTransform::~C3DTransform()
	{
		if(m_pSrcBits) free(m_pSrcBits);
	}

void  C3DTransform::Initialize()
{

    if (false == g_bInitSinCosTable)
    {
        _control87(_MCW_RC, _RC_DOWN); //设置FPU的舍入模式，在Bilinear函数中需要使用汇编加快float->int

        Build_Sin_Cos_Tables();
        g_bInitSinCosTable = true;
    }
}

// 使用3D图形算法来获取图片旋转后的四个顶点位置
void C3DTransform::GetQuadByAnimateValue(int nDegreeX, int nDegreeY, int nDegreeZ, int nZOffset, Quad* pOutQuad)
{
	// .局部坐标(以矩形的中心作为局部坐标的原点)
	float x = (float)m_nSrcWndWidth/2;
	float y = (float)m_nSrcWndHeight/2;

	POINT3D  pt3dModel[4] = { 
		{-x, y, 0}, {x, y, 0}, {x, -y, 0}, {-x, -y, 0}
	};
	POINT3D pt3DWorld[4] = {0};
	POINT3D pt3DCamera[4] = {0};

#pragma region // .局部坐标->世界坐标
	{
		// .以当前值作为角度进行旋转
		float fDegreeX = (float)nDegreeX;
		float fDegreeY = (float)nDegreeY;
		float fDegreeZ = (float)nDegreeZ;

		MATRIX_4_4  matTemp1, matTemp2;
		MATRIX_4_4  matRotateY, matRotateX, matRotateZ;

		MATRIX_4_4_PTR pLeftArg = NULL;
		if (0 != fDegreeY)
		{
			MAT_IDENTITY_4_4(&matRotateY);
			matRotateY.M00 = Fast_Cos(fDegreeY);
			matRotateY.M02 = -Fast_Sin(fDegreeY);
			matRotateY.M20 = Fast_Sin(fDegreeY);
			matRotateY.M22 = Fast_Cos(fDegreeY);
			
			pLeftArg = &matRotateY;
		}

		if (0 != fDegreeX)
		{

			MAT_IDENTITY_4_4(&matRotateX);
			matRotateX.M11 = Fast_Cos(fDegreeX);
			matRotateX.M12 = Fast_Sin(fDegreeX);
			matRotateX.M21 = -Fast_Sin(fDegreeX);
			matRotateX.M22 = Fast_Cos(fDegreeX);

			if (NULL == pLeftArg)
			{
				pLeftArg = &matRotateX;
			}
			else
			{
				Mat_Mul_4X4(pLeftArg, &matRotateX, &matTemp1);
				pLeftArg = &matTemp1;
			}
		}

		if (0 != fDegreeZ)
		{

			MAT_IDENTITY_4_4(&matRotateZ);
			matRotateZ.M00 = Fast_Cos(fDegreeZ);
			matRotateZ.M01 = Fast_Sin(fDegreeZ);
			matRotateZ.M10 = -Fast_Sin(fDegreeZ);
			matRotateZ.M11 = Fast_Cos(fDegreeZ);

			if (NULL == pLeftArg)
			{
				pLeftArg = &matRotateZ;
			}
			else
			{
				Mat_Mul_4X4(pLeftArg, &matRotateZ, &matTemp2);
				pLeftArg = &matTemp2;
			}
		}

		if (pLeftArg)
		{
			for (int i = 0; i < 4; i++)
				Mat_Mul_VECTOR3D_4X4(&pt3dModel[i], pLeftArg, &pt3DWorld[i]);
		}
		else
		{
			for (int i = 0; i < 4; i++)
			{
				pt3DWorld[i].x = pt3dModel[i].x;
				pt3DWorld[i].y = pt3dModel[i].y;
			}
		}

		// .由于仍然是平移到世界坐标的 （0，0，0）位置，因此不用计算平移
		for (int i = 0; i < 4; i++)
		{
			pt3DWorld[i].z += nZOffset;
		}
	}
#pragma endregion

	// 相机位置(这里默认将视平面放在与矩形所在面的同一位置，这样透视出来的坐标直接就可以
	// 当成屏幕坐标来用了，省了一步操作。
	// 但是需要注意的是，如果nCameraPos过小的话看到的图像就会缩小，
	CAMERA camerpos = {0};
	float  fCameraPos = 2000.0f;  // 相机位置
	float  d = fCameraPos;      // 相机与视平面的距离。将两值设成一样，避免了一次到屏幕坐标的转换

	VECTOR4D_INITXYZ(&camerpos.WorldPos, 0,0, -fCameraPos);

#pragma region // 世界坐标转换为相机坐标
	{
		// 平移矩阵
		MATRIX_4_4  matCameraTrans = {0};
		MAT_IDENTITY_4_4(&matCameraTrans);
		matCameraTrans.M30 = -camerpos.WorldPos.x;
		matCameraTrans.M31 = -camerpos.WorldPos.y;
		matCameraTrans.M32 = -camerpos.WorldPos.z;
		
		// 相机角度为0，不旋转

		for (int i = 0; i < 4; i++)
		{
			Mat_Mul_VECTOR3D_4X4(&pt3DWorld[i], &matCameraTrans, &pt3DCamera[i]);
		}
	}
#pragma endregion

#pragma region // 相机坐标转换为透视坐标
	POINT3D pt3DPerspectivePos[4];
	for (int i = 0; i < 4; i++)
	{
		float z = pt3DCamera[i].z;  // 这里的z是用于和d相比的距离，不是坐标. 当d值取的比较小时，会导致z为负
		float i_z = 1/z;
		if (pt3DCamera[i].z != 0)
		{
			pt3DPerspectivePos[i].x = d * pt3DCamera[i].x * i_z;  // nCameraPos相当于d
			pt3DPerspectivePos[i].y = d * pt3DCamera[i].y * i_z;  // 
		}
	}
#pragma endregion

	// 转换到屏幕坐标上
	for (int i = 0; i < 4; ++i)
	{
		pt3DPerspectivePos[i].x += (m_nSrcWndWidth>>1);
		pt3DPerspectivePos[i].y = -pt3DPerspectivePos[i].y;
		pt3DPerspectivePos[i].y += (m_nSrcWndHeight>>1);
	}

	// 赋值给返回值
	for (int i = 0; i < 4; i++)
	{
		pOutQuad->pos[2*i]   = (int)pt3DPerspectivePos[i].x;
		pOutQuad->pos[2*i+1] = (int)pt3DPerspectivePos[i].y;
	}
}


// 将函数调用改成宏定义，因为这个函数调用太频繁了
#define GetLine(pBits, nPitch,y) \
	(pBits + nPitch*(y))


void C3DTransform::Render(const PARAM3DTRANSFORM & param3d)
{
	RECT rc = {0, 0, m_nSrcWndWidth, m_nSrcWndHeight};

	// 计算当前的动画窗口屏幕坐标
	Quad quad;
	GetQuadByAnimateValue(param3d.nRotateX,param3d.nRotateY, param3d.nRotateZ, param3d.nOffsetZ, &quad);

	// 用于计算矩形->任意四边形的对应关系
	PerspectiveTransform perspective;
	perspective.SetQuad(&quad, &rc);
	perspective.ChangeToFixedPoint();

	int   nWidthDst  = m_nSrcWndWidth-1;   // (300->299)，数据范围是它的大小减一
	int   nHeightDst = m_nSrcWndHeight-1; //

	int nDstPitch = nWidthDst*4;

	float fxSrc = 0;
	float fySrc = 0;
	float *pfxSrc = &fxSrc;  // 为了在perspective_transform中使用指针
	float *pfySrc = &fySrc;

	LPBYTE pDstBits=m_pDstBits;
	
	int nPixByte=m_nBitsPixel/8;

#if 1  // 在大循环之前剔除掉一些空白区域
	//将目标图片清空
	memset(pDstBits, 0, nDstPitch * nHeightDst);

	int nMinX = max(0, min(min(min(quad.Ax,quad.Bx),quad.Cx),quad.Dx));
	int nMinY = max(0, min(min(min(quad.Ay,quad.By),quad.Cy),quad.Dy));
	int nMaxX = min(nWidthDst,  max(max(max(quad.Ax,quad.Bx),quad.Cx),quad.Dx));
	int nMaxY = min(nHeightDst, max(max(max(quad.Ay,quad.By),quad.Cy),quad.Dy));

	
	pDstBits += (nMinY*nDstPitch);
	for (int Y = nMinY; Y < nMaxY; Y++)
	{
		LPBYTE pDest=pDstBits;
		for (int X = nMinX; X < nMaxX; X++)
		{
#else
	for (int X = 0, Y = 0; Y < m_nSrcWndHeight; Y++)
	{
		for (X = 0; X < m_nSrcWndWidth; X++)
		{
#endif
			perspective_transform_fp(perspective, X, Y, pfxSrc, pfySrc);

			int nx = 0; //fx;   // 注: 直接赋值 int i = floatvalue; 会产生一个 _ftol()调用
			int ny = 0; //fy;   //     导致效率降低。这里使用内嵌汇编指令。(3D游戏编程大师技巧 P918)

			// 浮点数转整数。 注意：默认的fistp是四舍五入模式。需要通过调用_control87(_MCW_RC, _RC_DOWN);进行调整
#ifndef _WIN64
			__asm	fld    fxSrc;
			__asm	fistp  nx;

			__asm	fld    fySrc;
			__asm	fistp  ny;
#else
            nx = int(fxSrc);
            ny = int(fySrc);
#endif//WIN32

			// 1. ceil的效率非常非常低!千万别在这用
			// 但是有一个问题如果height为300，ySrc=299.99999999时，转成(int)得到的结果是300，
			// 但在这里判断的话是成功的，导致最后崩溃，因此当ySrc>299时即退出
			if (nx < 0 || nx >= nWidthDst || ny < 0 || ny >= nHeightDst)
			{        
				continue;
			}

			// 目标与源图像素对应
#if 0
			//////////////////////////////////////////////////////////////////////////
			// 邻近取样, 速度快，但有锯齿
			((DWORD*)pDstBits)[X] = ((DWORD*)(m_pSrcBits + m_nSrcPitch*(int)ySrc))[(int)xSrc];
#else
			//////////////////////////////////////////////////////////////////////////
			// 二次线性插值，能有效去掉锯齿，但速度非常慢
			//((DWORD*)pDstBits)[X] = Bilinear(m_pSrcBits, m_nSrcPitch, nx, ny, (float)fxSrc, (float)fySrc);

			//
			// 二次线性插值公式推导
			// http://blog.csdn.net/dakistudio/article/details/1767100 二次线性插值公式推导
			//
			// x为fx的向下取整，y为fy的向下取整
			//
			//inline DWORD& Bilinear(BYTE* pBits, const int& nPitch, const int& x, const int& y, const float& fx, const float& fy)
			{
				// 已将原始图片的right/bottom扩大1px，这样在获取 x+1, y+1时达到数组边缘也不会崩溃
				// 取附近的四个像素的颜色值(x,y) (x+1, y) (x, y+1) (x+1, y+1)

				LPBYTE pValue=GetLine(m_pSrcBits, m_nSrcPitch, ny);
				pValue += nx*nPixByte;

				LPBYTE p0=pValue;//(x,y);
				LPBYTE p2=pValue+nPixByte;//(x+1,y)
				pValue+=m_nSrcPitch;//y+1
				LPBYTE p1=pValue;//(x,y+1)
				LPBYTE p3=pValue+nPixByte;//(x+1,y+1)

				// 将浮点数乘法转为定点数乘法。同时优化浮点数转成整数。
				float u = (float)fxSrc - nx;
				float v = (float)fySrc - ny;
			
#ifndef _WIN64
				float fpm3 = FLOAT_TO_FIXP16(u*v);
				float fpm2 = FLOAT_TO_FIXP16(u*(1.0f-v));
				float fpm1 = FLOAT_TO_FIXP16(v*(1.0f-u));
				float fpm0 = FLOAT_TO_FIXP16((1.0f-u)*(1.0f-v));
				int pm3_16 = 0;
				int pm2_16 = 0;
				int pm1_16 = 0;
				int pm0_16 = 0;
				__asm
				{
					fld    fpm3;
					fistp  pm3_16;
					fld    fpm2;
					fistp  pm2_16;
					fld    fpm1;
					fistp  pm1_16;
					fld    fpm0;
					fistp  pm0_16;
				}
#else
                int pm3_16 = int(FLOAT_TO_FIXP16(u*v));
                int pm2_16 = int(FLOAT_TO_FIXP16(u*(1.0f-v)));
                int pm1_16 = int(FLOAT_TO_FIXP16(v*(1.0f-u)));
                int pm0_16 = int(FLOAT_TO_FIXP16((1.0f-u)*(1.0f-v)));
#endif//WIN32
                pDest = pDstBits + X * nPixByte;
				*pDest++=(BYTE)((pm0_16*(*p0++) + pm1_16*(*p1++) + pm2_16*(*p2++) + pm3_16*(*p3++)) >> FIXP16_SHIFT);
				*pDest++=(BYTE)((pm0_16*(*p0++) + pm1_16*(*p1++) + pm2_16*(*p2++) + pm3_16*(*p3++)) >> FIXP16_SHIFT);
				*pDest++=(BYTE)((pm0_16*(*p0++) + pm1_16*(*p1++) + pm2_16*(*p2++) + pm3_16*(*p3++)) >> FIXP16_SHIFT);
				if(nPixByte==4) *pDest++=(BYTE)((pm0_16*(*p0++) + pm1_16*(*p1++) + pm2_16*(*p2++) + pm3_16*(*p3++)) >> FIXP16_SHIFT);
			}
#endif
		}
		pDstBits += nDstPitch;
	}
}



BOOL C3DTransform::SetImage( LPBYTE pSour,LPBYTE pDest,int nWid,int nHei,int nBitsPixel )
{
	if(nBitsPixel!=24 && nBitsPixel!=32) //只支持24和32位
		return FALSE;

	m_nSrcWndWidth=nWid+1,m_nSrcWndHeight=nHei+1;
	m_nSrcPitch=STRIDELEN(m_nSrcWndWidth,nBitsPixel);
	m_pSrcBits=(LPBYTE)calloc(1,m_nSrcPitch*m_nSrcWndHeight);
	m_nBitsPixel=nBitsPixel;
	m_pDstBits=pDest;

	LPBYTE p1=m_pSrcBits+m_nSrcPitch;//bottom-up，跳过最下面一行
	LPBYTE p2=pSour;
	int nLen=STRIDELEN(nWid,nBitsPixel);
	for(int i=0;i<nHei;i++)
	{
		memcpy(p1,p2,nLen);
		p1+=m_nSrcPitch;
		p2+=nLen;
	}
	return TRUE;
}

}//end of namespace