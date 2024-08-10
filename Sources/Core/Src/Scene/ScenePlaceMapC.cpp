// ***************************************************************************************
// 地图的类定义实现
// Copyright : Kingsoft 2003
// Author    : wooy(wu yue)
// CreateTime: 2003-7-8
// ***************************************************************************************
#include "KCore.h"
#include "ScenePlaceMapC.h"
#include "crtdbg.h"
#include "../KPlayer.h"
#include "../ImgRef.h"
#include "../../Represent/iRepresent/iRepresentShell.h"
#include "../GameDataDef.h"
#include "KScenePlaceRegionC.h"
#include "SceneDataDef.h"
#include    "../../Engine/Src/KSG_StringProcess.h"
#include "KScenePlaceC.h"
extern KScenePlaceC	g_ScenePlace;

#define	PLACE_MAP_FILE_NAME_APPEND		"24.jpg"
#define	PLACE_MAP_SAVE_SECTION			"MAIN"
#define	MAP_SETTING_FILE				"\\Ui\\Setting.ini"

#define	RIGHT_BOTTOM_NO_LIMIT			0x7fffffff

KScenePlaceMapC::KScenePlaceMapC()
{
	m_bHavePicMap = false;
	m_bInited = false;
	m_bLine = false;
	AutoMove = TRUE;
	m_bHidePicMap = false;
	m_szEntireMapFile[0] = 0;
	m_EntireMapLTPosition.x = m_EntireMapLTPosition.y = 0;
	m_FocusPosition.x = m_FocusPosition.y = 0;
	m_FocusLimit.left = m_FocusLimit.right = m_FocusLimit.top = m_FocusLimit.bottom = 0;
	m_PicLoadedLTPosition.x = m_PicLoadedLTPosition.y = 0;
	m_Size.cx = m_Size.cy = 0;
	m_MapCoverArea.left = m_MapCoverArea.right = m_MapCoverArea.top = m_MapCoverArea.bottom = 0;
	m_PaintCell.left = m_PaintCell.right = m_PaintCell.top = m_PaintCell.bottom = 0;
	m_uMapShowElems = SCENE_PLACE_MAP_ELEM_NONE;
	memset(&m_ElemsList, 0, sizeof(m_ElemsList));
	m_pEntireMap = NULL;
	m_GreenLineDes.x = m_GreenLineDes.y = -123456; // toa do
	m_FlagPos.x = m_FlagPos.y = -123456; // toa do
}

KScenePlaceMapC::~KScenePlaceMapC()
{
	Terminate();
}

//初始化
bool KScenePlaceMapC::Initialize()
{
	if (m_bInited == false && g_pRepresent)
	{
		int nCount = MAP_CELL_MAX_RANGE * MAP_CELL_MAX_RANGE;
		int	nIndex, i;
		MAP_CELL* pCell = &m_ElemsList[0][0];
		for (nIndex = i = 0; nIndex < nCount && i < 0x03335688; i++)
		{
			sprintf(pCell->szImageName, "*PlaceMap_%d*", i);
			pCell->uImageId = g_pRepresent->CreateImage(
				pCell->szImageName, MAP_CELL_MAP_WIDTH, MAP_CELL_MAP_HEIGHT, ISI_T_BITMAP16);
			if (pCell->uImageId)
			{
				pCell++;
				nIndex++;
			}
		}
		m_bInited = true;

		KIniFile	ColorSetting;
		char		szBuffer[64];
		if (ColorSetting.Load(MAP_SETTING_FILE))
		{
			const char* pcszTemp;
			KRColor		Color;
			Color.Color_dw = 0;

			// 主角颜色
			ColorSetting.GetString("Map", "SelfColor", "255,255,255", szBuffer, sizeof(szBuffer));
            pcszTemp = szBuffer;
            Color.Color_b.r = KSG_StringGetInt(&pcszTemp, 255);
            KSG_StringSkipSymbol(&pcszTemp, ',');
			Color.Color_b.g = KSG_StringGetInt(&pcszTemp, 255);
            KSG_StringSkipSymbol(&pcszTemp, ',');
			Color.Color_b.b = KSG_StringGetInt(&pcszTemp, 255);
			m_uSelfColor = Color.Color_dw;
			
			// 队友颜色
			ColorSetting.GetString("Map", "TeammateColor", "255,255,255", szBuffer, sizeof(szBuffer));
            pcszTemp = szBuffer;
			Color.Color_b.r = KSG_StringGetInt(&pcszTemp, 255);
            KSG_StringSkipSymbol(&pcszTemp, ',');
			Color.Color_b.g = KSG_StringGetInt(&pcszTemp, 255);
            KSG_StringSkipSymbol(&pcszTemp, ',');
			Color.Color_b.b = KSG_StringGetInt(&pcszTemp, 255);
			m_uTeammateColor = Color.Color_dw;

			// 其他玩家颜色
			ColorSetting.GetString("Map", "PlayerColor", "255,255,255", szBuffer, sizeof(szBuffer));
            pcszTemp = szBuffer;
			Color.Color_b.r = KSG_StringGetInt(&pcszTemp, 255);
            KSG_StringSkipSymbol(&pcszTemp, ',');
			Color.Color_b.g = KSG_StringGetInt(&pcszTemp, 255);
            KSG_StringSkipSymbol(&pcszTemp, ',');
			Color.Color_b.b = KSG_StringGetInt(&pcszTemp, 255);
			m_uPlayerColor = Color.Color_dw;

			// 战斗npc颜色
			ColorSetting.GetString("Map", "FightNpcColor", "255,255,255", szBuffer, sizeof(szBuffer));
            pcszTemp = szBuffer;
			Color.Color_b.r = KSG_StringGetInt(&pcszTemp, 255);
            KSG_StringSkipSymbol(&pcszTemp, ',');
			Color.Color_b.g = KSG_StringGetInt(&pcszTemp, 255);
            KSG_StringSkipSymbol(&pcszTemp, ',');
			Color.Color_b.b = KSG_StringGetInt(&pcszTemp, 255);
			m_uFightNpcColor = Color.Color_dw;

			// 普通npc颜色
			ColorSetting.GetString("Map", "NormalNpcColor", "255,255,255", szBuffer, sizeof(szBuffer));
            pcszTemp = szBuffer;
			Color.Color_b.r = KSG_StringGetInt(&pcszTemp, 255);
            KSG_StringSkipSymbol(&pcszTemp, ',');
			Color.Color_b.g = KSG_StringGetInt(&pcszTemp, 255);
            KSG_StringSkipSymbol(&pcszTemp, ',');
			Color.Color_b.b = KSG_StringGetInt(&pcszTemp, 255);
			m_uNormalNpcColor = Color.Color_dw;
		}
	}

	return m_bInited;
}

