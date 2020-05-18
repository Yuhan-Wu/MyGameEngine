#pragma once
#include "WeakPointer.h"
#include "GameObject.h"
#include "GLib.h"

class RenderInfo: public IGameObjectController
{
public:
	struct Renderable {
		GLib::Sprites::Sprite* sprites;

		Renderable() {
			sprites = nullptr;
		}
		Renderable(GLib::Sprites::Sprite* p_sprites) {
			sprites = p_sprites;
		}
	};
	RenderInfo();
	RenderInfo(SmartPointer<GameObject>, GLib::Sprites::Sprite*);
	~RenderInfo();

	void EndUpdate(float) override;
	void ReleaseExtra() override;
	std::string Name() override{
		return "RenderInfo";
	}
private:
	void Render();

	WeakPointer<GameObject> wp_GameObject;
	GLib::Sprites::Sprite* p_Sprite;
};