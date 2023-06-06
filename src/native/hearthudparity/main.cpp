#include "../../../toolchain/ndk/arm/include/c++/4.9.x/cmath"

#include <hook.h>
#include <symbol.h>

#include <innercore/global_context.h>
#include <innercore/vtable.h>

#include "main.hpp"


void HeartHudParityModule::blit(ScreenContext* ctx, mce::MaterialPtr* material, float x, float y, float width, float height, const std::string& texture, float textureWidth, float textureHeight, float alpha) {
	float maxU = width / textureWidth;
	float maxV = height / textureHeight;
	ctx->tessellator->begin(4, false);
	ctx->tessellator->vertexUV(x, y + height, 0.0f, 0.0f, maxV);
	ctx->tessellator->vertexUV(x + width, y + height, 0.0f, maxU, maxV);
	ctx->tessellator->vertexUV(x + width, y, 0.0f, maxU, 0.0f);
	ctx->tessellator->vertexUV(x, y, 0.0f, 0.0f, 0.0f);
	ctx->shaderColor->setColor(mce::Color { 1.0f, 1.0f, 1.0f, alpha });
	RenderMesh::endTessellationAndRenderImmediately(*ctx, *ctx->tessellator, material, texture);
	ctx->shaderColor->setColor(mce::Color { 1.0f, 1.0f, 1.0f, 1.0f });
}


std::vector<Vec2> HeartHudParityModule::Data::healthBarOffsets;
int HeartHudParityModule::Data::playerHealth = 0;
int HeartHudParityModule::Data::lastPlayerHealth = 0;
long long HeartHudParityModule::Data::lastSystemTime = 0ll;
int HeartHudParityModule::Data::healthUpdateCounter = 0;


void HeartHudParityModule::generateHealthBarOffsets(float left, float top, int ticks, int maxHealth, int absorption, bool regeneration, int health, int uiProfileMultiplier) {
	Random* rand = GlobalContext::getLevel()->getRandom();
	rand->_setSeed(312871 * ticks);
	int healthBars = (int) ceilf((float) (maxHealth + absorption) / 2.0f);
	int healthRows = (int) ceilf((float) healthBars / 10.0f);
	int healthRowHeight = (int) fmaxf((float) (10 - (healthRows - 2)), 3.0f);
	int regeneratingHeartIndex = regeneration ? (ticks % (int) ceilf((float) maxHealth + 5.0f)) : -1;
	bool shouldAnimateHealth = health <= 4;
	int usingPlayerAbsorption = absorption;
	if(Data::healthBarOffsets.size() != healthBars) Data::healthBarOffsets.resize(healthBars, Vec2 { 0.0f, 0.0f });
	for(int i = healthBars - 1; i >= 0; --i) {
		int row = (int) ceilf((float) (i + 1) / 10.0f) - 1;
		float x = left + (float) (i % 10 * 8);
		int y = top + uiProfileMultiplier * row * healthRowHeight;
		if(shouldAnimateHealth) y += rand->nextInt(2) - 1;
		if(usingPlayerAbsorption <= 0 && i == regeneratingHeartIndex) y -= 2;
		if(usingPlayerAbsorption > 0) usingPlayerAbsorption -= 2 - usingPlayerAbsorption % 2;
		Vec2& point = Data::healthBarOffsets.at(i);
		point.x = x - left;
		point.y = y - top;
	}
}