//结束对象功能。释放对象的全部数据与动态构造的资源。
void KScenePlaceMapC::Terminate()
{
	if (m_bInited && g_pRepresent)
	{
		for (int v = 0; v < MAP_CELL_MAX_RANGE; v++)
		{
			for (int h = 0; h < MAP_CELL_MAX_RANGE; h++)
			{
				if (m_ElemsList[v][h].uImageId)
				{
					g_pRepresent->FreeImage(m_ElemsList[v][h].szImageName);
					m_ElemsList[v][h].uImageId = 0;
				}
				m_ElemsList[v][h].szImageName[0] = 0;
			}
		}
	}
}

//设置场景地图的包含的元素
void KScenePlaceMapC::SetShowElemsFlag(unsigned int uShowElemsFlag)
{
	m_uMapShowElems = uShowElemsFlag;
	if (m_uMapShowElems & SCENE_PLACE_MAP_ELEM_PIC)
		SetFocusPosition(m_FocusPosition.x, m_FocusPosition.y, true);
}

//读取设置
void KScenePlaceMapC::Load(KIniFile* pSetting, const char* pszScenePlaceRootPath)
{
	if (Initialize() == false)
		return;
	Free();
	m_bHavePicMap = false;
	if (pSetting && pszScenePlaceRootPath)
	{
		sprintf(m_szEntireMapFile, "%s"PLACE_MAP_FILE_NAME_APPEND, pszScenePlaceRootPath);
		m_bHavePicMap = g_FileExists(m_szEntireMapFile);
		if (m_bHavePicMap)
		{
			m_EntireMapLTPosition.x = -1;
			pSetting->GetInteger2(PLACE_MAP_SAVE_SECTION, "MapLTRegionIndex",
				(int*)&m_EntireMapLTPosition.x, (int*)m_EntireMapLTPosition.y);
			if (m_EntireMapLTPosition.x == -1)
			{
				RECT	rc;
				pSetting->GetRect(PLACE_MAP_SAVE_SECTION, "rect", &rc);
				m_EntireMapLTPosition.x = rc.left;
				m_EntireMapLTPosition.y = rc.top;
			}
			if (m_EntireMapLTPosition.x != -1)
			{
				m_EntireMapLTPosition.x *= KScenePlaceRegionC::RWPP_AREGION_WIDTH;
				m_EntireMapLTPosition.y *= KScenePlaceRegionC::RWPP_AREGION_HEIGHT;

				m_PicLoadedLTPosition.x = 0;
				m_PicLoadedLTPosition.y = 0;

				m_FocusLimit.left = m_EntireMapLTPosition.x + (m_Size.cx * MAP_SCALE_H / 2);
				m_FocusLimit.top  = m_EntireMapLTPosition.y + (m_Size.cy * MAP_SCALE_V / 2);
				m_FocusLimit.bottom = m_FocusLimit.right = RIGHT_BOTTOM_NO_LIMIT;

				m_bHavePicMap = true;
			}
			else
			{
				m_bHavePicMap = false;
			}
		}
	}
}

