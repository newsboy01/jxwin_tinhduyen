/*****************************************************************************************
//	ΩÁ√Ê--¥¢ŒÔœ‰ΩÁ√Ê
//	Copyright : Kingsoft 2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2003-4-21
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "../elem/wnds.h"
#include "UiStoreBox.h"
#include "UiGetMoney.h"


#include "UiLoginPWStoreBox.h"
#include "UiPWStoreBox.h"

#include "UiRPNewBox.h"
#include "UiInformation.h"

#include "UiItem.h"
#include "../../../core/src/coreshell.h"
#include "../../../core/src/GameDataDef.h"
#include "../UiSoundSetting.h"
#include "../UiBase.h"
#include <crtdbg.h>
#include "UiStaskItem.h"

extern iCoreShell*		g_pCoreShell;

#define SCHEME_INI_ITEM	"¥¢ŒÔœ‰.ini"

KUiStoreBox* KUiStoreBox::m_pSelf = NULL;

enum WAIT_OTHER_WND_OPER_PARAM
{
	UISTOREBOX_WAIT_GETMONEY,
};

//--------------------------------------------------------------------------
//	π¶ƒ‹£∫»Áπ˚¥∞ø⁄’˝±ªœ‘ æ£¨‘Ú∑µªÿ µ¿˝÷∏’Î
//--------------------------------------------------------------------------
KUiStoreBox* KUiStoreBox::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

//--------------------------------------------------------------------------
//	π¶ƒ‹£∫¥Úø™¥∞ø⁄£¨∑µªÿŒ®“ªµƒ“ª∏ˆ¿‡∂‘œÛ µ¿˝
//--------------------------------------------------------------------------
KUiStoreBox* KUiStoreBox::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiStoreBox;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		if (KUiItem::GetIfVisible() == NULL)
			KUiItem::OpenWindow();
		else
			UiSoundPlay(UI_SI_WND_OPENCLOSE);

		m_pSelf->UpdateData();
		m_pSelf->BringToTop();
		m_pSelf->Show();
		Wnd_GameSpaceHandleInput(false);

	}
	return m_pSelf;
}

//--------------------------------------------------------------------------
//	π¶ƒ‹£∫πÿ±’¥∞ø⁄
//--------------------------------------------------------------------------
void KUiStoreBox::CloseWindow()
{
	if (m_pSelf)
	{


		if (KUiRPNewBox::GetIfVisible())
			KUiRPNewBox::CloseWindow();



		Wnd_GameSpaceHandleInput(true);
		m_pSelf->Destroy();
		m_pSelf = NULL;
	}
}

// -------------------------------------------------------------------------
// π¶ƒ‹	: ≥ı ºªØ
// -------------------------------------------------------------------------
void KUiStoreBox::Initialize()
{
	AddChild(&m_Money);
	AddChild(&m_GetMoneyBtn);
	AddChild(&m_CloseBtn);



	AddChild(&m_BlockBox1);
	AddChild(&m_BlockBox2);

	AddChild(&m_PassWordBox);
	AddChild(&m_OpenBox);


	AddChild(&m_ItemBox);

	m_ItemBox.SetContainerId((int)UOC_STORE_BOX);
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	m_nMoney = 0;
	Wnd_AddWindow(this);
}

//--------------------------------------------------------------------------
//	π¶ƒ‹£∫ππ‘Ï∫Ø ˝
//--------------------------------------------------------------------------

void KUiStoreBox::UpdateClientDT()
{

if (m_pSelf)
{
m_pSelf->UpdateData();
}

}


void KUiStoreBox::UpdateData()
{
	m_ItemBox.Clear();
	m_nMoney = 0;
	int van = m_nMoney/10000;
	int luong = m_nMoney%10000;
	char ngan_luong[32] = "";
	if (van > 0)
	sprintf(ngan_luong,"%d %s ",van,"vπn");

	sprintf(ngan_luong,"%s%d %s",ngan_luong,luong,"l≠Óng");
	m_Money.SetText(ngan_luong);


	if (g_pCoreShell->GetGameData(GDI_PLAYER_BLOCK_BOX, 0, 0))
	{
	m_BlockBox1.Show();
	m_BlockBox2.Hide();
	}
	else
	{
	m_BlockBox2.Show();
	m_BlockBox1.Hide();
	}

	KUiObjAtRegion* pObjs = NULL;

	int nCount = g_pCoreShell->GetGameData(GDI_ITEM_IN_STORE_BOX, 0, 0);
	if (nCount == 0)
		return;

	if (pObjs = (KUiObjAtRegion*)malloc(sizeof(KUiObjAtRegion) * nCount))
	{
		g_pCoreShell->GetGameData(GDI_ITEM_IN_STORE_BOX, (unsigned int)pObjs, nCount);//µ•œﬂ≥Ã÷¥––£¨nCount÷µ≤ª±‰
		for (int i = 0; i < nCount; i++)
			UpdateItem(&pObjs[i], true);
		free(pObjs);
		pObjs = NULL;
	}
}

// -------------------------------------------------------------------------
// π¶ƒ‹	: ŒÔ∆∑±‰ªØ∏¸–¬
// -------------------------------------------------------------------------
void KUiStoreBox::UpdateItem(KUiObjAtRegion* pItem, int bAdd)
{
	if (pItem)
	{
		UiSoundPlay(UI_SI_PICKPUT_ITEM);
		if (pItem->Obj.uGenre != CGOG_MONEY)
		{
			KUiDraggedObject Obj;
			Obj.uGenre = pItem->Obj.uGenre;
			Obj.uId = pItem->Obj.uId;
			Obj.DataX = pItem->Region.h;
			Obj.DataY = pItem->Region.v;
			Obj.DataW = pItem->Region.Width;
			Obj.DataH = pItem->Region.Height;
			if (bAdd)
				m_ItemBox.AddObject(&Obj, 1);
			else
				m_ItemBox.RemoveObject(&Obj);
		}
		else
		{
			m_nMoney = pItem->Obj.uId;
			int van = m_nMoney/10000;
			int luong = m_nMoney%10000;
			char ngan_luong[32] = "";
	if (van > 0)
	sprintf(ngan_luong,"%d %s ",van,"vπn");

	sprintf(ngan_luong,"%s%d %s",ngan_luong,luong,"l≠Óng");
			m_Money.SetText(ngan_luong);
		}
	}
	else
		UpdateData();
}

// -------------------------------------------------------------------------
// π¶ƒ‹	: ‘ÿ»ÎΩÁ√Ê∑Ω∞∏
// -------------------------------------------------------------------------
void KUiStoreBox::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_ITEM);
	if (m_pSelf && Ini.Load(Buff))
	{
		m_pSelf->Init(&Ini, "Main");
		m_pSelf->m_Money.Init(&Ini, "Money");
		m_pSelf->m_GetMoneyBtn.Init(&Ini, "GetMoneyBtn");
		m_pSelf->m_CloseBtn.Init(&Ini, "CloseBtn");


		m_pSelf->m_BlockBox1.Init(&Ini, "BLockBox1");
		m_pSelf->m_BlockBox2.Init(&Ini, "BLockBox2");

		m_pSelf->m_PassWordBox.Init(&Ini, "PassWordBox");
		m_pSelf->m_OpenBox.Init(&Ini, "OpenBox");


		m_pSelf->m_ItemBox.Init(&Ini, "ItemBox");
		m_pSelf->m_ItemBox.EnableTracePutPos(true);
	}
}

// -------------------------------------------------------------------------
// π¶ƒ‹	: ¥∞ø⁄∫Ø ˝
// -------------------------------------------------------------------------
int KUiStoreBox::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	switch(uMsg)
	{
	case WND_N_ITEM_PICKDROP:
		OnItemPickDrop((ITEM_PICKDROP_PLACE*)uParam, (ITEM_PICKDROP_PLACE*)nParam);
		break;
	case WND_N_LEFT_CLICK_SHIFT_ITEM:
		OnLeftShiftClickItem((KUiDraggedObject*)uParam);
		break;
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_CloseBtn)
			CloseWindow();
		else if (uParam == (unsigned int)(KWndWindow*)&m_GetMoneyBtn)
		{
			KUiGetMoney::OpenWindow(0, m_nMoney, this, UISTOREBOX_WAIT_GETMONEY, &m_Money);
		}





		else if (uParam == (unsigned int)(KWndWindow*)&m_BlockBox1)
		{
				if (g_pCoreShell->GetGameData(GDI_PLAYER_BLOCK_BOX, 0, 0))
				{
			KUiLoginPWStoreBox::OpenWindow();
				}
		}
		


		else if (uParam == (unsigned int)(KWndWindow*)&m_PassWordBox)
		{
				if (!g_pCoreShell->GetGameData(GDI_PLAYER_BLOCK_BOX, 0, 0))
				{
			    KUiPWStoreBox::OpenWindow();
				}
				else
				{
				UIMessageBox("Ch≠a mÎ kh„a r≠¨ng !",this,"Tho∏t");
				}
		}







		else if (uParam == (unsigned int)(KWndWindow*)&m_OpenBox)
		{
				

			    if (KUiRPNewBox::GetIfVisible())
				{
			        KUiRPNewBox::CloseWindow();
				}
				else
				{
					KUiRPNewBox::OpenWindow();
				}
				
		}



		










		break;
	case WND_M_OTHER_WORK_RESULT:
		if (uParam == UISTOREBOX_WAIT_GETMONEY)
			OnGetMoney(nParam);
		break;
	default:
		return KWndShowAnimate::WndProc(uMsg, uParam, nParam);
	}
	return 0;
}

void KUiStoreBox::OnGetMoney(int nMoney)
{
	if (nMoney > 0)
	{
		g_pCoreShell->OperationRequest(GOI_MONEY_INOUT_STORE_BOX,
			false, nMoney);
	}
}


void KUiStoreBox::OnLeftShiftClickItem(KUiDraggedObject* pItem)
{
	if (pItem == NULL || g_pCoreShell == NULL)
		return;

	if (pItem->uGenre != CGOG_ITEM || pItem->uId <= 0)
		return;

	if (!g_pCoreShell)
		return;

	if (g_pCoreShell->GetGameData(GDI_ITEMDATAINFO,pItem->uId, 2) != 4 || g_pCoreShell->GetGameData(GDI_ITEMDATAINFO,pItem->uId, 3) != 1)
        return;

	KUiObjAtContRegion	Obj;
	Obj.Obj.uGenre = pItem->uGenre;
	Obj.Obj.uId = pItem->uId;
	Obj.Region.h = pItem->DataX;
	Obj.Region.v = pItem->DataY;
	Obj.Region.Width  = pItem->DataW;
	Obj.Region.Height = pItem->DataH;
	Obj.eContainer = UOC_ITEM_TAKE_WITH;


		KUiItemBuySelInfo	Price = { 0 };
	
		if (g_pCoreShell->GetGameData(GDI_TRADE_ITEM_PRICE,
			(unsigned int)(&Obj), (int)(&Price)) == 2)
		{
			KUiStaskItem::OpenWindow(&Obj, &Price);
		}
			
	

}



void KUiStoreBox::OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos)
{
	if (g_UiBase.GetStatus() != UIS_S_IDLE)
		return;
	KUiObjAtContRegion	Pick, Drop;
	KUiDraggedObject	Obj;

	if (pPickPos)
	{
		_ASSERT(pPickPos->pWnd);		
		((KWndObjectMatrix*)(pPickPos->pWnd))->GetObject(
			Obj, pPickPos->h, pPickPos->v);
		Pick.Obj.uGenre = Obj.uGenre;
		Pick.Obj.uId = Obj.uId;
		Pick.Region.Width = Obj.DataW;
		Pick.Region.Height = Obj.DataH;
		Pick.Region.h = Obj.DataX;
		Pick.Region.v = Obj.DataY;
		Pick.eContainer = UOC_STORE_BOX;
	}

	if (pDropPos)
	{
		Wnd_GetDragObj(&Obj);
		Drop.Obj.uGenre = Obj.uGenre;
		Drop.Obj.uId = Obj.uId;
		Drop.Region.Width = Obj.DataW;
		Drop.Region.Height = Obj.DataH;
		Drop.Region.h = pDropPos->h;
		Drop.Region.v = pDropPos->v;
		Drop.eContainer = UOC_STORE_BOX;
	}
	
	g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT,
		pPickPos ? (unsigned int)&Pick : 0,
		pDropPos ? (int)&Drop : 0);
}