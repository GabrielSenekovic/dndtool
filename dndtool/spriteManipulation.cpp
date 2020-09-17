#include "dndtool.h"

void DnDTool::MaskSprite(olc::Sprite* sprite)
{
	for (size_t i = 0; i < sprite->width * sprite->height; i++)
	{
		Gdiplus::Color c; int x = (int)i % sprite->width; int y = (int)i / sprite->width;
		iconMask->GetPixel(x, y, &c);
		olc::Pixel pixel = olc::Pixel(
			sprite->GetPixel({ x, y }).r,
			sprite->GetPixel({ x, y }).g,
			sprite->GetPixel({ x, y }).b,
			c.GetAlpha());
		sprite->SetPixel(x, y, pixel);
	}
}
void DnDTool::BrightenSprite(olc::Sprite* sprite)
{
	float brightness = 50;
	for (size_t i = 0; i < sprite->width * sprite->height; i++)
	{
		int x = (int)i % sprite->width; int y = (int)i / sprite->width;
		olc::Pixel pixel = olc::Pixel(
			min(sprite->GetPixel({ x, y }).r + brightness, 255),
			min(sprite->GetPixel({ x, y }).g + brightness, 255),
			min(sprite->GetPixel({ x, y }).b + brightness, 255),
			sprite->GetPixel({ x,y }).a);
		sprite->SetPixel(x, y, pixel);
	}
}
void DnDTool::GrayscaleSprite(olc::Sprite* sprite)
{
	for (size_t i = 0; i < sprite->width * sprite->height; i++)
	{
		int x = (int)i % sprite->width; int y = (int)i / sprite->width;
		float value = (sprite->GetPixel({ x, y }).r + sprite->GetPixel({ x, y }).g + sprite->GetPixel({ x, y }).b) / 3;
		olc::Pixel pixel = olc::Pixel(value, value, value, sprite->GetPixel({ x,y }).a);
		sprite->SetPixel(x, y, pixel);
	}
}
void DnDTool::FuseSprites(olc::Sprite* sprite_1, olc::Sprite* sprite_2)
{
	for (size_t i = 0; i < sprite_1->width * sprite_1->height; i++)
	{
		int x = (int)i % sprite_1->width; int y = (int)i / sprite_1->width;
		olc::Pixel pixel = sprite_2->GetPixel({ x,y }).a > 0 ?
			olc::Pixel
			(
				sprite_2->GetPixel({ x,y }).r, sprite_2->GetPixel({ x,y }).g, sprite_2->GetPixel({ x,y }).b
				) :
			olc::Pixel
			(
				sprite_1->GetPixel({ x,y }).r, sprite_1->GetPixel({ x,y }).g, sprite_1->GetPixel({ x,y }).b
				);
		sprite_1->SetPixel(x, y, pixel);
	}
}
/*void DnDTool::BrightenSprite(olc::Sprite* sprite)
{
	float brightness = 1.6f;
	for (size_t i = 0; i < sprite->width * sprite->height; i++)
	{
		int x = (int)i % sprite->width; int y = (int)i / sprite->width;
		olc::Pixel pixel = olc::Pixel(
			sprite->GetPixel({ x, y }).r +(255 - (int)sprite->GetPixel({ x, y }).r * brightness < 0 ? 0: 255 - sprite->GetPixel({ x, y }).r * brightness),
			sprite->GetPixel({ x, y }).g + (255 - (int)sprite->GetPixel({ x, y }).g * brightness< 0 ? 0: 255 - sprite->GetPixel({ x, y }).g * brightness),
			sprite->GetPixel({ x, y }).b + (255 - (int)sprite->GetPixel({ x, y }).b * brightness < 0 ? 0: 255 - sprite->GetPixel({ x, y }).b * brightness),
			sprite->GetPixel({ x,y }).a);
		sprite->SetPixel(x, y, pixel);
	}
}*/