//设置场景地图的焦点
void KScenePlaceMapC::SetFocusPosition(int nX, int nY, bool bChangedRegion)
{
	if (nX < m_FocusLimit.left)
		m_FocusPosition.x = m_FocusLimit.left;
	else if (nX > m_FocusLimit.right)
		m_FocusPosition.x = m_FocusLimit.right;
	else
		m_FocusPosition.x = nX;
	if (nY < m_FocusLimit.top)
		m_FocusPosition.y = m_FocusLimit.top;
	else if (nY > m_FocusLimit.bottom)
		m_FocusPosition.y = m_FocusLimit.bottom;
	else
		m_FocusPosition.y = nY;

	m_MapCoverArea.left = m_FocusPosition.x / MAP_SCALE_H - m_Size.cx / 2;
	m_MapCoverArea.top  = m_FocusPosition.y / MAP_SCALE_V - m_Size.cy / 2;
	m_MapCoverArea.right  = m_MapCoverArea.left + m_Size.cx;
	m_MapCoverArea.bottom = m_MapCoverArea.top  + m_Size.cy;

	if (m_uMapShowElems & SCENE_PLACE_MAP_ELEM_PIC)
	{
		if (bChangedRegion && m_bHavePicMap)
		{
			POINT	CellIndex;
			CellIndex.x = (m_FocusPosition.x - m_PicLoadedLTPosition.x) / MAP_CELL_SCENE_WIDTH;
			CellIndex.y = (m_FocusPosition.y - m_PicLoadedLTPosition.y) / MAP_CELL_SCENE_HEIGHT;
			if (CellIndex.x != MAP_CELL_CENTRE_INDEX || //MAP_CELL_FOCUS_INDEX_MIN || CellIndex.x > MAP_CELL_FOCUS_INDEX_MAX ||
				CellIndex.y != MAP_CELL_CENTRE_INDEX)	//MAP_CELL_FOCUS_INDEX_MIN || CellIndex.y > MAP_CELL_FOCUS_INDEX_MAX)
			{
				m_PicLoadedLTPosition.x += (CellIndex.x - MAP_CELL_CENTRE_INDEX) * MAP_CELL_SCENE_WIDTH;
				m_PicLoadedLTPosition.y += (CellIndex.y - MAP_CELL_CENTRE_INDEX) * MAP_CELL_SCENE_HEIGHT;
				FillCellsPicInfo();
			}
		}
		CalcPicLayout();
	}
}

void KScenePlaceMapC::FillCellsPicInfo()
{
	if (m_pEntireMap == NULL)
	{
		if (m_bHavePicMap)
		{
			KBitmapDataBuffInfo	Info;
			short *pBuff = (short*)g_pRepresent->GetBitmapDataBuffer(
				m_ElemsList[0][0].szImageName, &Info);
			if (pBuff)
			{
				g_pRepresent->ReleaseBitmapDataBuffer(
						m_ElemsList[0][0].szImageName, pBuff);
				unsigned int uMask16 = -1;
				if (Info.eFormat == BDBF_16BIT_555)
					uMask16 = RGB_555;
				else if (Info.eFormat == BDBF_16BIT_565)
					uMask16 = RGB_565;
				if (uMask16 >= 0)
					m_pEntireMap = get_jpg_image(m_szEntireMapFile, uMask16);
			}			
		}
		if (m_pEntireMap)
		{
			m_FocusLimit.right = m_FocusLimit.left + (m_pEntireMap->nWidth - m_Size.cx)* MAP_SCALE_H;
			m_FocusLimit.bottom = m_FocusLimit.top + (m_pEntireMap->nHeight - m_Size.cy)* MAP_SCALE_V;
			if (m_FocusLimit.right < m_FocusLimit.left)
				m_FocusLimit.right = m_FocusLimit.left;
			if (m_FocusLimit.bottom < m_FocusLimit.top)
				m_FocusLimit.bottom = m_FocusLimit.top;
		}
		else
		{
			m_bHavePicMap = false;
			return;
		}
	}

	int	nStartX = (m_PicLoadedLTPosition.x - m_EntireMapLTPosition.x) / MAP_SCALE_H;
	int nStartY = (m_PicLoadedLTPosition.y - m_EntireMapLTPosition.y) / MAP_SCALE_V;

	SIZE	PicEntireSize;
	PicEntireSize.cx = m_pEntireMap->nWidth;
	PicEntireSize.cy = m_pEntireMap->nHeight;
	POINT	DestPos;
	
	int	h, v, x, y, nFromX, nFromY, nToX, nToY;
	for (v = 0, y = nStartY; v < MAP_CELL_MAX_RANGE; v++, y += MAP_CELL_MAP_HEIGHT)
	{
		nToY = y + MAP_CELL_MAP_HEIGHT;
		bool bCleared = false;
		if (y < 0 || nToY > PicEntireSize.cy)
		{//纵向没有全部落在图内
			for (h = 0; h < MAP_CELL_MAX_RANGE; h++)
			{
				g_pRepresent->ClearImageData(m_ElemsList[v][h].szImageName,
					m_ElemsList[v][h].uImageId, m_ElemsList[v][h].sISPosition);
			}
			bCleared = true;
		}
		if (y < PicEntireSize.cy && nToY > 0)
		{
			if (y >= 0)
			{
				nFromY = y;
				DestPos.y = 0;
			}
			else
			{
				nFromY = 0;
				DestPos.y = -y;
			}

			if (nToY > PicEntireSize.cy)
				nToY = PicEntireSize.cy;

			for (h = 0, x = nStartX; h < MAP_CELL_MAX_RANGE; h++, x += MAP_CELL_MAP_WIDTH)
			{
				nToX = x + MAP_CELL_MAP_WIDTH;
				if (bCleared == false && (x < 0 || nToX > PicEntireSize.cx))
				{//横向没有全部落在图内
					g_pRepresent->ClearImageData(m_ElemsList[v][h].szImageName,
						m_ElemsList[v][h].uImageId, m_ElemsList[v][h].sISPosition);
				}

				if (x < PicEntireSize.cx && nToX > 0)
				{
					if (x >= 0)
					{
						nFromX = x;
						DestPos.x = 0;
					}
					else
					{
						nFromX = 0;
						DestPos.x = -x;
					}
					if (nToX > PicEntireSize.cx)
						nToX = PicEntireSize.cx;
					
					short *pBuf = (short*)g_pRepresent->GetBitmapDataBuffer(
						m_ElemsList[v][h].szImageName, NULL);
					if (pBuf)
					{
						short* pEntire = (short*)(&m_pEntireMap->Data) +
							nFromY * PicEntireSize.cx + nFromX;
						short* pDest = pBuf + MAP_CELL_MAP_WIDTH * DestPos.y + DestPos.x;
						for (int i = nFromY; i < nToY; i++)
						{
							memcpy(pDest, pEntire, (nToX - nFromX) * 2);
							pEntire += PicEntireSize.cx;
							pDest += MAP_CELL_MAP_WIDTH;
						}
						g_pRepresent->ReleaseBitmapDataBuffer(
							m_ElemsList[v][h].szImageName, pBuf);
					}
				}
			}
		}
	}
}

