#include "KCore.h"
#include "KObjSet.h"
#include "KNpcSet.h"
#include "KSubWorld.h"
#include "KNpc.h"
#include "KIniFile.h"
#include "KSubWorldSet.h"
#include "KMissleSet.h"
#include "LuaFuns.h"
#include "KNpcTemplate.h"
#include "KPlayerSet.h"
#include "KPlayer.h"
extern KNpcTemplate	* g_pNpcTemplate[MAX_NPCSTYLE][MAX_NPC_LEVEL]; //0,0为起点
KSubWorldSet g_SubWorldSet;
KLuaScript * pPlayScript;


KSubWorldSet::KSubWorldSet()
{
	m_nLoopRate = 0;
	m_nGameVersion = ITEM_VERSION;


	m_TimeNew_Gio = 0;
	m_TimeNew_Phut = 0;
	m_TimeNew_Giay = 0;

#ifndef _SERVER
	m_dwPing = 0;
#endif
}

int KSubWorldSet::SearchWorld(DWORD dwID)
{
	for (int i = 0; i < MAX_SUBWORLD; i++)
	{
		if ((DWORD)SubWorld[i].m_SubWorldID == dwID)
			return i;
	}
	return -1;
}

BOOL KSubWorldSet::Load(LPSTR szFileName)
{
	KIniFile	IniFile;
	char		szKeyName[32];
	int			nWorldID;
	int			nWorldCount = 0;
	
	IniFile.Load(szFileName);
	IniFile.GetInteger("Init", "Count", 1, &nWorldCount);
	if (nWorldCount > MAX_SUBWORLD)
	{
		return FALSE;
	}
	
	for (int i = 0; i < nWorldCount; i++)
	{
		sprintf((char*)szKeyName, "World%02d", i);	// %02 chi load dc 99 map %03 se load 999 map sao tui thay co 70 map la max roi
		IniFile.GetInteger("World", szKeyName, 1000, &nWorldID);
#ifdef _SERVER
		SubWorld[i].m_nIndex = i;
		if (!SubWorld[i].LoadMap(nWorldID))
		{
		//	printf("Errot Load Map: %d \n",nWorldID);
		}
		else
		{
			//printf("Load Map: %d \n",nWorldID);
		}
#endif
	}







 #ifdef _SERVER

 try
 {


 pPlayScript =(KLuaScript*) g_GetScript("\\script\\admin\\banghoi\\banghoi.lua");
    if (!pPlayScript)
	{
	}
   else
    {

		int nTopIndex = 0;	
		pPlayScript->SafeCallBegin(&nTopIndex);	
		pPlayScript->CallFunction("LoadTongMapMain",0,"");
		pPlayScript->SafeCallEnd(nTopIndex);
		
        
    }

}

catch(...)
{
		printf("Xay ra loi chay Spcrit dieu khien \\script\\admin\\banghoi\\banghoi.lua !!!!!");
}

#endif



	return TRUE;
}

int nActiveRegionCount;

