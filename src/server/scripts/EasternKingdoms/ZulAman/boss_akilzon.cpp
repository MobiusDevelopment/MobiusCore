/**
 * This file is part of the MobiusCore project.
 * See AUTHORS file for copyright information.
 */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "zulaman.h"

enum Says
{
    SAY_AGGRO               = 0,
    SAY_PLAYER_KILL         = 1,
    EMOTE_ELECTRICAL_STORM  = 2,
    SAY_SUMMON_EAGLE        = 3,
    SAY_SUMMON_BIRDS        = 4,
    SAY_BERSERK             = 5,
    SAY_DEATH               = 6
};

class boss_akilzon : public CreatureScript
{
    public:
        boss_akilzon() : CreatureScript("boss_akilzon") { }

        struct boss_akilzonAI : public BossAI
        {
            boss_akilzonAI(Creature* creature) : BossAI(creature, DATA_AKILZON) { }

            void Reset() override
            {
                _Reset();
            }

            void EnterCombat(Unit* /*who*/) override
            {
                Talk(SAY_AGGRO);
                _EnterCombat();
            }

            void JustDied(Unit* /*killer*/) override
            {
                Talk(SAY_DEATH);
                _JustDied();
            }

            void KilledUnit(Unit* who) override
            {
                if (who->GetTypeId() == TYPEID_PLAYER)
                    Talk(SAY_PLAYER_KILL);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;
                /*
                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        default:
                            break;
                    }
                }
                */

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetZulAmanAI<boss_akilzonAI>(creature);
        }
};

void AddSC_boss_akilzon()
{
    new boss_akilzon();
}
