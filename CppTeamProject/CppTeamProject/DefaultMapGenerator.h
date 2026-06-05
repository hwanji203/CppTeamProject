#pragma once
#include "MapGenerator.h"
#include<vector>
#include "Types.h"

class BSPGenerator :
    public MapGenerator
{
public:
    std::unique_ptr<GameMap> Generate(int _width, int _height) override;
};