void KScenePlaceMapC::CalcPicLayout()
{
	POINT	ShowLTPos;

	ShowLTPos.x = m_MapCoverArea.left - m_PicLoadedLTPosition.x / MAP_SCALE_H ;
	ShowLTPos.y = m_MapCoverArea.top  - m_PicLoadedLTPosition.y / MAP_SCALE_V ;
//	ShowLTPos.x = (m_FocusPosition.x - m_PicLoadedLTPosition.x) / MAP_SCALE_H - m_Size.cx / 2;
//	ShowLTPos.y = (m_FocusPosition.y - m_PicLoadedLTPosition.y) / MAP_SCALE_V - m_Size.cy / 2;

	m_PaintCell.left = ShowLTPos.x / MAP_CELL_MAP_WIDTH;
	m_PaintCell.top = ShowLTPos.y / MAP_CELL_MAP_HEIGHT;

	m_FirstCellSkipWidth.cx = ShowLTPos.x - m_PaintCell.left * MAP_CELL_MAP_WIDTH;
	m_FirstCellSkipWidth.cy = ShowLTPos.y - m_PaintCell.top * MAP_CELL_MAP_HEIGHT;

	int nTemp = m_Size.cx + m_FirstCellSkipWidth.cx + MAP_CELL_MAP_WIDTH - 1;
	m_PaintCell.right = nTemp / MAP_CELL_MAP_WIDTH;
	m_LastCellSkipHeight.cx = nTemp - MAP_CELL_MAP_WIDTH * m_PaintCell.right;
	m_PaintCell.right += m_PaintCell.left;

	nTemp = m_Size.cy + m_FirstCellSkipWidth.cy + MAP_CELL_MAP_HEIGHT - 1;
	m_PaintCell.bottom = nTemp / MAP_CELL_MAP_HEIGHT;
	m_LastCellSkipHeight.cy = nTemp - MAP_CELL_MAP_HEIGHT * m_PaintCell.bottom;
	m_PaintCell.bottom += m_PaintCell.top;
}

//清除
void KScenePlaceMapC::Free()
{
	if (m_pEntireMap)
	{
		release_image(m_pEntireMap);
		m_pEntireMap = NULL;
	}
}

//绘制
void KScenePlaceMapC::PaintMap(int nX, int nY)
{
	m_MapPos.left = nX; m_MapPos.top = nY; // toa do
	m_MapPos.right = m_MapPos.left + m_Size.cx; m_MapPos.bottom = m_MapPos.top + m_Size.cy; // toa do
	if (m_bHavePicMap && g_pRepresent)
	{
		//----绘制缩略图----
		if (m_uMapShowElems & SCENE_PLACE_MAP_ELEM_PIC)
			PaintMapPic(nX, nY);

		//---绘制队友位置----
		PaintCharacters(nX, nY);

		//---绘制自己位置----
		int nNpcIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
		if (nNpcIdx >= 0 && Npc[nNpcIdx].m_RegionIndex >= 0)
		{
			int nNpcX = LOWORD(Npc[nNpcIdx].m_dwRegionID) * MAP_A_REGION_NUM_MAP_PIXEL_H +
				Npc[nNpcIdx].m_MapX * 2;
			int nNpcY = HIWORD(Npc[nNpcIdx].m_dwRegionID) * MAP_A_REGION_NUM_MAP_PIXEL_V +
				Npc[nNpcIdx].m_MapY;

			if (nNpcX >= m_MapCoverArea.left && nNpcX < m_MapCoverArea.right &&
				nNpcY >= m_MapCoverArea.top  && nNpcY < m_MapCoverArea.bottom)
			{
				KRUShadow	FootSpot;
				FootSpot.Color.Color_dw = m_uSelfColor;
				FootSpot.oPosition.nX = nX + nNpcX - m_MapCoverArea.left - 1;
				FootSpot.oPosition.nY = nY + nNpcY - m_MapCoverArea.top  - 1;
				FootSpot.oEndPos.nX = FootSpot.oPosition.nX + 3;
				FootSpot.oEndPos.nY = FootSpot.oPosition.nY + 3;
				g_pRepresent->DrawPrimitives(1, &FootSpot, RU_T_SHADOW, true);
			}
			// DrawGreenLine(nX + m_GreenLineDes.x - m_MapCoverArea.left - 1,nY + m_GreenLineDes.y - m_MapCoverArea.top  - 1, nX + nNpcX - m_MapCoverArea.left - 1, nY + nNpcY - m_MapCoverArea.top  - 1); // toa do			
			PaintFindPos(nX + m_FlagPos.x - m_MapCoverArea.left - 1,nY + m_FlagPos.y - m_MapCoverArea.top  - 1, nX + nNpcX - m_MapCoverArea.left - 1, nY + nNpcY - m_MapCoverArea.top  - 1);
				
		}
	}
}

