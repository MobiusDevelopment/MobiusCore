/**
 * This file is part of the MobiusCore project.
 * See AUTHORS file for copyright information.
 */

#ifndef AUCTION_HOUSE_BOT_SELLER_H
#define AUCTION_HOUSE_BOT_SELLER_H

#include "Define.h"
#include "ItemTemplate.h"
#include "AuctionHouseBot.h"

struct ItemToSell
{
    uint32 Color;
    uint32 Itemclass;
};

typedef std::vector<ItemToSell> ItemsToSellArray;
typedef std::vector<std::vector<uint32>> AllItemsArray;

struct SellerItemClassInfo
{
    SellerItemClassInfo(): AmountOfItems(0), MissItems(0), Quantity(0), PriceRatio(0), RandomStackRatio(100) {}

    uint32 AmountOfItems;
    uint32 MissItems;
    uint32 Quantity;
    uint32 PriceRatio;
    uint32 RandomStackRatio;
};

struct SellerItemInfo
{
    SellerItemInfo(): AmountOfItems(0), MissItems(0), PriceRatio(0) {}

    uint32 AmountOfItems;
    uint32 MissItems;
    uint32 PriceRatio;

    SellerItemClassInfo ItemClassInfos[MAX_ITEM_CLASS];
};

class SellerConfiguration
{
public:
    SellerConfiguration(): LastMissedItem(0), _houseType(AUCTION_HOUSE_NEUTRAL), _minTime(1), _maxTime(72)
    {
    }

    ~SellerConfiguration() {}

    void Initialize(AuctionHouseType houseType)
    {
        _houseType = houseType;
    }

    AuctionHouseType GetHouseType() const { return _houseType; }

    uint32 LastMissedItem;

    void SetMinTime(uint32 value)
    {
        _minTime = value;
    }
    uint32 GetMinTime() const
    {
        return std::min(1u, std::min(_minTime, _maxTime));
    }

    void SetMaxTime(uint32 value) { _maxTime = value; }
    uint32 GetMaxTime() const { return _maxTime; }
    // Data access classified by item class and item quality
    void SetItemsAmountPerClass(AuctionQuality quality, ItemClass itemclass, uint32 amount) { _ItemInfo[quality].ItemClassInfos[itemclass].AmountOfItems = amount * _ItemInfo[quality].ItemClassInfos[itemclass].Quantity; }
    uint32 GetItemsAmountPerClass(AuctionQuality quality, ItemClass itemclass) const { return _ItemInfo[quality].ItemClassInfos[itemclass].AmountOfItems; }
    void SetItemsQuantityPerClass(AuctionQuality quality, ItemClass itemclass, uint32 qty) { _ItemInfo[quality].ItemClassInfos[itemclass].Quantity = qty; }
    uint32 GetItemsQuantityPerClass(AuctionQuality quality, ItemClass itemclass) const { return _ItemInfo[quality].ItemClassInfos[itemclass].Quantity; }
    void SetMissedItemsPerClass(AuctionQuality quality, ItemClass itemclass, uint32 found)
    {
        if (_ItemInfo[quality].ItemClassInfos[itemclass].AmountOfItems > found)
            _ItemInfo[quality].ItemClassInfos[itemclass].MissItems = _ItemInfo[quality].ItemClassInfos[itemclass].AmountOfItems - found;
        else
            _ItemInfo[quality].ItemClassInfos[itemclass].MissItems = 0;
    }
    uint32 GetMissedItemsPerClass(AuctionQuality quality, ItemClass itemclass) const { return _ItemInfo[quality].ItemClassInfos[itemclass].MissItems; }

    // Data for every quality of item
    void SetItemsAmountPerQuality(AuctionQuality quality, uint32 cnt) { _ItemInfo[quality].AmountOfItems = cnt; }
    uint32 GetItemsAmountPerQuality(AuctionQuality quality) const { return _ItemInfo[quality].AmountOfItems; }
    void SetPriceRatioPerQuality(AuctionQuality quality, uint32 value) { _ItemInfo[quality].PriceRatio = value; }
    uint32 GetPriceRatioPerQuality(AuctionQuality quality) const { return _ItemInfo[quality].PriceRatio; }
    void SetPriceRatioPerClass(ItemClass item, uint32 value) { _ItemInfo[0].ItemClassInfos[item].PriceRatio = value; }
    uint32 GetPriceRatioPerClass(ItemClass item) const { return _ItemInfo[0].ItemClassInfos[item].PriceRatio; }
    void SetRandomStackRatioPerClass(ItemClass item, uint32 value) { _ItemInfo[0].ItemClassInfos[item].RandomStackRatio = value; }
    uint32 GetRandomStackRatioPerClass(ItemClass item) const { return _ItemInfo[0].ItemClassInfos[item].RandomStackRatio; }

private:
    AuctionHouseType _houseType;
    uint32 _minTime;
    uint32 _maxTime;
    SellerItemInfo _ItemInfo[MAX_AUCTION_QUALITY];
};

// This class handle all Selling method
// (holder of AHB_Seller_Config data for each auction house type)
class GAME_API AuctionBotSeller : public AuctionBotAgent
{
public:
    typedef std::vector<uint32> ItemPool;

    AuctionBotSeller();
    ~AuctionBotSeller();

    bool Initialize() override;
    bool Update(AuctionHouseType houseType) override;

    void AddNewAuctions(SellerConfiguration& config);
    void SetItemsRatio(uint32 al, uint32 ho, uint32 ne);
    void SetItemsRatioForHouse(AuctionHouseType house, uint32 val);
    void SetItemsAmount(uint32(&vals)[MAX_AUCTION_QUALITY]);
    void SetItemsAmountForQuality(AuctionQuality quality, uint32 val);
    void LoadConfig();

private:
    SellerConfiguration _houseConfig[MAX_AUCTION_HOUSE_TYPE];

    ItemPool _itemPool[MAX_AUCTION_QUALITY][MAX_ITEM_CLASS];

    void LoadSellerValues(SellerConfiguration& config);
    uint32 SetStat(SellerConfiguration& config);
    bool GetItemsToSell(SellerConfiguration& config, ItemsToSellArray& itemsToSellArray, AllItemsArray const& addedItem);
    void SetPricesOfItem(ItemTemplate const* itemProto, SellerConfiguration& config, uint32& buyp, uint32& bidp, uint32 stackcnt);
    uint32 GetStackSizeForItem(ItemTemplate const* itemProto, SellerConfiguration& config) const;
    void LoadItemsQuantity(SellerConfiguration& config);
    static uint32 GetBuyModifier(ItemTemplate const* prototype);
    static uint32 GetSellModifier(ItemTemplate const* itemProto);
};

#endif