void HeartHudParityModule::onRender(ScreenContext* ctx, Vec2* position, int ticks, int uiProfileMultiplier, float propagatedAlpha) {
	LocalPlayer* player = GlobalContext::getLocalPlayer();
	int playerHealth = player->getHealth();
	bool drawFlash = Data::healthUpdateCounter > ticks && (Data::healthUpdateCounter - ticks) / 3 % 2 == 1;
	if(playerHealth < Data::playerHealth && player->hurtResistantTime > 0) {
		Data::lastSystemTime = getTimeMilliseconds();
		Data::healthUpdateCounter = ticks + 20;
	}
	if(playerHealth > Data::playerHealth && player->hurtResistantTime > 0) {
		Data::lastSystemTime = getTimeMilliseconds();
		Data::healthUpdateCounter = ticks + 10;
	}
	long long currentTimestamp = getTimeMilliseconds();
	if(currentTimestamp - Data::lastSystemTime > 1000ll) {
		Data::playerHealth = playerHealth;
		Data::lastPlayerHealth = playerHealth;
		Data::lastSystemTime = currentTimestamp;
	}
	Data::playerHealth = playerHealth;
	int playerMaxHealth = player->getMaxHealth();
	int playerAbsorption = (int) ceilf(player->getAbsorptionAmount());
	bool regeneration = player->hasEffect(*MobEffect::getById(10));
	generateHealthBarOffsets(position->x, position->y, ticks, playerMaxHealth, playerAbsorption, regeneration, playerHealth, uiProfileMultiplier);
	mce::MaterialPtr material = mce::RenderMaterialGroup::common.getMaterial(HashedString("ui_textured_and_glcolor"));
	int usingPlayerAbsorption = playerAbsorption;
	for(int i = Data::healthBarOffsets.size() - 1; i >= 0; --i) {
		Vec2& offset = Data::healthBarOffsets.at(i);
		float x = position->x + offset.x;
		float y = position->y + offset.y;
		blit(ctx, &material, x, y, 9.0f, 9.0f, "textures/ui/heart_background", 9.0f, 9.0f, propagatedAlpha);
		if(drawFlash) blit(ctx, &material, x, y, 9.0f, 9.0f, "textures/ui/heart_blink", 9.0f, 9.0f, propagatedAlpha);
		if(usingPlayerAbsorption > 0) {
			if(usingPlayerAbsorption == playerAbsorption && playerAbsorption % 2 == 1) {
				blit(ctx, &material, x, y, 9.0f, 9.0f, "textures/ui/absorption_heart_half", 9.0f, 9.0f, propagatedAlpha);
				--usingPlayerAbsorption;
			} else {
				blit(ctx, &material, x, y, 9.0f, 9.0f, "textures/ui/absorption_heart", 9.0f, 9.0f, propagatedAlpha);
				usingPlayerAbsorption -= 2;
			}
		} else {
			std::string prefix("textures/ui/");
			if(player->hasEffect(*MobEffect::getById(19)) || player->hasEffect(*MobEffect::getById(25))) prefix = "textures/ui/poison_";
			if(player->hasEffect(*MobEffect::getById(20))) prefix = "textures/ui/wither_";
			int currentHealthDrawn = i * 2 + 1;
			if(drawFlash) {
				if(Data::lastPlayerHealth > Data::playerHealth) {
					if(currentHealthDrawn < Data::lastPlayerHealth) {
						blit(ctx, &material, x, y, 9.0f, 9.0f, prefix + "heart_flash", 9.0f, 9.0f, propagatedAlpha);
					}
					if(currentHealthDrawn == Data::lastPlayerHealth) {
						blit(ctx, &material, x, y, 9.0f, 9.0f, prefix + "heart_flash_half", 9.0f, 9.0f, propagatedAlpha);
					}
				}
			}
			if(currentHealthDrawn < Data::playerHealth) {
				blit(ctx, &material, x, y, 9.0f, 9.0f, prefix + "heart", 9.0f, 9.0f, propagatedAlpha);
			}
			if(currentHealthDrawn == Data::playerHealth) {
				blit(ctx, &material, x, y, 9.0f, 9.0f, prefix + "heart_half", 9.0f, 9.0f, propagatedAlpha);
			}
		}
	}
}


