#include <mod.h>

#include "recovered.hpp"

#ifndef HEARTHUDPARITY_MAIN_HPP
#define HEARTHUDPARITY_MAIN_HPP


class HeartHudParityModule : public Module {
	public:
	static void blit(ScreenContext* ctx, mce::MaterialPtr* material, float x, float y, float width, float height, const std::string& texture, float textureWidth, float textureHeight, float alpha);
	class Data {
		public:
		static std::vector<Vec2> healthBarOffsets;
		static int playerHealth;
		static int lastPlayerHealth;
		static long long lastSystemTime;
		static int healthUpdateCounter;
	};
	static void generateHealthBarOffsets(float left, float top, int ticks, int maxHealth, int absorption, bool regeneration, int health, int uiProfileMultiplier);
	static void onRender(ScreenContext* ctx, Vec2* position, int ticks, int uiProfileMultiplier, float propagatedAlpha);
	static void fixupArmorOffset(Vec2& position, bool reverse);
	HeartHudParityModule(): Module("hearthudparity") {};
	virtual void initialize();
};


#endif //HEARTHUDPARITY_MAIN_HPP