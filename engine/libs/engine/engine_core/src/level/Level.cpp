#include "engine_core/level/Level.hpp"
#include "engine_core/world/IWorld.hpp"
namespace slug::engine_core
{
Level::Level()
{

}

void Level::SetWorld(IWorld* world)
{
    m_world = world;
}

IWorld* Level::GetWorld()
{
    return m_world;
}

void Level::SetParentLevel(Level* parent)
{
    if (m_parent != nullptr)
    {
        m_parent->DeleteChildLevel(this->GetUUID());
    }

    if (parent != nullptr)
    {
        parent->AddChildLevel(this);
    }

    m_parent = parent;
}

Level* Level::GetParentLevel()
{
    return m_parent;
}

void Level::AddChildLevel(Level* level)
{
    if (level != nullptr && !HasChildLevel(level->GetUUID()))
    {
        m_childLevels.push_back(level);
        level->SetParentLevel(this);
    }
}

void Level::DeleteChildLevel(core::UUID uuid)
{
    auto itr = m_childLevels.begin();
    while (itr != m_childLevels.end())
    {
        if (itr->get()->GetUUID() == uuid)
        {
            itr = m_childLevels.erase(itr);
        } else
        {
            itr++;
        }
    }
}

bool Level::HasChildLevel(core::UUID uuid)
{
    for (auto& child : m_childLevels)
    {
        if (child != nullptr &&  child->GetUUID() == uuid)
        {
            return true;
        }
    }
    return false;
}

LevelPtr Level::GetChild(core::UUID uuid)
{
    for (auto& child : m_childLevels)
    {
        if (child != nullptr && child->GetUUID() == uuid)
        {
            return child;
        }
    }
    return nullptr;
}

void Level::GetChildren(core::TVector<Level*>& children, bool recursive)
{
    for (auto& child : m_childLevels)
    {
        if (child != nullptr)
        {
            children.push_back(child);
            if (recursive)
            {
                child->GetChildren(children, recursive);
            }
        }
    }
}


void Level::AddActor(Actor* actor)
{
    if (actor != nullptr && !HasActor(actor->GetUUID()))
    {
        m_actors.push_back(actor);
        actor->SetLevel(this);
    }
}

void Level::DeleteActor(core::UUID uuid)
{
    auto itr = m_actors.begin();
    while (itr != m_actors.end())
    {
        if (itr->get()->GetUUID() == uuid)
        {
            itr = m_actors.erase(itr);
        } else
        {
            itr++;
        }
    }
}

bool Level::HasActor(core::UUID uuid)
{
    for (auto& actor : m_actors)
    {
        if (actor->GetUUID() == uuid)
        {
            return true;
        }
    }
    return false;
}

ActorPtr Level::GetActor(core::UUID uuid)
{
    for (auto& actor : m_actors)
    {
        if (actor->GetUUID() == uuid)
        {
            return actor;
        }
    }
    return nullptr;
}

void Level::GetActors(core::TVector<Actor*>& actors)
{
    for (auto& actor : m_actors)
    {
        actors.push_back(actor);
    }
}

void Level::EnumerateActors(core::TVector<Actor*>& objects)
{
    for (auto itr : m_actors)
    {
        objects.push_back(itr);
        itr->GetChildren(objects, true);
    }

    for (auto itr : m_childLevels)
    {
        itr->EnumerateActors(objects);
    }
}

void Level::EnumerateLevels(core::TVector<Level*>& levels)
{
    GetChildren(levels, true);
}

}
