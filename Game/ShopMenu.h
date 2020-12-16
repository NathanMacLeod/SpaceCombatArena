#pragma once
#include "Drawable.h"
#include "Ore.h"
#include "Player.h"
#include <string.h>

class ShopMenu {
public:

	static enum MenuType { Buy, Vault, Sell };
	static const int N_MENUS = 3;
	static std::string getMenuName(MenuType menu);

	ShopMenu(SpaceMinerGame* game);
	void clickShop(SpaceMinerGame* game);
	void draw(SpaceMinerGame* g);
	void setIsOpen(bool s);
	bool getOpen();
private:
	struct ShopItem {
		ShopItem(const std::string& title, const std::string& description, int cost);
		void draw(PixelEngine3D* g, int x, int y, int width, int height, bool hovered);
		
		Player::UpgradeProfile upgradeChanges;
		bool canPurchase(int inventory[Ore::N_TYPES], int vault[Ore::N_TYPES], int money);
		void purchase(Player* p, int vault[Ore::N_TYPES], Player::UpgradeProfile* shopProfile);
		ShopItem* next;
		std::string title;
		std::string description;
		int cost;
		int oreReq[Ore::N_TYPES];
	};

	bool itemHovered(int itemN, int mouseX, int mouseY);
	Vector3D getItemCoords(int itemN);

	void drawOreDisplay(SpaceMinerGame* g, int x, int y, int nCols, int tileSize, int tileGap, int* inv);
	void getOreCoord(int* xOut, int* yOut, SpaceMinerGame* g, int x, int y, int nCols, int tileSize, int tileGap, int i);
	int getOreHovered(SpaceMinerGame* g, int x, int y, int nCols, int tileSize, int tileGap);
	bool sellButtonHovered(SpaceMinerGame* g);

	MenuType currentMenu;

	Player::UpgradeProfile profile;
	int vault[Ore::N_TYPES];
	int toSell[Ore::N_TYPES];

	int x;
	int y;
	int margin;
	int width;
	int height;
	int itemWidth;
	int itemHeight;
	int itemGap;
	int tabWidth;
	int tabHeight;
	int tabGap;
	bool isOpen;

	int vaultInvX;
	int vaultInvY;
	int vaultInvSize;
	int vaultInvGap;
	int vaultNCols;

	int vaultDispX;
	int vaultDispY;
	int vaultDispSize;
	int vaultDispGap;

	int sellButtonX;
	int sellButtonY;
	int sellButtonWidth;
	int sellButtonHeight;
	std::vector<ShopItem*> shopItems;
};