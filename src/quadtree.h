#ifndef __QUADTREE_H__
#define __QUADTREE_H__

#include <vector>

using namespace std;

class Quadtree;
class Entity;

class Quadtree
{
public:
    Quadtree(float x, float y, float width, float height, int level, int maxLevel);

    void addEntity(Entity* object);
    vector<Entity*> entitiesAt(float x, float y);
    void clear();

private:
    float m_x;
    float m_y;
    float m_width;
    float m_height;
    int m_level;
    int m_maxLevel;
    vector<Entity*> m_entities;

    Quadtree* parent;
    Quadtree* NW;
    Quadtree* NE;
    Quadtree* SW;
    Quadtree* SE;

    bool contains(Quadtree* child, Entity* object);
};

#endif
