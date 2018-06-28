#include "stdafx.h"
#include "sclock.h"

namespace SOUI{

SClock::SClock()
{
}

void SClock::OnPaint(SOUI::IRenderTarget * pRT)
{
	SWindow::OnPaint(pRT);

	CRect rcClient;
	GetClientRect(&rcClient);

	CPoint center = rcClient.CenterPoint();

	// 计算矩形
	// 35 * 16
	CRect rcDraw(center, SOUI::CPoint(center.x + 200, center.y + 32));
	rcDraw.OffsetRect(-35, -16);
	CRect rcSrc(0, 0, 200, 32);


    SYSTEMTIME last_refresh_time;
    ::GetLocalTime(&last_refresh_time);

    {
        double angle = GetHourAngle(last_refresh_time.wHour,last_refresh_time.wMinute);
        SMatrix form = InitMatrix(angle,  center);
        pRT->SetTransform(&form, NULL);
        pRT->DrawBitmapEx(rcDraw, pointer_hour, &rcSrc, EM_STRETCH, 255);
    }

    {
        double angle = GetMinuteSecondAngle(last_refresh_time.wMinute);
        SMatrix form = InitMatrix(angle, center);
        pRT->SetTransform(&form, NULL);
        pRT->DrawBitmapEx(rcDraw, pointer_minute, &rcSrc, EM_STRETCH, 255);
    }

    {
        double angle = GetMinuteSecondAngle(last_refresh_time.wSecond);
        SMatrix form = InitMatrix(angle, center);
        pRT->SetTransform(&form, NULL);
        pRT->DrawBitmapEx(rcDraw, pointer_second, &rcSrc, EM_STRETCH, 255);
    }
	pRT->SetTransform(&SMatrix());
}

SMatrix SClock::InitMatrix(double angle, CPoint &center)
{
    //先平移到center，再旋转angle，再平移-center
    return SMatrix().translate((FLOAT)center.x,(FLOAT)center.y)
                    .rotate((FLOAT)angle-90)
                    .translate((FLOAT)-center.x,(FLOAT)-center.y);
}


double SClock::GetMinuteSecondAngle(int nValue)
{
    return (double)nValue * ((double)360 / 60);
}

double SClock::GetHourAngle(int nHour,int nMinute)
{
	double base = (double)(nHour % 12) * ((double)360 / 12);
	base += (double)nMinute * ((double)360 / 12 / 60);
	return base;
}

void SClock::OnTimer(char cTimerID)
{
	Invalidate();
}

int SClock::OnCreate(void*)
{
	SetTimer(TIMER_REFRESH, 200);
	return 0;
}

}