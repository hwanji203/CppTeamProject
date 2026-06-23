#pragma once
#include "MapGenerator.h"
#include<vector>
#include "Types.h"

class DefaultMapGenerator :
    public MapGenerator
{
public:
    std::unique_ptr<GameMap> Generate(int _width, int _height) override;

public:
    const int GetGroundY() const { return m_groundY; }

private:
    int m_groundY;
    int m_edgeGap;
};