/**
 * This file is part of the MobiusCore project.
 * See AUTHORS file for copyright information.
 */

#ifndef SHADOW_LABYRINTH_H_
#define SHADOW_LABYRINTH_H_

#include "CreatureAIImpl.h"

#define SLScriptName "instance_shadow_labyrinth"
#define DataHeader "SL"

uint32 const EncounterCount = 4;

enum SLDataTypes
{
    // Encounter States/Boss GUIDs
    DATA_AMBASSADOR_HELLMAW             = 0,
    DATA_BLACKHEART_THE_INCITER         = 1,
    DATA_GRANDMASTER_VORPIL             = 2,
    DATA_MURMUR                         = 3,

    // Additional Data
    DATA_FEL_OVERSEER                   = 4
};

enum SLCreatureIds
{
    NPC_AMBASSADOR_HELLMAW              = 18731,
    NPC_GRANDMASTER_VORPIL              = 18732,
    NPC_FEL_OVERSEER                    = 18796
};

enum SLGameObjectIds
{
    GO_REFECTORY_DOOR                   = 183296, // door opened when blackheart the inciter dies
    GO_SCREAMING_HALL_DOOR              = 183295  // door opened when grandmaster vorpil dies
};

enum SLMisc
{
    ACTION_AMBASSADOR_HELLMAW_INTRO     = 1,
    ACTION_AMBASSADOR_HELLMAW_BANISH    = 2,
};

template<typename AI>
inline AI* GetShadowLabyrinthAI(Creature* creature)
{
    return GetInstanceAI<AI>(creature, SLScriptName);
}

#endif // SHADOW_LABYRINTH_H_