void KSubWorldSet::MainLoop()
{
	m_nLoopRate++;

	#ifdef _SERVER
	if (m_nLoopRate%1080 == 0)
	{
		try
		{
			pPlayScript =(KLuaScript*) g_GetScript("\\script\\admin\\dieukhien.lua");
			if (!pPlayScript)
			{
			}
			else
			{
				int nTopIndex = 0;		
				pPlayScript->SafeCallBegin(&nTopIndex);	
				pPlayScript->CallFunction("RunTimer",0,"");
				pPlayScript->SafeCallEnd(nTopIndex);
			}
		}

		catch(...)
		{
			printf("Xay ra loi chay Spcrit dieu khien \\script\\admin\\dieukhien.lua !!!!!");
		}

		try
		{
			pPlayScript =(KLuaScript*) g_GetScript("\\script\\hoatdong\\congthanh\\npccongthanh.lua");
			if (!pPlayScript)
			{
			}
			else
			{
				int nTopIndex = 0;	
				pPlayScript->SafeCallBegin(&nTopIndex);	
				pPlayScript->CallFunction("RunTimer",0,"");
				pPlayScript->SafeCallEnd(nTopIndex);
			}
		}
		catch(...)
		{
			printf("Xay ra loi chay Spcrit dieu khien \\script\\hoatdong\\congthanh\\npccongthanh.lua !!!!!");
		}

		try
		{
			pPlayScript =(KLuaScript*) g_GetScript("\\script\\hoatdong\\liendau\\sugiald.lua");
			if (!pPlayScript)
			{
			}
			else
			{
				int nTopIndex = 0;
				pPlayScript->SafeCallBegin(&nTopIndex);	
				pPlayScript->CallFunction("RunTimer",0,"");
				pPlayScript->SafeCallEnd(nTopIndex);
			}
		}
		catch(...)
		{
			printf("Xay ra loi chay Spcrit dieu khien \\script\\hoatdong\\liendau\\sugiald.lua !!!!!");
		}

		try
		{
			pPlayScript =(KLuaScript*) g_GetScript("\\script\\hoatdong\\loidai\\sugiald.lua");
			if (!pPlayScript)
			{
			}
			else
			{
				int nTopIndex = 0;
				pPlayScript->SafeCallBegin(&nTopIndex);	
				pPlayScript->CallFunction("RunTimer",0,"");
				pPlayScript->SafeCallEnd(nTopIndex);
			}
		}
		catch(...)
		{
			printf("Xay ra loi chay Spcrit dieu khien \\script\\hoatdong\\loidai\\sugiald.lua !!!!!");
		}

		try
		{
			pPlayScript =(KLuaScript*) g_GetScript("\\script\\hoatdong\\songbac\\chusongbac.lua");
			if (!pPlayScript)
			{
			}
			else
			{
				int nTopIndex = 0;
				pPlayScript->SafeCallBegin(&nTopIndex);	
				pPlayScript->CallFunction("RunTimer",0,"");
				pPlayScript->SafeCallEnd(nTopIndex);
			}
		}
		catch(...)
		{
			printf("Xay ra loi chay Spcrit dieu khien \\script\\hoatdong\\songbac\\chusongbac.lua !!!!!");
		}

		try
		{
			pPlayScript =(KLuaScript*) g_GetScript("\\script\\hoatdong\\daihoianhhung\\sugia.lua");
			if (!pPlayScript)
			{
			}
			else
			{
				int nTopIndex = 0;			
				pPlayScript->SafeCallBegin(&nTopIndex);	
				pPlayScript->CallFunction("RunTimer",0,"");
				pPlayScript->SafeCallEnd(nTopIndex);
			}
		}

		catch(...)
		{
			printf("Xay ra loi chay Spcrit dieu khien \\script\\hoatdong\\daihoianhhung\\sugia.lua !!!!!");
		}

		try
		{
			pPlayScript =(KLuaScript*) g_GetScript("\\script\\hoatdong\\loidai\\sugialoidai.lua");
			if (!pPlayScript)
			{
			}
			else
			{
				int nTopIndex = 0;		
				pPlayScript->SafeCallBegin(&nTopIndex);	
				pPlayScript->CallFunction("RunTimer",0,"");
				pPlayScript->SafeCallEnd(nTopIndex);
			}
		}
		catch(...)
		{
			printf("Xay ra loi chay Spcrit dieu khien \\script\\hoatdong\\loidai\\sugialoidai.lua !!!!!");
		}

		try
		{
			pPlayScript =(KLuaScript*) g_GetScript("\\script\\hoatdong\\tongkim\\dieukhientk.lua");
			if (!pPlayScript)
			{
			}
			else
			{
				int nTopIndex = 0;	
				pPlayScript->SafeCallBegin(&nTopIndex);	
				pPlayScript->CallFunction("RunTimer",0,"");
				pPlayScript->SafeCallEnd(nTopIndex);
			}
		}
		catch(...)
		{
			printf("Xay ra loi chay Spcrit dieu khien \\script\\hoatdong\\tongkim\\dieukhientk.lua !!!!!");
		}
	}
	#endif

	#ifndef _SERVER
	//	if (!(m_nLoopRate % 20))
	//		SendClientCmdPing();
		// 游戏音乐的处理
		this->m_cMusic.Play(SubWorld[0].m_SubWorldID, SubWorld[0].m_dwCurrentTime, Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_FightMode);
	#endif
	#ifdef _SERVER
	//		g_GlobalMissionArray.Activate();
	#endif
	nActiveRegionCount = 0;
	for (int i = 0; i < MAX_SUBWORLD; i++)
	{
		SubWorld[i].Activate();
	#ifndef _SERVER
			NpcSet.CheckBalance();
	#endif
	}

	//	if ((m_nLoopRate % 100) == 0)
	//		printf("Region:%d:%d\n", m_nLoopRate, nActiveRegionCount);
	#ifdef _SERVER
		PlayerSet.AutoSave();
	#endif
}

void KSubWorldSet::MessageLoop()
{
	
	for (int i = 0; i < MAX_SUBWORLD; i++)
	{
		SubWorld[i].MessageLoop();
	}
}

BOOL KSubWorldSet::SendMessage(int nSubWorldID, DWORD dwMsgType, int nParam1, int nParam2, int nParam3)
{
	KWorldMsgNode *pNode = NULL;
	
	pNode = new KWorldMsgNode;
	if (!pNode)
		return FALSE;
	
	pNode->m_dwMsgType	= dwMsgType;
	pNode->m_nParam[0]	= nParam1;
	pNode->m_nParam[1]	= nParam2;
	pNode->m_nParam[2]	= nParam3;
	if (pNode->m_dwMsgType == 4001) g_DebugLog("Send !!!!");
	return SubWorld[nSubWorldID].m_WorldMessage.Send(pNode);
}

void KSubWorldSet::Close()
{
	for (int i = 0; i < MAX_SUBWORLD; i++)
	{
		SubWorld[i].Close();
	}
	NpcSet.RemoveAll();
#ifndef _SERVER
	Player[CLIENT_PLAYER_INDEX].m_ItemList.RemoveAll();
	Player[CLIENT_PLAYER_INDEX].m_cTeam.Release();
	g_Team[0].Release();
	m_cMusic.Stop();
#endif
}
#ifndef _SERVER
void KSubWorldSet::Paint()
{
	SubWorld[0].Paint();
}
#endif

#ifdef _SERVER
void KSubWorldSet::GetRevivalPosFromId(DWORD dwSubWorldId, int nRevivalId, POINT* pPos)
{
	if (!pPos)
		return;

//	int nIdx = SearchWorld(dwSubWorldId);
//	if (nIdx >= 0)
//	{
	KIniFile IniFile;
	char	szKeyName[32];
	char	szSection[32];
	g_SetFilePath("\\settings");
	IniFile.Load("RevivePos.ini");
	sprintf(szSection, "%d", dwSubWorldId);
	sprintf(szKeyName, "%d", nRevivalId);
	int nX = 51200;
	int nY = 102400;
	IniFile.GetInteger2(szSection, szKeyName, &nX, &nY);
	
	pPos->x = nX;
	pPos->y = nY;
//	}
}
#endif
