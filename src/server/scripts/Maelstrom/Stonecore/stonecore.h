/**
 * This file is part of the MobiusCore project.
 * See AUTHORS file for copyright information.
 */

#ifndef DEF_STONECORE_H
#define DEF_STONECORE_H

#include "CreatureAIImpl.h"

#define SCScriptName "instance_stonecore"
#define DataHeader "SC"

enum SCDataTypes
{
    // Encounter States/Boss GUIDs
    DATA_CORBORUS,
    DATA_SLABHIDE,
    DATA_OZRUK,
    DATA_HIGH_PRIESTESS_AZIL,

    // Additional Data
    DATA_MILLHOUSE_MANASTORM,
    DATA_MILLHOUSE_EVENT_FACE,
    DATA_HANDLE_CORBORUS_ROCKDOOR,
    DATA_MILLHOUSE_EVENT_KNOCKBACK,
    DATA_MILLHOUSE_EVENT_DESPAWN,

    DATA_SLABHIDE_INTRO,
    DATA_SLABHIDE_ROCK_WALL,

    // Teleporters
    DATA_STONECORE_TELEPORTER,
    DATA_STONECORE_TELEPORTER_2,
};

enum SCMisc
{
    ACTION_CORBORUS_INTRO,
    ACTION_SLABHIDE_INTRO,

    NPC_WORLDTRIGGER = 22515,
    NPC_MILLHOUSE_MANASTORM = 43391,

    NPC_CORBORUS = 43438,
    NPC_SLABHIDE = 43214,
    NPC_OZRUK = 42188,
    NPC_HIGH_PRIESTESS_AZIL = 42333,

    // Stonecore Teleporter misc
    MAX_STONECORE_TELEPORTERS = 2,
    NPC_STONECORE_TELEPORTER = 51396, // Entrance teleporter
    NPC_STONECORE_TELEPORTER_2 = 51397, // Slabhide teleporter
    SPELL_TELEPORTER_ACTIVE_VISUAL = 95298,

    GAMEOBJECT_TWILIGHT_DOCUMENTS = 207415,
    GAMEOBJECT_CORBORUS_ROCKDOOR = 207343,
    GAMEOBJECT_SLABHIDE_ROCK_WALL = 204381,

    SPELL_RING_WYRM_KNOCKBACK = 81235,

    // Creature Formation IDs
    CREATURE_FORMATION_MILLHOUSE_EVENT_TRASH      = 340418,
    CREATURE_FORMATION_MILLHOUSE_EVENT_LAST_GROUP = 340492,
};

template<typename AI>
inline AI* GetStonecoreAI(Creature* creature)
{
    return GetInstanceAI<AI>(creature, SCScriptName);
}

#endif // DEF_STONECORE