void KScenePlaceMapC::SetHidePicMap(bool bHide)
{
	m_bHidePicMap = bHide;
}
//绘制队友位置

void KScenePlaceMapC::PaintFindPos(int nX, int nY, int nNpcPosX, int nNpcPosY)
{
	int nNpcPosIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	int nNpcIdx = 0;
	int nNpcX = LOWORD(Npc[nNpcPosIdx].m_dwRegionID) * MAP_A_REGION_NUM_MAP_PIXEL_H + Npc[nNpcPosIdx].m_MapX * 2;
	int nNpcY = HIWORD(Npc[nNpcPosIdx].m_dwRegionID) * MAP_A_REGION_NUM_MAP_PIXEL_V + Npc[nNpcPosIdx].m_MapY;
	bool nPaintFindSymbol = true;

	
	if (m_FlagPos.x - nNpcX > -5 && m_FlagPos.x - nNpcX < 5 && m_FlagPos.y - nNpcY > -5 && m_FlagPos.y - nNpcY < 5)
		g_ScenePlace.bPaintFlag = FALSE;
	if (nX <= 0 || g_ScenePlace.bPaintFlag == false)
		return;

	if (nNpcPosIdx >= 0 && Npc[nNpcPosIdx].m_RegionIndex >= 0)
	{
		if (nNpcX < m_MapPos.left)
			nNpcX = m_MapPos.left;
		if (nNpcY < m_MapPos.top)
			nNpcY = m_MapPos.top;
		if (nNpcX > m_MapPos.right)
			nNpcX = m_MapPos.right;
		if (nNpcY > m_MapPos.bottom)
			nNpcY = m_MapPos.bottom;
			
		if (nX < m_MapPos.left)
		{
			nX = m_MapPos.left;
			nPaintFindSymbol = false;
		}
		if (nY < m_MapPos.top)
		{
			nY = m_MapPos.top;
			nPaintFindSymbol = false;
		}
		if (nX > m_MapPos.right)
		{
			nX = m_MapPos.right;
			nPaintFindSymbol = false;
		}
		if (nY > m_MapPos.bottom)
		{
			nY = m_MapPos.bottom;
			nPaintFindSymbol = false;
		}
		if (nNpcPosX < m_MapPos.left)
			nNpcPosX = m_MapPos.left;
		if (nNpcPosY < m_MapPos.top)
			nNpcPosY = m_MapPos.top;
		if (nNpcPosX > m_MapPos.right)
			nNpcPosX = m_MapPos.right;
		if (nNpcPosY > m_MapPos.bottom)
			nNpcPosY = m_MapPos.bottom;			
		KRULine m_LineFindPos;
		m_LineFindPos.oPosition.nX = nX;
		m_LineFindPos.oPosition.nY = nY;
		m_LineFindPos.oPosition.nZ = 0;
		m_LineFindPos.oEndPos.nX = nNpcPosX;
		m_LineFindPos.oEndPos.nY = nNpcPosY;
		m_LineFindPos.oEndPos.nZ = 0;

		m_LineFindPos.Color.Color_dw = 0xFF000000 | 0 << 16 | 255 << 8 | 0;

		if (nPaintFindSymbol)
		{
			KRUImage m_SymbolFindPos;
			m_SymbolFindPos.nType = ISI_T_SPR;
			m_SymbolFindPos.Color.Color_b.a = 255;
			m_SymbolFindPos.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
			m_SymbolFindPos.uImage = 0;
			m_SymbolFindPos.nISPosition = IMAGE_IS_POSITION_INIT;
			m_SymbolFindPos.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
			strcpy(m_SymbolFindPos.szImage, "\\spr\\ui3\\小地图\\地图小旗帜.spr");
			m_SymbolFindPos.nFrame = 0;
			m_SymbolFindPos.oPosition.nX = nX;
			m_SymbolFindPos.oPosition.nY = nY - m_MapPos.top + - 12;
			m_SymbolFindPos.oPosition.nZ = 1;
			g_pRepresent->DrawPrimitives(1, &m_SymbolFindPos, RU_T_IMAGE, 1);
		}
			char MapPos[80];
			sprintf(MapPos,"%d,%d", m_FlagPos.x/16, m_FlagPos.y/16);
			BYTE nFontSize = 12;
			BYTE nFontWidth = nFontSize / 2;
			int nNamePosLeftB = m_LineFindPos.oPosition.nX;
			int nNamePosLeftE = m_LineFindPos.oPosition.nX + (g_StrLen(MapPos) * nFontWidth) + nFontWidth;;
			int nNamePosY = m_LineFindPos.oPosition.nY - 5;
			if (nNamePosLeftE > m_MapPos.right)
				nNamePosLeftB -= nNamePosLeftE - m_MapPos.right;
			if (nNamePosY - 15 < m_MapPos.top)
				nNamePosY = m_MapPos.top + 15;
			if (nNamePosY + nFontSize > m_MapPos.bottom)
				nNamePosY -= nNamePosY + nFontSize - m_MapPos.bottom;
			if (m_bLine)
			{
				g_pRepresent->DrawPrimitives(1, &m_LineFindPos, RU_T_LINE, 1);
				g_pRepresent->OutputText(nFontSize, MapPos, KRF_ZERO_END, nNamePosLeftB, nNamePosY, 0xffff00);	
				if (AutoMove)
				{
					int nX1 = m_FlagPos.x / 2 * 32;
					int nY1 = m_FlagPos.y* 32;
					Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SendCommand(do_run, nX1, nY1);
					SendClientCmdRun(nX1, nY1);	
				}
			}
	}
}

