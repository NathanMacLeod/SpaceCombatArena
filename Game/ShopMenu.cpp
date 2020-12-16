#include "ShopMenu.h"
#include "SpaceMinerGame.h"

std::string ShopMenu::getMenuName(MenuType menu) {
	switch (menu) {
	case Buy:
		return std::string("SHOP");
	case Vault:
		return std::string("VAULT");
	case Sell:
		return std::string("SELL");
	}
}

ShopMenu::ShopMenu(SpaceMinerGame* g) {

	ShopItem* gunsight = new ShopItem(std::string("Lead Gunsight"), std::string("The HUD will calculate lead, displaying the\ncorrect position to aim in order to hit\nthe targeted enemy"), 7000);
	gunsight->oreReq[Ore::Copper] = 5;
	gunsight->oreReq[Ore::Gold] = 3;
	gunsight->upgradeChanges.hasGunsight = true;

	ShopItem* inventory = new ShopItem(std::string("Inventory: 1"), std::string("Increases the number of ore you can\ncarry from 12 to 16"), 3000);
	inventory->oreReq[Ore::Iron] = 5;
	inventory->upgradeChanges.inventoryLevel = 1;

	ShopItem* thrusters = new ShopItem(std::string("Thrusters: 1"), std::string("Increases acceleration in the forward\nand reverse directions"), 6000);
	thrusters->oreReq[Ore::Iron] = 3;
	thrusters->oreReq[Ore::Silver] = 1;
	thrusters->upgradeChanges.thrustLevel = 1;

	ShopItem* twingun = new ShopItem(std::string("Twin Cannons"), std::string("Upgrade from a single cannon\nto twin cannons for a higher\nrate of fire"), 15000);
	twingun->oreReq[Ore::NormalDebris] = 15;
	twingun->oreReq[Ore::Plutonium] = 3;
	twingun->upgradeChanges.gun = Player::Twin;

	ShopItem* radar = new ShopItem(std::string("Radar: 1"), std::string("Improves the range of where enemies will be\ndetected and shown on dispalys"), 5000);
	radar->oreReq[Ore::Copper] = 8;
	radar->upgradeChanges.radarLevel = 1;

	ShopItem* missiles = new ShopItem(std::string("Missiles"), std::string("Gain access to fire and forget missiles,\nespecially effective at longer ranges"), 20000);
	missiles->oreReq[Ore::Gold] = 8;
	missiles->oreReq[Ore::Gold] = 8;
	missiles->upgradeChanges.hasMissiles = true;

	shopItems.push_back(gunsight);
	shopItems.push_back(inventory);
	shopItems.push_back(thrusters);
	shopItems.push_back(twingun);
	shopItems.push_back(radar);
	shopItems.push_back(missiles);
	
	height = 0.8 * g->ScreenHeight();
	width = g->ScreenWidth() - 0.2 * g->ScreenHeight();
	x = (g->ScreenWidth() - width) / 2;
	y = (g->ScreenHeight() - height) / 2;
	margin = 0.025 * width;
	itemWidth = 0.45 * width;
	itemHeight = 0.15 * height;
	itemGap = (height - 2 * margin - 5 * itemHeight) / 4;

	tabWidth = width / 9;
	tabGap = tabWidth / 5;
	tabHeight = tabWidth / 3;

	vaultNCols = 7;
	vaultInvSize = 60;
	vaultInvGap = 6;
	vaultInvX = x + width / 2 - (vaultInvSize * vaultNCols + (vaultNCols - 1) * vaultInvGap) / 2;
	vaultInvY = y + height * 1.95 / 3;

	vaultDispSize = 75;
	vaultDispGap = 7;
	vaultDispX = x + width / 2 - (vaultDispSize * vaultNCols + (vaultNCols - 1) * vaultDispGap) / 2;
	vaultDispY = y + height / 8;

	sellButtonWidth = 100;
	sellButtonHeight = 60;
	sellButtonX = 1.5 * x + width - vaultInvX / 2 - sellButtonWidth / 2;
	sellButtonY = vaultInvY + vaultInvSize - sellButtonHeight / 2;

	currentMenu = Buy;

	isOpen = false;

	for(int i = 0; i < Ore::N_TYPES; i++) {
		vault[i] = 0;
	}
}

void ShopMenu::setIsOpen(bool b) {
	isOpen = b;
}

bool ShopMenu::getOpen() {
	return isOpen;
}

