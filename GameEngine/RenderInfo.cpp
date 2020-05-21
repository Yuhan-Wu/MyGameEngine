#include "pch.h"
#include "RenderInfo.h"

RenderInfo::RenderInfo()
{
	p_Sprite = nullptr;
}

RenderInfo::RenderInfo(SmartPointer<GameObject> p_GameObject, GLib::Sprites::Sprite* sprites) {
	wp_GameObject = p_GameObject;
	p_Sprite = sprites;
}

RenderInfo::~RenderInfo()
{
	wp_GameObject = nullptr;
	if(p_Sprite) GLib::Sprites::Release(p_Sprite);
}

void RenderInfo::Render() {
	Point2D position2D = wp_GameObject.Get()->m_Position2D;
	GLib::Point2D Offset = { position2D.X(), position2D.Y()};
	GLib::Sprites::RenderSprite(*p_Sprite, Offset, 0.0f);
}

void RenderInfo::EndUpdate(float delta_time) {
	Render();
}

void RenderInfo::ReleaseExtra() {
	if (p_Sprite) GLib::Sprites::Release(p_Sprite);
	wp_GameObject = nullptr;
}