void HeartHudParityModule::fixupArmorOffset(Vec2& position, bool reverse) {
	int reverseMultiplier = reverse ? -1 : 1;
	VTABLE_FIND_OFFSET(ClientInstance_getOptions, _ZTV14ClientInstance, _ZNK14ClientInstance10getOptionsEv);
	Options* options = VTABLE_CALL<Options*>(ClientInstance_getOptions, GlobalContext::getMinecraftClient());
	int uiProfileMultiplier = options->getUIProfile() == 0 ? -1 : 1;
	LocalPlayer* player = GlobalContext::getLocalPlayer();
	int maxHealth = player->getMaxHealth();
	int absorption = (int) ceilf(player->getAbsorptionAmount());
	int healthBars = (int) ceilf((float) (maxHealth + absorption) / 2.0f);
	int healthRows = (int) ceilf((float) healthBars / 10.0f);
	int healthRowHeight = (int) fmaxf((float) (10 - (healthRows - 2)), 3.0f);
	position.y -= (float) (reverseMultiplier * uiProfileMultiplier * healthRows * 10);
	position.y += (float) (reverseMultiplier * uiProfileMultiplier * (healthRows * healthRowHeight + 7));
}


void HeartHudParityModule::initialize() {

	DLHandleManager::initializeHandle("libminecraftpe.so", "mcpe");

	HookManager::addCallback(
		SYMBOL("mcpe", "_ZN16HudHeartRendererC2Ev"),
		LAMBDA((HudHeartRenderer* renderer), {
			renderer->ticks = 0;
		}, ),
		HookManager::RETURN | HookManager::LISTENER
	);

	HookManager::addCallback(
		SYMBOL("mcpe", "_ZN16HudHeartRenderer6updateER15IClientInstanceR9UIControlRK7UIScene"),
		LAMBDA((HudHeartRenderer* renderer, ClientInstance& clientInstance, UIControl& control, const UIScene& scene), {
			renderer->ticks = (int) (getTimeS() * 20.0);
		}, ),
		HookManager::CALL | HookManager::LISTENER
	);

	HookManager::addCallback(
		SYMBOL("mcpe", "_ZN16HudHeartRenderer23_loadHorseHeartTexturesERN3mce12TextureGroupE"),
		LAMBDA((HookManager::CallbackController* controller, HudHeartRenderer* renderer, mce::TextureGroup& textureGroup), {
			controller->prevent();
		}, ),
		HookManager::CALL | HookManager::LISTENER | HookManager::CONTROLLER
	);

	HookManager::addCallback(
		SYMBOL("mcpe", "_ZN16HudHeartRenderer6renderER24MinecraftUIRenderContextR15IClientInstanceR9UIControliR13RectangleArea"),
		LAMBDA((HookManager::CallbackController* controller, HudHeartRenderer* renderer, MinecraftUIRenderContext& renderContext, ClientInstance& clientInstance, UIControl& control, int someInt, RectangleArea& area), {
			VTABLE_FIND_OFFSET(ClientInstance_getOptions, _ZTV14ClientInstance, _ZNK14ClientInstance10getOptionsEv);
			Options* options = VTABLE_CALL<Options*>(ClientInstance_getOptions, &clientInstance);
			int uiProfileMultiplier = options->getUIProfile() == 0 ? -1 : 1;
			onRender(renderContext.getScreenContext(), control.getPosition(), renderer->ticks, uiProfileMultiplier, renderer->propagatedAlpha);
		}, ),
		HookManager::REPLACE | HookManager::CONTROLLER
	);
	
	HookManager::addCallback(
		SYMBOL("mcpe", "_ZN16HudArmorRenderer6renderER24MinecraftUIRenderContextR15IClientInstanceR9UIControliR13RectangleArea"),
		LAMBDA((HudArmorRenderer* renderer, MinecraftUIRenderContext& renderContext, ClientInstance& clientInstance, UIControl& control, int someInt, RectangleArea& area), {
			fixupArmorOffset(*control.getPosition(), false);
		}, ),
		HookManager::CALL | HookManager::LISTENER
	);

	HookManager::addCallback(
		SYMBOL("mcpe", "_ZN16HudArmorRenderer6renderER24MinecraftUIRenderContextR15IClientInstanceR9UIControliR13RectangleArea"),
		LAMBDA((HudArmorRenderer* renderer, MinecraftUIRenderContext& renderContext, ClientInstance& clientInstance, UIControl& control, int someInt, RectangleArea& area), {
			fixupArmorOffset(*control.getPosition(), true);
		}, ),
		HookManager::RETURN | HookManager::LISTENER
	);

}


MAIN {
	Module* main_module = new HeartHudParityModule();
}