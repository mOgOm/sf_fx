#include "ui_label.h"
#include "../fonts_provider.h"
#include "../extentions.h"

namespace qw
{
UiLabel::UiLabel(sf::String text, char const* font, unsigned char_size)
	:_str(text)
	,_text(text, FontsProvider::GetFont(font), char_size)
{
	_RecalculateCharSize();
}


IUiItem* UiLabel::SetPosition(sf::Vector2f const& position)
{
	_text.setPosition(position);
	return this;
}


sf::Vector2f UiLabel::GetPosition()
{
	return _text.getPosition();
}


IUiItem* UiLabel::SetSize(sf::Vector2f const& size)
{
	_width =  floor(size.x / _charWidth);
	_height = floor(size.y / _charHeight);
	if (_width == 0) _width = 1;
	if (_height == 0) _height = 1;
	
	_SetSize(_width, _height);
	return this;
}


sf::Vector2f UiLabel::GetSize()
{
	return sf::Vector2f(_charWidth * _width, _charHeight * _height);
}


IUiItem* UiLabel::SetRotation(float angle)
{
	_text.setRotation(angle * QW_RAD_TO_DEG);
	return this;
}


float UiLabel::GetRotation()
{
	return _text.getRotation() * QW_DEG_TO_RAD;
}


std::vector<IUiItem*>* UiLabel::GetChilds()
{
	return nullptr;
}


UiLabel* UiLabel::Scroll(int step)
{
	if (_scrollable) _scrollPos = (0 > step && -step > _scrollPos) ? 0 : _scrollPos + step;
	return this;
}


IUiItem* UiLabel::AddChild(IUiItem* new_ui_item)
{
	return this;
}


void UiLabel::Draw()
{
	_pw->draw(_text);
}


bool tryClipByHeight(sf::String& tmp, size_t& height, size_t idx, size_t& scroll, bool& scrolling, bool _scrollable)
{
	bool needToBreak{ false };
	--height;
	if (scrolling)
	{
		if (scroll != 0)
		{
			--scroll;
			if (scroll == 0)
			{
				scroll = idx + 1;
				scrolling = false;
			}
		}
		else scrolling = false;
	}
	if (height == 0)
	{
		tmp = _scrollable ? tmp.substring(scroll, idx - scroll) : tmp.substring(0, idx);
		needToBreak = true;
	}
	return needToBreak;
}


void UiLabel::_SetSize(size_t width, size_t height)
{
	sf::String tmp{ _str + '\n' };
	bool scrolling{ _scrollable };
	size_t scroll{ _scrollPos };
	size_t last_space{ width };
	height += _scrollPos;

	if (width > 1)
	{
		for (size_t idx{ 0 }, count{ 0 }; tmp.getSize() > idx; ++idx)
		{
			auto ch = tmp[idx];
			if (ch == ' ' || ch == '\t' || ch == '\n') last_space = idx + 1;
			if (ch == '\n') count = 0;
			if (count >= width)
			{
				count = 0;
				tmp.insert(last_space, '\n');
				idx = last_space;
				last_space += width + 1;
			}
			else
			{
				++count;
			}

			if (tmp[idx] == '\n' && tryClipByHeight(tmp, height, idx, scroll, scrolling, _scrollable))
			{
				break;
			}
		}
	}
	else
	{
		for (size_t idx = 1; tmp.getSize() > idx; idx += 2)
		{
			tmp.insert(idx, '\n');
			if (tryClipByHeight(tmp, height, idx, scroll, scrolling, _scrollable))
			{
				break;
			}
		}
	}

	_text.setString(tmp);
}


void UiLabel::_RecalculateCharSize()
{
	sf::Text tmp("ab", *_text.getFont(), _text.getCharacterSize());
	_charWidth = tmp.findCharacterPos(1).x - tmp.findCharacterPos(0).x;
	tmp.setString("a\nb");
	_charHeight = tmp.findCharacterPos(2).y - tmp.findCharacterPos(0).y;
}
}
