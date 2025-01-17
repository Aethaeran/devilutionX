#include "DiabloUI/text_input.hpp"

#include <memory>
#include <string>
#include <string_view>

#include <SDL.h>
#include <function_ref.hpp>

#ifdef USE_SDL1
#include "utils/sdl2_to_1_2_backports.h"
#endif

#include "utils/log.hpp"
#include "utils/parse_int.hpp"
#include "utils/sdl_ptrs.h"
#include "utils/str_cat.hpp"
#include "utils/utf8.hpp"

namespace devilution {

namespace {

bool HandleInputEvent(const SDL_Event &event, TextInputState &state,
    tl::function_ref<bool(std::string_view)> typeFn,
    [[maybe_unused]] tl::function_ref<bool(std::string_view)> assignFn)
{
	switch (event.type) {
	case SDL_KEYDOWN: {
		switch (event.key.keysym.sym) {
#ifndef USE_SDL1
		case SDLK_v:
			if ((SDL_GetModState() & KMOD_CTRL) != 0) {
				if (SDL_HasClipboardText() == SDL_TRUE) {
					std::unique_ptr<char, SDLFreeDeleter<char>> clipboard { SDL_GetClipboardText() };
					if (clipboard == nullptr || *clipboard == '\0') {
						Log("{}", SDL_GetError());
					} else {
						typeFn(clipboard.get());
					}
				}
			}
			return true;
#endif
		case SDLK_BACKSPACE:
			state.backspace();
			return true;
		case SDLK_DELETE:
			state.del();
			return true;
		case SDLK_LEFT:
			state.moveCursorLeft();
			return true;
		case SDLK_RIGHT:
			state.moveCursorRight();
			return true;
		case SDLK_HOME:
			state.setCursorToStart();
			return true;
		case SDLK_END:
			state.setCursorToEnd();
			return true;
		default:
			break;
		}
#ifdef USE_SDL1
		if ((event.key.keysym.mod & KMOD_CTRL) == 0 && event.key.keysym.unicode >= ' ') {
			std::string utf8;
			AppendUtf8(event.key.keysym.unicode, utf8);
			typeFn(utf8);
			return true;
		}
#endif
		return false;
	}
#ifndef USE_SDL1
	case SDL_TEXTINPUT:
#ifdef __vita__
		assignFn(event.text.text);
#else
		typeFn(event.text.text);
#endif
		return true;
	case SDL_TEXTEDITING:
		return true;
#endif
	default:
		return false;
	}
}

} // namespace

bool HandleTextInputEvent(const SDL_Event &event, TextInputState &state)
{
	return HandleInputEvent(
	    event, state, [&](std::string_view str) {
				state.type(str);
				return true; },
	    [&](std::string_view str) {
		    state.assign(str);
		    return true;
	    });
}

[[nodiscard]] int NumberInputState::value(int defaultValue) const
{
	return ParseInt<int>(textInput_.value()).value_or(defaultValue);
}

std::string NumberInputState::filterStr(std::string_view str, bool allowMinus)
{
	std::string result;
	if (allowMinus && !str.empty() && str[0] == '-') {
		str.remove_prefix(1);
		result += '-';
	}
	for (const char c : str) {
		if (c >= '0' && c <= '9') {
			result += c;
		}
	}
	return result;
}

void NumberInputState::type(std::string_view str)
{
	const std::string filtered = filterStr(
	    str, /*allowMinus=*/min_ < 0 && textInput_.cursorPosition() == 0);
	if (filtered.empty())
		return;
	textInput_.type(filtered);
	enforceRange();
}

void NumberInputState::assign(std::string_view str)
{
	const std::string filtered = filterStr(str, /*allowMinus=*/min_ < 0);
	if (filtered.empty()) {
		textInput_.clear();
		return;
	}
	textInput_.assign(filtered);
	enforceRange();
}

void NumberInputState::enforceRange()
{
	if (textInput_.empty())
		return;
	ParseIntResult<int> parsed = ParseInt<int>(textInput_.value());
	if (parsed.has_value()) {
		if (*parsed > max_) {
			textInput_.assign(StrCat(max_));
		} else if (*parsed < min_) {
			textInput_.assign(StrCat(min_));
		}
	}
}

bool HandleNumberInputEvent(const SDL_Event &event, NumberInputState &state)
{
	return HandleInputEvent(
	    event, state.textInput(), [&](std::string_view str) {
				state.type(str);
				return true; },
	    [&](std::string_view str) {
		    state.assign(str);
		    return true;
	    });
}

} // namespace devilution