void ShopMenu::clickShop(SpaceMinerGame* game) {
	int mX = game->GetMouseX();
	int mY = game->GetMouseY();

	if (mY <= y && mY >= y - tabHeight) {
		for (int i = 0; i < N_MENUS; i++) {
			int tabX = x + tabGap + i * (tabGap + tabWidth);
			if (mX > tabX && mX < tabX + tabWidth) {
				MenuType m = (MenuType)i;

				if (currentMenu != Sell && m == Sell) {
					for (int i = 0; i < Ore::N_TYPES; i++) {
						toSell[i] = 0;
					}
				}

				currentMenu = m;
				return;
			}
		}
	}

	switch (currentMenu) {
	case Buy:
	{
		for (int i = 0; i < shopItems.size(); i++) {
			if (itemHovered(i, mX, mY)) {
				Player* p = game->getPlayer();
				if (shopItems.at(i)->canPurchase(p->getInventory(), vault, p->getMoney()) || true) {
					shopItems.at(i)->purchase(p, vault, &profile);
				}
				else {
					//add cant afford message stuff here later
				}
			}
		}
	}
		break;
	case Vault:
	{
		int selected = getOreHovered(game, vaultInvX, vaultInvY, vaultNCols, vaultInvSize, vaultInvGap);
		int* pInv = game->getPlayer()->getInventory();
		if (selected != -1) {
			if (pInv[selected] > 0) {
				if (game->GetMouse(0).bPressed) {
					vault[selected] += pInv[selected];
					pInv[selected] = 0;
				}
			}
		}
	}
		break;
	case Sell:
	{
		int selected = getOreHovered(game, vaultDispX, vaultDispY, vaultNCols, vaultDispSize, vaultDispGap);
		if (selected != -1) {
			if (vault[selected] - toSell[selected] > 0) {
				if (game->GetMouse(0).bPressed) {
					toSell[selected]++;
				}
			}
		}

		selected = getOreHovered(game, vaultInvX, vaultInvY, vaultNCols, vaultInvSize, vaultInvGap);
		if (selected != -1) {
			if (toSell[selected] > 0) {
				if (game->GetMouse(0).bPressed) {
					toSell[selected]--;
				}
			}
		}

		if (sellButtonHovered(game)) {
			int value = 0;
			Player* player = game->getPlayer();
			for (int i = 0; i < Ore::N_TYPES; i++) {
				value += toSell[i] * Ore::getValue((Ore::Material)i);
				vault[i] -= toSell[i];
				toSell[i] = 0;
			}
			player->alterMoney(value);
		}
	}
		break;
	}
	
}

ShopMenu::ShopItem::ShopItem(const std::string& title, const std::string& description, int cost) {
	this->title = title;
	this->description = description;
	this->cost = cost;
	for (int i = 0; i < Ore::N_TYPES; i++) {
		this->oreReq[i] = 0;
	}
	next = nullptr;
}

void ShopMenu::ShopItem::draw(PixelEngine3D* g, int x, int y, int width, int height, bool hovered) {
	static char buff[64];

	g->DrawRect(x, y,width,  height, hovered? olc::RED : olc::CYAN);
	g->DrawString(x + 0.05 * width, y + 4, title, olc::CYAN, 1);
	g->DrawString(x + 0.05 * width, y + 22, description, olc::WHITE, 1);
	sprintf_s(buff, "REQ: $%d ", cost);
	g->DrawString(x + 0.05 * width, y + 50, buff, olc::WHITE, 1);

	double xPos = x + 0.05 * width + 8 * strlen(buff);
	for (int i = 0; i < Ore::N_TYPES; i++) {
		if (oreReq[i] > 0) {
			Ore::Material m = (Ore::Material) i;
			sprintf_s(buff, "%s: %d ", Ore::getName(m).c_str(), oreReq[i]);
			g->DrawString(xPos, y + 50, buff, Ore::getColor(m), 1);
			xPos += 8 * strlen(buff);
		}
	}
}

bool ShopMenu::ShopItem::canPurchase(int inventory[Ore::N_TYPES], int vault[Ore::N_TYPES], int money) {
	if (money < cost) {
		return false;
	}
	for (int i = 0; i < Ore::N_TYPES; i++) {
		if (inventory[i] + vault[i] < oreReq[i]) {
			return false;
		}
	}
	return true;
}

