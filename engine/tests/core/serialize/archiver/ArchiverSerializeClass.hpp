#pragma once

#include "core/serialize/archiver/ArchiverSerializer.hpp"
#include "core/container/String.hpp"
#include "core/container/Vector.hpp"
#include "core/container/UnorderedMap.hpp"

namespace slug::test::archiver
{
struct SimpleData
{
    int id = 0;
    float value = 0.0f;
    core::String name;

    template<typename Archive>
    void Serialize(Archive& ar) const
    {
        ar.BeginObject();
        ar.Field("id", id);
        ar.Field("value", value);
        ar.Field("name", name);
        ar.EndObject();
    }

    template<typename Archive>
    void Deserialize(Archive& ar)
    {
        ar.BeginObject();
        ar.Field("id", id);
        ar.Field("value", value);
        ar.Field("name", name);
        ar.EndObject();
    }

    bool operator==(const SimpleData& rhs) const
    {
        return id == rhs.id &&
            value == rhs.value &&
            name == rhs.name;
    }
};

struct Inventory
{
    core::TVector<int> itemIds;
    core::String owner;

    template<typename Archive>
    void Serialize(Archive& ar) const
    {
        ar.BeginObject();
        ar.Field("itemIds", itemIds);
        ar.Field("owner", owner);
        ar.EndObject();
    }

    template<typename Archive>
    void Deserialize(Archive& ar)
    {
        ar.BeginObject();
        ar.Field("itemIds", itemIds);
        ar.Field("owner", owner);
        ar.EndObject();
    }

    bool operator==(const Inventory& rhs) const
    {
        return itemIds == rhs.itemIds &&
            owner == rhs.owner;
    }
};

struct Weapon
{
    core::String name;
    int attack = 0;

    template<typename Archive>
    void Serialize(Archive& ar) const
    {
        ar.BeginObject();
        ar.Field("name", name);
        ar.Field("attack", attack);
        ar.EndObject();
    }

    template<typename Archive>
    void Deserialize(Archive& ar)
    {
        ar.BeginObject();
        ar.Field("name", name);
        ar.Field("attack", attack);
        ar.EndObject();
    }

    bool operator==(const Weapon& rhs) const
    {
        return name == rhs.name &&
            attack == rhs.attack;
    }
};

struct Player
{
    int id = 0;
    core::String name;
    float hp = 0.0f;
    core::TVector<int> inventoryIds;
    Weapon weapon;

    template<typename Archive>
    void Serialize(Archive& ar) const
    {
        ar.BeginObject();
        ar.Field("id", id);
        ar.Field("name", name);
        ar.Field("hp", hp);
        ar.Field("inventoryIds", inventoryIds);
        ar.Field("weapon", weapon);
        ar.EndObject();
    }

    template<typename Archive>
    void Deserialize(Archive& ar)
    {
        ar.BeginObject();
        ar.Field("id", id);
        ar.Field("name", name);
        ar.Field("hp", hp);
        ar.Field("inventoryIds", inventoryIds);
        ar.Field("weapon", weapon);
        ar.EndObject();
    }

    bool operator==(const Player& rhs) const
    {
        return id == rhs.id &&
            name == rhs.name &&
            hp == rhs.hp &&
            inventoryIds == rhs.inventoryIds &&
            weapon == rhs.weapon;
    }
};

struct Entity
{
    int entityId = 0;

    template<typename Archive>
    void Serialize(Archive& ar) const
    {
        ar.BeginObject();
        ar.Field("entityId", entityId);
        ar.EndObject();
    }

    template<typename Archive>
    void Deserialize(Archive& ar)
    {
        ar.BeginObject();
        ar.Field("entityId", entityId);
        ar.EndObject();
    }

    bool operator==(const Entity& rhs) const
    {
        return entityId == rhs.entityId;
    }
};

struct ItemId
{
    int id = 0;

    template<typename Archive>
    void Serialize(Archive& ar) const
    {
        ar.Field("id", id);
    }

    template<typename Archive>
    void Deserialize(Archive& ar)
    {
        ar.Field("id", id);
    }

    bool operator==(const ItemId& rhs) const
    {
        return id == rhs.id;
    }
};

struct ItemTable
{
    core::TUnorderedMap<int, core::String> items;

    template<typename Archive>
    void Serialize(Archive& ar) const
    {
        ar.BeginObject();
        ar.Field("items", items);
        ar.EndObject();
    }

    template<typename Archive>
    void Deserialize(Archive& ar)
    {
        ar.BeginObject();
        ar.Field("items", items);
        ar.EndObject();
    }

    bool operator==(const ItemTable& rhs) const
    {
        return items == rhs.items;
    }
};

struct Character : Entity
{
    core::String name;
    int level = 0;
    core::TVector<int> skillIds;

    template<typename Archive>
    void Serialize(Archive& ar) const
    {
        ar.BeginObject();
        ar.BaseClass(core::base_class<Entity>(*this));
        ar.Field("name", name);
        ar.Field("level", level);
        ar.Field("skillIds", skillIds);
        ar.EndObject();
    }

    template<typename Archive>
    void Deserialize(Archive& ar)
    {
        ar.BeginObject();
        ar.BaseClass(core::base_class<Entity>(*this));
        ar.Field("name", name);
        ar.Field("level", level);
        ar.Field("skillIds", skillIds);
        ar.EndObject();
    }

    bool operator==(const Character& rhs) const
    {
        return static_cast<const Entity&>(*this) == static_cast<const Entity&>(rhs) &&
            name == rhs.name &&
            level == rhs.level &&
            skillIds == rhs.skillIds;
    }
};
}

template<>
struct std::hash<slug::test::archiver::ItemId>
{
    size_t operator()(const slug::test::archiver::ItemId& key) const noexcept
    {
        return std::hash<int>{}(key.id);
    }
};
