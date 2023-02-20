#pragma once

#include <cstdint>

#include "DiabloUI/ui_item.h"
#include "engine/clx_sprite.hpp"
#include "utils/sdl_geometry.h"

namespace devilution {

extern OptionalOwnedClxSpriteList ArtScrollBarBackground;
extern OptionalOwnedClxSpriteList ArtScrollBarThumb;
extern OptionalOwnedClxSpriteList ArtScrollBarArrow;
extern Uint16 ScrollBarWidth;

extern uint16_t ScrollBarArrowFrame_UP_ACTIVE;
extern uint16_t ScrollBarArrowFrame_UP;
extern uint16_t ScrollBarArrowFrame_DOWN_ACTIVE;
extern uint16_t ScrollBarArrowFrame_DOWN;

inline SDL_Rect UpArrowRect(const UiScrollbar &bar)
{
	return MakeSdlRect(
	    bar.m_rect.x,
	    bar.m_rect.y,
	    ScrollBarWidth,
	    bar.m_arrow[0].height());
}

inline SDL_Rect DownArrowRect(const UiScrollbar &bar)
{
	return MakeSdlRect(
	    bar.m_rect.x,
	    bar.m_rect.y + bar.m_rect.h - bar.m_arrow[0].height(),
	    ScrollBarWidth,
	    bar.m_arrow[0].height());
}

inline Uint16 BarHeight(const UiScrollbar &bar)
{
	return bar.m_rect.h - 2 * bar.m_arrow[0].height();
}

inline SDL_Rect BarRect(const UiScrollbar &bar)
{
	return MakeSdlRect(
	    bar.m_rect.x,
	    bar.m_rect.y + bar.m_arrow[0].height(),
	    ScrollBarWidth,
	    BarHeight(bar));
}

inline SDL_Rect ThumbRect(const UiScrollbar &bar, size_t selectedIndex, size_t numItems)
{
	constexpr int ThumbOffsetX = 3;
	const int thumbMaxY = BarHeight(bar) - bar.m_thumb.height();
	const int thumbY = static_cast<int>(selectedIndex * thumbMaxY / (numItems - 1));

	return MakeSdlRect(
	    bar.m_rect.x + ThumbOffsetX,
	    bar.m_rect.y + bar.m_arrow[0].height() + thumbY,
	    bar.m_rect.w - ThumbOffsetX,
	    bar.m_thumb.height());
}

void LoadScrollBar();
void UnloadScrollBar();

} // namespace devilution
