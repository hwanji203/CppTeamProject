#pragma once

#include<memory>
#include "GameMap.h"
class MapGenerator
{
public:
	virtual ~MapGenerator() = default;
	virtual std::unique_ptr<GameMap> Generate(int _width, int _height) abstract;
};