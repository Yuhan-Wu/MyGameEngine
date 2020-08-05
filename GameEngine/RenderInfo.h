#pragma once
#include "WeakPointer.h"
#include "GameData.h"
#include "GLib.h"

class RenderInfo: public IGameObjectComponent
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

	void Render() override;
	void ReleaseExtra() override;
	ComponentType Name() override{
		return ComponentType::RenderInfo;
	}
private:
	

	WeakPointer<GameObject> wp_GameObject;
	GLib::Sprites::Sprite* p_Sprite;
};