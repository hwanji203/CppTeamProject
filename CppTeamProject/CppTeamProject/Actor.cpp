#include "Actor.h"
#include<string>

Actor::Actor(Vector2 pos, Color renderColor, std::string renderIcon)
	:m_pos(pos),
	m_renderColor(renderColor),
	m_renderIcon(renderIcon)
{
}