void KScenePlaceMapC::PaintCharacters(int nX, int nY)
{
	bool bCharacters = (m_uMapShowElems & SCENE_PLACE_MAP_ELEM_CHARACTER) != 0;
	bool bPartners = (m_uMapShowElems & SCENE_PLACE_MAP_ELEM_PARTNER) != 0;
	if (bCharacters == false || bPartners == false)
		return;

#define	MAX_NUM_PARTNER		50
	POINT		oPartnerPos[MAX_NUM_PARTNER];
	int			nNumPartner = 0;
	int			nIsInTeam = Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag;

#define	MAX_NUM_CHARACTER	100
	KRUShadow	FootSpot[MAX_NUM_CHARACTER];
	int			nNumSpot = 0;

	// 显示其他玩家和普通npc
	int nNpcIdx = 0;
	while (nNpcIdx = NpcSet.GetNextIdx(nNpcIdx))
	{
		if (Npc[nNpcIdx].m_RegionIndex == -1)
			continue;
		
		int nNpcX = LOWORD(Npc[nNpcIdx].m_dwRegionID) * MAP_A_REGION_NUM_MAP_PIXEL_H +
						Npc[nNpcIdx].m_MapX * 2;
		int nNpcY = HIWORD(Npc[nNpcIdx].m_dwRegionID) * MAP_A_REGION_NUM_MAP_PIXEL_V +
						Npc[nNpcIdx].m_MapY;
		if (nNpcX < m_MapCoverArea.left || nNpcX >= m_MapCoverArea.right ||
			nNpcY < m_MapCoverArea.top  || nNpcY >= m_MapCoverArea.bottom)
		{
			continue;
		}

		bool			bValidNpc = false;
		unsigned int	uColor;

		if (Npc[nNpcIdx].m_Kind == kind_normal && bCharacters)
		{
			if (Npc[nNpcIdx].m_CurrentCamp == Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentCamp &&
					 Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(128) > 0)
			{
			bValidNpc = true;
			uColor = 0x0099FF99;
			}
			else
			{
			bValidNpc = true;
			uColor = m_uFightNpcColor;
			}
		}
		else if (Npc[nNpcIdx].m_Kind == kind_dialoger && bCharacters)
		{
		
			bValidNpc = true;
			uColor = m_uNormalNpcColor;
			
		}
		else if (Npc[nNpcIdx].m_Kind == kind_player && nNpcIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex)
		{
			if (bPartners && //要显示队友
				(

				(nIsInTeam && //主角是组队状态
				((DWORD)g_Team[0].m_nCaptain == Npc[nNpcIdx].m_dwID ||	//是主角所在队伍的队长
					g_Team[0].FindMemberID(Npc[nNpcIdx].m_dwID) >= 0)	
				) 

				||

					(Npc[nNpcIdx].m_CurrentCamp == Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_CurrentCamp &&
					 Player[CLIENT_PLAYER_INDEX].m_cTask.GetSaveVal(128) > 0)
				)
					 &&
				nNumPartner < MAX_NUM_PARTNER)	
			{
				oPartnerPos[nNumPartner].x = nNpcX;
				oPartnerPos[nNumPartner].y = nNpcY;
				nNumPartner ++;
			}
			else if (bCharacters)
			{
				bValidNpc = true;
				uColor = m_uPlayerColor;
			}
		}
		if (bValidNpc)
		{

           if (Npc[nNpcIdx].m_InvisibilityState.nTime <= 0 || nNpcIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex)
		   {

			FootSpot[nNumSpot].Color.Color_dw = uColor;
			FootSpot[nNumSpot].oPosition.nX = nX + nNpcX - m_MapCoverArea.left - 1;
			FootSpot[nNumSpot].oPosition.nY = nY + nNpcY - m_MapCoverArea.top  - 1;
			FootSpot[nNumSpot].oEndPos.nX = FootSpot[nNumSpot].oPosition.nX + 3;
			FootSpot[nNumSpot].oEndPos.nY = FootSpot[nNumSpot].oPosition.nY + 3;
			nNumSpot++;
			if (nNumSpot == MAX_NUM_CHARACTER)
			{
				g_pRepresent->DrawPrimitives(MAX_NUM_CHARACTER, &FootSpot[0], RU_T_SHADOW, true);
				nNumSpot = 0;
			}
		   }
		}
	}

	if (nNumSpot)
	{
		g_pRepresent->DrawPrimitives(nNumSpot, &FootSpot[0], RU_T_SHADOW, true);
		nNumSpot = 0;
	}
	if (nNumPartner)
	{
		_ASSERT(nNumPartner <= MAX_NUM_CHARACTER);
		for (int i = 0; i < nNumPartner; i++)
		{
			FootSpot[i].Color.Color_dw = m_uTeammateColor;
			FootSpot[i].oPosition.nX = nX + oPartnerPos[i].x - m_MapCoverArea.left - 1;
			FootSpot[i].oPosition.nY = nY + oPartnerPos[i].y - m_MapCoverArea.top  - 1;
			FootSpot[i].oEndPos.nX = FootSpot[i].oPosition.nX + 3;
			FootSpot[i].oEndPos.nY = FootSpot[i].oPosition.nY + 3;
		}
		g_pRepresent->DrawPrimitives(nNumPartner, &FootSpot[0], RU_T_SHADOW, true);		
	}
}

