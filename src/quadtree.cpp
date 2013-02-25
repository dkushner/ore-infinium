#include "quadtree.h"

#include "entity.h"
#include "debug.h"

using namespace std;

Quadtree::Quadtree(float _x, float _y, float _width, float _height, int _level, int _maxLevel) :
    m_x(_x),
    m_y(_y),
    m_width(_width),
    m_height(_height),
    m_level(_level),
    m_maxLevel(_maxLevel)
{
    if (m_level == m_maxLevel) {
        Debug::log() << "quadtree max level obtained: " << m_maxLevel;
        return;
    }

    NW = new Quadtree(m_x, m_y, m_width / 2.0f, m_height / 2.0f, m_level + 1, m_maxLevel);
    NE = new Quadtree(m_x + m_width / 2.0f, m_y, m_width / 2.0f, m_height / 2.0f, m_level + 1, m_maxLevel);
    SW = new Quadtree(m_x, m_y + m_height / 2.0f, m_width / 2.0f, m_height / 2.0f, m_level + 1, m_maxLevel);
    SE = new Quadtree(m_x + m_width / 2.0f, m_y + m_height / 2.0f, m_width / 2.0f, m_height / 2.0f, m_level + 1, m_maxLevel);
}

void Quadtree::addEntity(Entity *entity)
{
    if (m_level == m_maxLevel) {
        m_entities.push_back(entity);
        return;
    }

    if (contains(NW, entity)) {
        NW->addEntity(entity);
        return;
    } else if (contains(NE, entity)) {
        NE->addEntity(entity);
        return;
    } else if (contains(SW, entity)) {
        SW->addEntity(entity);
        return;
    } else if (contains(SE, entity)) {
        SE->addEntity(entity);
        return;
    }

    if (contains(this, entity)) {
        m_entities.push_back(entity);
    }
}

vector<Entity*> Quadtree::entitiesAt(float _x, float _y)
{
    if (m_level == m_maxLevel) {
        return m_entities;
    }

    vector<Entity*> returnObjects, childReturnObjects;
    if (!m_entities.empty()) {
        returnObjects = m_entities;
    }

    if (_x > m_x + m_width / 2.0f && _x < m_x + m_width) {
        if (_y > m_y + m_height / 2.0f && _y < m_y + m_height) {
            childReturnObjects = SE->entitiesAt(_x, _y);
            returnObjects.insert(returnObjects.end(), childReturnObjects.begin(), childReturnObjects.end());
            return returnObjects;
        } else if (_y > m_y && _y <= m_y + m_height / 2.0f) {
            childReturnObjects = NE->entitiesAt(_x, _y);
            returnObjects.insert(returnObjects.end(), childReturnObjects.begin(), childReturnObjects.end());
            return returnObjects;
        }
    } else if (_x > m_x && _x <= m_x + m_width / 2.0f) {
        if (_y > m_y + m_height / 2.0f && _y < m_y + m_height) {
            childReturnObjects = SW->entitiesAt(_x, _y);
            returnObjects.insert(returnObjects.end(), childReturnObjects.begin(), childReturnObjects.end());
            return returnObjects;
        } else if (_y > m_y && _y <= m_y + m_height / 2.0f) {
            childReturnObjects = NW->entitiesAt(_x, _y);
            returnObjects.insert(returnObjects.end(), childReturnObjects.begin(), childReturnObjects.end());
            return returnObjects;
        }
    }

    return returnObjects;
}

void Quadtree::clear()
{
    if (m_level == m_maxLevel) {
        m_entities.clear();
        return;
    } else {
        NW->clear();
        NE->clear();
        SW->clear();
        SE->clear();
    }

    if (!m_entities.empty()) {
        m_entities.clear();
    }
}

bool Quadtree::contains(Quadtree *child, Entity *entity)
{
    const glm::vec2& position = entity->position();
    const glm::vec2& size = entity->size();

    return   !(position.x < child->m_x ||
               position.y < child->m_y ||
               position.x > child->m_x + child->m_width  ||
               position.y > child->m_y + child->m_height ||
               position.x + size.x < child->m_x ||
               position.y + size.y < child->m_y ||
               position.x + size.x > child->m_x + child->m_width ||
               position.y + size.y > child->m_y + child->m_height);
}
