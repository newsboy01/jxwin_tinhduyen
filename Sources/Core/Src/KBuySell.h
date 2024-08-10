#ifndef	KBuySellH
#define	KBuySellH

#define	BUY_SELL_SCALE		10

class KItem;

typedef struct
{
	int		nGenre;
	int		nDetailType;
	int		nParticularType;
	int		nSeriesReq;
	int		nLevel;
	int		nPrice;
} ItemGenerate;

class KInventory;

class KBuySell
{
public:
	KBuySell();
	~KBuySell();
	BOOL			Init();
	int				GetWidth() { return m_Width; }
	int				GetHeight() { return m_Height; }
	KItem*			GetItem(int nIndex);
	int				GetItemIndex(int nShop, int nIndex);
private:
	int**			m_SellItem;
	KItem*			m_Item;
	int				m_Width;
	int				m_Height;
	int				m_MaxItem;
public:
#ifdef _SERVER
	void			OpenSale(int nPlayerIdx, int nShop, int nSlect);
	BOOL			Buy(int nPlayerIdx, int nBuy, int nBuyIdx, int nPlace, int nX, int nY);	// 买第几个买卖列表中的第几项道具
	BOOL			Sell(int nPlayerIdx, int nBuy, int nIdx);
	BOOL	AutoBuyItem(int nPlayerIdx, BYTE nItemGenre, BYTE nDetailType, BYTE nLevel, BYTE nBuyNumber);
	BOOL			AutoSell(int nPlayerIdx, int nBuy, int nIdx, int nBuyNumber);
#endif
#ifndef _SERVER
	KInventory*		m_pShopRoom;
	void			OpenSale(int nShop, int nSlect);
	void			PaintItem(int nIdx, int nX, int nY);
#endif
};

extern KBuySell	BuySell;
#endif