void ShopMenu::ShopItem::purchase(Player* p, int vault[Ore::N_TYPES], Player::UpgradeProfile* shopProfile) {
	shopProfile->applyUpgrade(upgradeChanges);
	p->applyUpgradeProfile(*shopProfile);

	p->alterMoney(-cost);
	int* inventory = p->getInventory();
	for (int i = 0; i < Ore::N_TYPES; i++) {
		int req = oreReq[i];
		if (vault[i] < req) {
			req -= vault[i];
			vault[i] = 0;
			inventory[i] -= req;
		}
		else {
			vault[i] -= req;
		}
	}

}

void ShopMenu::draw(SpaceMinerGame* g) {
	static char buff[128];
	g->FillRect(x, y, width, height, olc::BLACK);
	g->DrawRect(x, y, width, height, olc::CYAN);

	for (int i = 0; i < N_MENUS; i++) {
		int tabX = x + tabGap + i * (tabGap + tabWidth);
		int tabY = y - tabHeight;

		g->FillRect(tabX, tabY, tabWidth, tabHeight, olc::BLACK);
		g->DrawRect(tabX, tabY, tabWidth, tabHeight, olc::CYAN);

		if ((MenuType)i == currentMenu) {
			g->DrawLine(tabX + 1, y, tabX + tabWidth - 1, y, olc::BLACK);
		}

		int textS = 1;
		std::string tabTitle = getMenuName((MenuType)i);
		g->DrawString(tabX + tabWidth / 2.0 - strlen(tabTitle.c_str()) * 4 * textS, tabY + tabHeight / 2.0 - 4 * textS, tabTitle, olc::CYAN, textS);
	}

	switch (currentMenu) {
	case Buy:
	{
		for (int i = 0; i < shopItems.size(); i++) {
			ShopItem* s = shopItems.at(i);
			Vector3D itemCoords = getItemCoords(i);
			s->draw(g, itemCoords.x, itemCoords.y, itemWidth, itemHeight, itemHovered(i, g->GetMouseX(), g->GetMouseY()));
		}
	}
		break;
	case Vault:
	{
		std::string title("Vault");
		//std::string sellMsg("Click to sell 1 for money");
		int titleSize = 2;
		g->DrawString(x + width / 2 - title.size() * titleSize * 4, y + height / 32, title, olc::CYAN, titleSize);
		//g->DrawString(x + width / 2 - sellMsg.size() * 4, y + height * 1.25 / 16, sellMsg, olc::CYAN, 1);
		drawOreDisplay(g, vaultDispX, vaultDispY, vaultNCols, vaultDispSize, vaultDispGap, vault);
		std::string invTitle("Inventory");
		g->DrawString(x + width / 2 - invTitle.size() * titleSize * 4, y + height / 1.9, invTitle, olc::CYAN, titleSize);
		std::string invMessage("Click to deposit");
		g->DrawString(x + width / 2 - invMessage.size() * 4, y + height / 1.7, invMessage, olc::CYAN, 1);

		drawOreDisplay(g, vaultInvX, vaultInvY, vaultNCols, vaultInvSize, vaultInvGap, g->getPlayer()->getInventory());
	}
		break;
	case Sell:
	{
		std::string title("Vault");
		std::string sellMsg("Click to move to To Sell");
		int titleSize = 2;
		g->DrawString(x + width / 2 - title.size() * titleSize * 4, y + height / 32, title, olc::CYAN, titleSize);
		//g->DrawString(x + width / 2 - sellMsg.size() * 4, y + height * 1.25 / 16, sellMsg, olc::CYAN, 1);

		static int leftOver[Ore::N_TYPES];
		int sellValue = 0;
		for (int i = 0; i < Ore::N_TYPES; i++) {
			sellValue += toSell[i] * Ore::getValue((Ore::Material)i);
			leftOver[i] = vault[i] - toSell[i];
		}

		drawOreDisplay(g, vaultDispX, vaultDispY, vaultNCols, vaultDispSize, vaultDispGap, leftOver);
		std::string sellTitle("To Sell");
		g->DrawString(x + width / 2 - sellTitle.size() * titleSize * 4, y + height / 1.9, sellTitle, olc::CYAN, titleSize);

		sprintf_s(buff, "Currently will sell for: $%d", sellValue);
		std::string sellMessage(buff);
		g->DrawString(x + width / 2 - sellMessage.size() * 4, y + height / 1.7, sellMessage, olc::CYAN, 1);

		drawOreDisplay(g, vaultInvX, vaultInvY, vaultNCols, vaultInvSize, vaultInvGap, toSell);

		g->FillRect(sellButtonX, sellButtonY, sellButtonWidth, sellButtonHeight, olc::Pixel(30, 30, 30));
		std::string sellBtn("SELL");
		olc::Pixel col = (sellButtonHovered(g)) ? olc::RED : olc::CYAN;
		g->DrawString(sellButtonX + sellButtonWidth / 2 - sellBtn.size() * 4, sellButtonY + sellButtonHeight / 2 - 4, sellBtn, col, 1);
		g->DrawRect(sellButtonX, sellButtonY, sellButtonWidth, sellButtonHeight, col);
	}
		break;
	}

}

