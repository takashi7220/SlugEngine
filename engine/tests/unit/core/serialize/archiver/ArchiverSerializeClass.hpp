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
        ar.BeginObject();
        ar.Field("id", id);
        ar.EndObject();
    }

    template<typename Archive>
    void Deserialize(Archive& ar)
    {
        ar.BeginObject();
        ar.Field("id", id);
        ar.EndObject();
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
}

template<>
struct std::hash<slug::test::archiver::ItemId>
{
    size_t operator()(const slug::test::archiver::ItemId& key) const noexcept
    {
        return std::hash<int>{}(key.id);
    }
};

namespace slug::test::archiver
{

struct StatusEffect
{
    core::String type;
    int power    = 0;
    int duration = 0;

    template<typename Archive>
    void Serialize(Archive& ar) const
    {
        ar.BeginObject();
        ar.Field("type", type);
        ar.Field("power", power);
        ar.Field("duration", duration);
        ar.EndObject();
    }

    template<typename Archive>
    void Deserialize(Archive& ar)
    {
        ar.BeginObject();
        ar.Field("type", type);
        ar.Field("power", power);
        ar.Field("duration", duration);
        ar.EndObject();
    }

    bool operator==(const StatusEffect& rhs) const
    {
        return type == rhs.type &&
            power == rhs.power &&
            duration == rhs.duration;
    }
};

struct BattleStats
{
    int   maxHp              = 0;
    int   maxMp              = 0;
    float attackMultiplier   = 1.0f;
    float defenseMultiplier  = 1.0f;

    template<typename Archive>
    void Serialize(Archive& ar) const
    {
        ar.BeginObject();
        ar.Field("maxHp", maxHp);
        ar.Field("maxMp", maxMp);
        ar.Field("attackMultiplier", attackMultiplier);
        ar.Field("defenseMultiplier", defenseMultiplier);
        ar.EndObject();
    }

    template<typename Archive>
    void Deserialize(Archive& ar)
    {
        ar.BeginObject();
        ar.Field("maxHp", maxHp);
        ar.Field("maxMp", maxMp);
        ar.Field("attackMultiplier", attackMultiplier);
        ar.Field("defenseMultiplier", defenseMultiplier);
        ar.EndObject();
    }

    bool operator==(const BattleStats& rhs) const
    {
        return maxHp == rhs.maxHp &&
            maxMp == rhs.maxMp &&
            attackMultiplier == rhs.attackMultiplier &&
            defenseMultiplier == rhs.defenseMultiplier;
    }
};

struct Equipment
{
    Weapon               mainWeapon;
    Weapon               subWeapon;
    core::TVector<core::String> accessories;

    template<typename Archive>
    void Serialize(Archive& ar) const
    {
        ar.BeginObject();
        ar.Field("mainWeapon", mainWeapon);
        ar.Field("subWeapon", subWeapon);
        ar.Field("accessories", accessories);
        ar.EndObject();
    }

    template<typename Archive>
    void Deserialize(Archive& ar)
    {
        ar.BeginObject();
        ar.Field("mainWeapon", mainWeapon);
        ar.Field("subWeapon", subWeapon);
        ar.Field("accessories", accessories);
        ar.EndObject();
    }

    bool operator==(const Equipment& rhs) const
    {
        return mainWeapon == rhs.mainWeapon &&
            subWeapon == rhs.subWeapon &&
            accessories == rhs.accessories;
    }
};

struct GameObject
{
    int          objectId = 0;
    core::String tag;

    template<typename Archive>
    void Serialize(Archive& ar) const
    {
        ar.BeginObject();
        ar.Field("objectId", objectId);
        ar.Field("tag", tag);
        ar.EndObject();
    }

    template<typename Archive>
    void Deserialize(Archive& ar)
    {
        ar.BeginObject();
        ar.Field("objectId", objectId);
        ar.Field("tag", tag);
        ar.EndObject();
    }

    bool operator==(const GameObject& rhs) const
    {
        return objectId == rhs.objectId &&
            tag == rhs.tag;
    }
};

struct Actor : GameObject
{
    core::String name;
    BattleStats  stats;

    template<typename Archive>
    void Serialize(Archive& ar) const
    {
        ar.BeginObject();
        ar.BaseClass(core::base_class<GameObject>(*this));
        ar.Field("name", name);
        ar.Field("stats", stats);
        ar.EndObject();
    }

    template<typename Archive>
    void Deserialize(Archive& ar)
    {
        ar.BeginObject();
        ar.BaseClass(core::base_class<GameObject>(*this));
        ar.Field("name", name);
        ar.Field("stats", stats);
        ar.EndObject();
    }

    bool operator==(const Actor& rhs) const
    {
        return static_cast<const GameObject&>(*this) == static_cast<const GameObject&>(rhs) &&
            name == rhs.name &&
            stats == rhs.stats;
    }
};

struct Hero : Actor
{
    int                                              level = 0;
    Equipment                                        equipment;
    core::TVector<StatusEffect>                      activeEffects;
    core::TUnorderedMap<core::String, int>           attributes;
    core::TUnorderedMap<ItemId, int, std::hash<ItemId>> inventory;

    template<typename Archive>
    void Serialize(Archive& ar) const
    {
        ar.BeginObject();
        ar.BaseClass(core::base_class<Actor>(*this));
        ar.Field("level", level);
        ar.Field("equipment", equipment);
        ar.Field("activeEffects", activeEffects);
        ar.Field("attributes", attributes);
        ar.Field("inventory", inventory);
        ar.EndObject();
    }

    template<typename Archive>
    void Deserialize(Archive& ar)
    {
        ar.BeginObject();
        ar.BaseClass(core::base_class<Actor>(*this));
        ar.Field("level", level);
        ar.Field("equipment", equipment);
        ar.Field("activeEffects", activeEffects);
        ar.Field("attributes", attributes);
        ar.Field("inventory", inventory);
        ar.EndObject();
    }

    bool operator==(const Hero& rhs) const
    {
        return static_cast<const Actor&>(*this) == static_cast<const Actor&>(rhs) &&
            level == rhs.level &&
            equipment == rhs.equipment &&
            activeEffects == rhs.activeEffects &&
            attributes == rhs.attributes &&
            inventory == rhs.inventory;
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
