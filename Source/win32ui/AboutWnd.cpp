#include <zlib.h>
#include <boost/version.hpp>
#include <boost/lexical_cast.hpp>
#include "AboutWnd.h"
#include "PtrMacro.h"
#include "layout/LayoutStretch.h"
#include "layout/HorizontalLayout.h"
#include "win32/LayoutWindow.h"
#include "resource.h"
#include "../PS2VM.h"
#include "string_cast.h"

#define CLSNAME		_T("AboutWnd")
#define WNDSTYLE	(WS_CAPTION | WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU)
#define WNDSTYLEEX	(WS_EX_DLGMODALFRAME)

using namespace Framework;
using namespace std;
using namespace boost;

CAboutWnd::CAboutWnd(HWND hParent) :
Win32::CModalWindow(hParent)
{
	RECT rc;

	TCHAR sVersion[256];

	if(!DoesWindowClassExist(CLSNAME))
	{
		WNDCLASSEX w;
		memset(&w, 0, sizeof(WNDCLASSEX));
		w.cbSize		= sizeof(WNDCLASSEX);
		w.lpfnWndProc	= CWindow::WndProc;
		w.lpszClassName	= CLSNAME;
		w.hbrBackground	= (HBRUSH)GetSysColorBrush(COLOR_BTNFACE);
		w.hInstance		= GetModuleHandle(NULL);
		w.hCursor		= LoadCursor(NULL, IDC_ARROW);
		RegisterClassEx(&w);
	}

	SetRect(&rc, 0, 0, 360, 125);

	if(hParent != NULL)
	{
		EnableWindow(hParent, FALSE);
	}

	Create(WNDSTYLEEX, CLSNAME, _T("About Play!"), WNDSTYLE, &rc, hParent, NULL);
	SetClassPtr();

	SetRect(&rc, 0, 0, 1, 1);

	m_pImage = new Win32::CStatic(m_hWnd, rc, SS_ICON);
	m_pImage->SetIcon(LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_PUREI), IMAGE_ICON, 48, 48, 0));

    tstring sDate(string_cast<tstring>(__DATE__));
    tstring sZlibVersion(string_cast<tstring>(ZLIB_VERSION));

	_sntprintf(sVersion, countof(sVersion), _T("Version %s (%s) - zlib v%s - boost v%s"), 
        APP_VERSIONSTR, sDate.c_str(), sZlibVersion.c_str(), GetBoostVersion().c_str());

    FlatLayoutPtr pSubLayout0 = CVerticalLayout::Create(3);
    pSubLayout0->InsertObject(Win32::CLayoutWindow::CreateTextBoxBehavior(100, 30, new Win32::CStatic(m_hWnd, sVersion, SS_EDITCONTROL)));
    pSubLayout0->InsertObject(Win32::CLayoutWindow::CreateTextBoxBehavior(100, 15, new Win32::CStatic(m_hWnd, _T("Written by Jean-Philip Desjardins"))));
    pSubLayout0->InsertObject(Win32::CLayoutWindow::CreateTextBoxBehavior(100, 15, new Win32::CStatic(m_hWnd, _T("jean-philip.desjardins@polymtl.ca"))));
    pSubLayout0->InsertObject(CLayoutStretch::Create());
    pSubLayout0->SetHorizontalStretch(1);

    FlatLayoutPtr pSubLayout1 = CVerticalLayout::Create();
    pSubLayout1->InsertObject(Win32::CLayoutWindow::CreateCustomBehavior(48, 48, 0, 0, m_pImage));
    pSubLayout1->InsertObject(CLayoutStretch::Create());

    FlatLayoutPtr pSubLayout2 = CHorizontalLayout::Create(10);
    pSubLayout2->InsertObject(pSubLayout1);
    pSubLayout2->InsertObject(pSubLayout0);
    pSubLayout2->SetVerticalStretch(0);

    m_pLayout = CVerticalLayout::Create();
	m_pLayout->InsertObject(pSubLayout2);
    m_pLayout->InsertObject(CLayoutStretch::Create());

	RefreshLayout();
}

CAboutWnd::~CAboutWnd()
{

}

void CAboutWnd::RefreshLayout()
{
	RECT rc;

	GetClientRect(&rc);

	SetRect(&rc, rc.left + 10, rc.top + 10, rc.right - 10, rc.bottom - 10);

	m_pLayout->SetRect(rc.left, rc.top, rc.right, rc.bottom);
	m_pLayout->RefreshGeometry();

	Redraw();
}

tstring CAboutWnd::GetBoostVersion()
{
    return 
        lexical_cast<tstring>(BOOST_VERSION / 100000) + _T(".") +
        lexical_cast<tstring>(BOOST_VERSION / 100 % 1000) + _T(".") +
        lexical_cast<tstring>(BOOST_VERSION % 100);
}