bool ShopMenu::sellButtonHovered(SpaceMinerGame* g) {
	int mX = g->GetMouseX();
	int mY = g->GetMouseY();
	return mX >= sellButtonX && mX <= sellButtonX + sellButtonWidth && mY >= sellButtonY && mY <= sellButtonY + sellButtonWidth;
}

Vector3D ShopMenu::getItemCoords(int itemN) {
	int nPerCol = 5;
	return Vector3D(x + margin + (itemN / nPerCol) * (2 * margin + itemWidth), y + margin + (itemN % nPerCol) * (itemHeight + itemGap), 0);
}

bool ShopMenu::itemHovered(int itemN, int mouseX, int mouseY) {
	Vector3D pos = getItemCoords(itemN);
	return mouseX >= pos.x && mouseX <= pos.x + itemWidth && mouseY >= pos.y && mouseY <= pos.y + itemHeight;
}

void ShopMenu::drawOreDisplay(SpaceMinerGame* g, int x, int y, int nCols, int tileSize, int tileGap, int* inv) {
	static char buff[64];
	int nRows = Ore::N_TYPES / nCols + ((Ore::N_TYPES % nCols != 0)? 1 : 0);
	int hovered = getOreHovered(g, x, y, nCols, tileSize, tileGap);
	for (int i = 0; i < nCols * nRows; i++) {
		int tX;
		int tY;
		getOreCoord(&tX, &tY, g, x, y, nCols, tileSize, tileGap, i);
		int count = (i >= Ore::N_TYPES) ? 0 : inv[i];
		Ore::Material m = (Ore::Material)i;
		if (count > 0) {
			olc::Pixel color = Ore::getColor(m);
			olc::Pixel textColor = olc::Pixel(255 - color.r, 255 - color.g, 255 - color.b);

			g->FillRect(tX, tY, tileSize, tileSize, color);

			sprintf_s(buff, "%s: %d", Ore::getAbbrev(m), count);
			g->DrawString(tX + tileSize / 2.0 - 4 * strlen(buff), tY + tileSize / 2.0 - 4, buff, textColor, 1);
		}
		else {
			g->FillRect(tX, tY, tileSize, tileSize, olc::Pixel(30, 30, 30));
		}
		g->DrawRect(tX, tY, tileSize, tileSize, ((i == hovered)? olc::RED : olc::CYAN));
	}
}

void ShopMenu::getOreCoord(int* xOut, int* yOut, SpaceMinerGame* g, int x, int y, int nCols, int tileSize, int tileGap, int i) {
	int nRows = Ore::N_TYPES / nCols + ((Ore::N_TYPES % nCols != 0) ? 1 : 0);
	int xItr = i % nCols;
	int yItr = i / nCols;
	*xOut = x + xItr * tileSize + ((xItr > 0) ? xItr : 0) * tileGap;
	*yOut = y + yItr * tileSize + ((yItr > 0) ? yItr : 0) * tileGap;
}

int ShopMenu::getOreHovered(SpaceMinerGame* g, int x, int y, int nCols, int tileSize, int tileGap) {
	int nRows = Ore::N_TYPES / nCols + ((Ore::N_TYPES % nCols != 0) ? 1 : 0);
	int mouseX = g->GetMouseX();
	int mouseY = g->GetMouseY();
	for (int i = 0; i < nCols * nRows; i++) {
		int tX;
		int tY;
		getOreCoord(&tX, &tY, g, x, y, nCols, tileSize, tileGap, i);
		if (mouseX >= tX && mouseX <= tX + tileSize && mouseY >= tY && mouseY <= tY + tileSize) {
			if (i >= Ore::N_TYPES) {
				return -1;
			}
			return i;
		}
	}
	return -1;
}