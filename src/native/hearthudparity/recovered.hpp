#include <string>

#ifndef HEARTHUDPARITY_RECOVERED_HPP
#define HEARTHUDPARITY_RECOVERED_HPP


class HashedString {
    public:
    char filler[20];
    HashedString(const char*);
};

namespace mce {
    struct Color {
        float r, g, b, a;
    };
    class MaterialPtr {
        public:
        char filler[8];
    };
    class RenderMaterialGroup {
        public:
        static RenderMaterialGroup common;
        mce::MaterialPtr getMaterial(const HashedString&);
    };
    class TextureGroup;
}

class ShaderColor {
    public:
    void setColor(const mce::Color&);
};

class Tessellator {
    public:
    void begin(int, bool);
    void vertexUV(float, float, float, float, float);
};

class ScreenContext {
    public:
    char filler1[28]; // 28
    ShaderColor* shaderColor; // 32
    char filler2[72]; // 104
    Tessellator* tessellator; // 108
};

class MinecraftUIRenderContext {
    public:
    ScreenContext* getScreenContext() const;
};

namespace RenderMesh {
    void endTessellationAndRenderImmediately(ScreenContext&, Tessellator&, void*, std::string const&);
}

struct Vec2 {
    float x, y;
};

class UIControl {
    public:
    Vec2* getPosition() const;
};

class UIScene;
class RectangleArea;

class UICustomRenderer {
    public:
    void** vtable; // 4
    float propagatedAlpha; // 8
};

class MinecraftUICustomRenderer : public UICustomRenderer {public:};

class HudHeartRenderer : public MinecraftUICustomRenderer {
    public:
    bool texturesWereLoaded; // 9 + 3
    int ticks; // 16
};

class HudArmorRenderer : public MinecraftUICustomRenderer {public:};

class Options {
    public:
    int getUIProfile() const;
};

class IClientInstance {public:};

class ClientInstance : public IClientInstance {
    public:
};

class MobEffect {
    public:
    static MobEffect* getById(int);
};

class Attribute;

class SharedAttributes {
    public:
    static Attribute ABSORPTION;
};

class AttributeInstance {
    public:
    float getCurrentValue() const;
};

class Actor {
    public:
    char filler1[572];
    int hurtResistantTime;
    AttributeInstance* getAttribute(const Attribute&) const;
    int getHealth() const;
    int getMaxHealth() const;
    bool hasEffect(const MobEffect&) const;
};

class Mob : public Actor {public:};

class Player : public Mob {
    public:
    bool isHurt();
    float getExhaustion() const;
    float getHunger() const;
    float getSaturation() const;
    inline float getAbsorptionAmount() const {
        return getAttribute(SharedAttributes::ABSORPTION)->getCurrentValue();
    }
};

class LocalPlayer : public Player {public:};

class Core {
    public:
    class Random {
        public:
        void _setSeed(unsigned int);
    };
};

class Random : public Core::Random {
    public:
    int nextInt(int);
};

class Level {
    public:
    Random* getRandom() const;
};

long long getTimeMilliseconds();
double getTimeS();


#endif //HEARTHUDPARITY_RECOVERED_HPP