//绘制缩略图
void KScenePlaceMapC::PaintMapPic(int nX, int nY)
{
	//_ASSERT(g_pRepresent);

	KRUImagePart	Img;
	Img.bRenderFlag = 0;
	Img.bRenderStyle = IMAGE_RENDER_STYLE_OPACITY;
	Img.Color.Color_dw = 0;
	Img.nFrame = 0;
	Img.nType = ISI_T_BITMAP16;
	Img.oPosition.nY = nY;

	for (int v = m_PaintCell.top; v < m_PaintCell.bottom; v++)
	{
		if (v != m_PaintCell.top)
			Img.oImgLTPos.nY = 0;
		else
			Img.oImgLTPos.nY = m_FirstCellSkipWidth.cy;
		if (v != m_PaintCell.bottom - 1)
			Img.oImgRBPos.nY = MAP_CELL_MAP_HEIGHT;
		else
			Img.oImgRBPos.nY = m_LastCellSkipHeight.cy;

		Img.oPosition.nX = nX;
		for (int h = m_PaintCell.left; h < m_PaintCell.right; h++)
		{
			if (h != m_PaintCell.left)
				Img.oImgLTPos.nX = 0;
			else
				Img.oImgLTPos.nX = m_FirstCellSkipWidth.cx;
			if (h != m_PaintCell.right - 1)
				Img.oImgRBPos.nX = MAP_CELL_MAP_WIDTH;
			else
				Img.oImgRBPos.nX = m_LastCellSkipHeight.cx;

			Img.nISPosition = m_ElemsList[v][h].sISPosition;
			strcpy(Img.szImage, m_ElemsList[v][h].szImageName);
			Img.uImage = m_ElemsList[v][h].uImageId;
			g_pRepresent->DrawPrimitives(1, &Img, RU_T_IMAGE_PART, true);
			m_ElemsList[v][h].sISPosition = Img.nISPosition;
			m_ElemsList[v][h].uImageId = Img.uImage;

			Img.oPosition.nX += Img.oImgRBPos.nX - Img.oImgLTPos.nX;
		}
		Img.oPosition.nY += Img.oImgRBPos.nY - Img.oImgLTPos.nY;
	};
}

//设置小地图的大小（单位：像素点）
void KScenePlaceMapC::SetSize(int cx, int cy)
{
	if (m_Size.cx != cx || m_Size.cy != cy)
	{
		if (cx > MAP_MAX_SUPPORT_WIDTH)
			cx = MAP_MAX_SUPPORT_WIDTH;
		else if (cx < 0)
			cx = 0;
		if (cy > MAP_MAX_SUPPORT_HEIGHT)
			cy = MAP_MAX_SUPPORT_HEIGHT;
		else if (cy < 0)
			cy = 0;
		int nDLimit = (cx - m_Size.cx) * MAP_SCALE_H / 2;
		m_FocusLimit.left += nDLimit;
		if (m_FocusLimit.right != RIGHT_BOTTOM_NO_LIMIT)
		{
			m_FocusLimit.right -= nDLimit;
			if (m_FocusLimit.right < m_FocusLimit.left)
				m_FocusLimit.right = m_FocusLimit.left;
		}
		nDLimit = (cy - m_Size.cy) * MAP_SCALE_V / 2;
		m_FocusLimit.top += nDLimit;
		if (m_FocusLimit.bottom != RIGHT_BOTTOM_NO_LIMIT)
		{
			m_FocusLimit.bottom -= nDLimit;
			if (m_FocusLimit.bottom < m_FocusLimit.top)
				m_FocusLimit.bottom = m_FocusLimit.top;
		}

		m_Size.cx = cx;
		m_Size.cy = cy;
		SetFocusPosition(m_FocusPosition.x, m_FocusPosition.y, true);
	}
}

