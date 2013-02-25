#include "quadtree.h"

#include "entity.h"
#include "debug.h"

using namespace std;

QuadTree::QuadTree(float _x, float _y, float _width, float _height, int _level) :
    m_x(_x),
    m_y(_y),
    m_width(_width),
    m_height(_height),
    m_level(_level)
{
    if (m_level == MAX_LEVELS) {
        Debug::log() << "quadtree max level obtained: " << m_level;
        return;
    }
}

void QuadTree::split()
{
    nodes[0] = new QuadTree(m_x, m_y, m_width / 2.0f, m_height / 2.0f, m_level + 1);
    nodes[1] = new QuadTree(m_x + m_width / 2.0f, m_y, m_width / 2.0f, m_height / 2.0f, m_level + 1);
    nodes[2] = new QuadTree(m_x, m_y + m_height / 2.0f, m_width / 2.0f, m_height / 2.0f, m_level + 1);
    nodes[3] = new QuadTree(m_x + m_width / 2.0f, m_y + m_height / 2.0f, m_width / 2.0f, m_height / 2.0f, m_level + 1);
}

vector<Entity*> QuadTree::entitiesAt(float _x, float _y)
{
    /*
    if (m_level == MAX_LEVELS) {
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
    */
}

void QuadTree::clear()
{
    if (m_level == MAX_LEVELS) {
        m_entities.clear();
        return;
    } else {
        /*
        NW->clear();
        NE->clear();
        SW->clear();
        SE->clear();
        */
    }

    if (!m_entities.empty()) {
        m_entities.clear();
    }
}

/*
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
*/

/*
 * Insert the object into the quadtree. If the node
 * exceeds the capacity, it will split and add all
 * objects to their corresponding nodes.
 */
void QuadTree::insert(Entity* entity) {
    if (nodes[0] != nullptr) {
        int index = getIndex(entity);

        if (index != -1) {
            nodes[index]->insert(entity);

            return;
        }
    }

    m_entities.push_back(entity);

    if (m_entities.size() > MAX_OBJECTS && m_level < MAX_LEVELS) {
        if (nodes[0] == nullptr) {
            split();
        }

        int i = 0;
        while (i < m_entities.size()) {
            int index = getIndex(objects.get(i));
            if (index != -1) {
                nodes[index]->insert(objects.remove(i));
            }
            else {
                i++;
            }
        }
    }
}

/*
 * Determine which node the object belongs to. -1 means
 * object cannot completely fit within a child node and is part
 * of the parent node
 */
int QuadTree::getIndex(Entity* entity) {
    int index = -1;

    const glm::vec2& position = entity->position();
    const glm::vec2& size = entity->size();

    double verticalMidpoint = m_x + (m_width / 2);
    double horizontalMidpoint = m_y + (m_height / 2);

    // Object can completely fit within the top quadrants
    bool topQuadrant = (position.y < horizontalMidpoint && position.y + size.y < horizontalMidpoint);
    // Object can completely fit within the bottom quadrants
    bool bottomQuadrant = (position.y > horizontalMidpoint);

    // Object can completely fit within the left quadrants
    if (position.x < verticalMidpoint && position.x + size.x < verticalMidpoint) {
        if (topQuadrant) {
            index = 1;
        }
        else if (bottomQuadrant) {
            index = 2;
        }
    }
    // Object can completely fit within the right quadrants
    else if (position.x > verticalMidpoint) {
        if (topQuadrant) {
            index = 0;
        }
        else if (bottomQuadrant) {
            index = 3;
        }
    }

    return index;
}

/*
 * Return all objects that could collide with the given object
 */
vector<Entity*> QuadTree::retrieve(vector<Entity*> returnObjects, Entity* entity) {
    int index = getIndex(entity);
    if (index != -1 && nodes[0] != nullptr) {
        nodes[index]->retrieve(returnObjects, entity);
    }

    for (Entity* currentEntity : m_entities) {
        returnObjects.push_back(currentEntity);
    }

    return returnObjects;
}