//返回值表示是否有地图
int KScenePlaceMapC::GetMapRect(RECT* pRect)
{
	if (pRect)
	{
		pRect->left = m_EntireMapLTPosition.x;
		pRect->top  = m_EntireMapLTPosition.y;
		if (m_pEntireMap)
		{
			pRect->right = m_EntireMapLTPosition.x + m_pEntireMap->nWidth * MAP_SCALE_H;
			pRect->bottom = m_EntireMapLTPosition.y + m_pEntireMap->nHeight * MAP_SCALE_V;
		}
		else
		{
			pRect->right = m_EntireMapLTPosition.x;
			pRect->bottom = m_EntireMapLTPosition.y;
		}
	}
	return m_bHavePicMap;
}




void KScenePlaceMapC::SetGreenLine(int nX, int nY, BOOL bSearch, BOOL bDelete) // toa do
{
	b_mDelete = bDelete;
	if (!bSearch)	// neu khong tim se tinh toa do cach khac
	{
		if ( nX > m_MapPos.left && nY > m_MapPos.top && nX < m_MapPos.right && nY < m_MapPos.bottom)
		{
			m_GreenLineDes.x = nX - m_MapPos.left + m_MapCoverArea.left + 1;
			m_GreenLineDes.y = nY - m_MapPos.top + m_MapCoverArea.top + 1;
			bFlag = 0;
		}
		else
		{
			m_GreenLineDes.x = -123456;
			m_GreenLineDes.y = -123456;
		}
	}
	else	// tim toa do thi doi lai toa do
	{
		m_GreenLineDes.x = nX*MAP_SCALE_H;
		m_GreenLineDes.y = nY*MAP_SCALE_H;
	}
}

void KScenePlaceMapC::DrawGreenLine(int nX, int nY, int nX2, int nY2) // toa do
{
if (nX < -100000 || nY < -100000) return;
if (!b_mDelete) return;

KRULine Line;
bool limit = FALSE;

if (nX < m_MapPos.left)
{
nX = m_MapPos.left;
limit = TRUE;
}
if (nX > m_MapPos.right)
{
nX = m_MapPos.right;
limit = TRUE;
}
if (nY < m_MapPos.top)
{
nY = m_MapPos.top;
limit = TRUE;
}
if (nY > m_MapPos.bottom)
{
nY = m_MapPos.bottom;
limit = TRUE;
}

Line.oPosition.nX = nX;
Line.oPosition.nY = nY;

if (nY2 > m_MapPos.bottom)
	nY2 = m_MapPos.bottom;
if (nY2 < m_MapPos.top)
	nY2 = m_MapPos.top;

if (nX2 < m_MapPos.left)
	nX2 = m_MapPos.left;
if (nX2 > m_MapPos.right)
	nX2 = m_MapPos.right;

Line.oPosition.nZ = 0;
Line.oEndPos.nX = nX2;
Line.oEndPos.nY = nY2;
Line.oEndPos.nZ = 0;

Line.Color.Color_dw = 0xFF000000 | 0 << 16 | 255 << 8 | 0;
g_pRepresent->DrawPrimitives(1, &Line, RU_T_LINE, 1); 


if (!limit)
{
	KRUImage RPostSearchImage;
	RPostSearchImage.nType = ISI_T_SPR;
	RPostSearchImage.Color.Color_b.a = 255;
	RPostSearchImage.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
	RPostSearchImage.uImage = 0;
	RPostSearchImage.nISPosition = IMAGE_IS_POSITION_INIT;
	RPostSearchImage.bRenderFlag = 0;
   	strcpy(RPostSearchImage.szImage,"\\Spr\\Ui3\\小地图\\地图小旗帜.spr");
	RPostSearchImage.oPosition.nX = nX;
	RPostSearchImage.oPosition.nY = nY - 10;
	RPostSearchImage.oPosition.nZ = 0;
	RPostSearchImage.nFrame = 0;
    g_pRepresent->DrawPrimitives(1, &RPostSearchImage, RU_T_IMAGE, 1);
}

}

void KScenePlaceMapC::CalcFlagPos(int nX, int nY, bool bCalc, bool bLine)
{
if (bCalc)
{
if ( nX > m_MapPos.left && nY > m_MapPos.top && nX < m_MapPos.right && nY < m_MapPos.bottom)
{
m_FlagPos.x = nX - m_MapPos.left + m_MapCoverArea.left + 1;
m_FlagPos.y = nY - m_MapPos.top + m_MapCoverArea.top + 1;
}
else
{
m_FlagPos.x = 0;
m_FlagPos.y = 0;
}
}
else
{
m_FlagPos.x = nX * MAP_SCALE_H + 8;
m_FlagPos.y = nY * MAP_SCALE_H + 8;
}
if (bLine)
m_bLine = true;
else
m_bLine = false;
}