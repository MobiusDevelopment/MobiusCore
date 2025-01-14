/**
 * This file is part of the MobiusCore project.
 * See AUTHORS file for copyright information.
 */

/* ScriptData
SDName: Boss_Celebras_the_Cursed
SD%Complete: 100
SDComment:
SDCategory: Maraudon
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"

enum Spells
{
    SPELL_WRATH                 = 21807,
    SPELL_ENTANGLINGROOTS       = 12747,
    SPELL_CORRUPT_FORCES        = 21968
};

class celebras_the_cursed : public CreatureScript
{
public:
    celebras_the_cursed() : CreatureScript("celebras_the_cursed") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new celebras_the_cursedAI(creature);
    }

    struct celebras_the_cursedAI : public ScriptedAI
    {
        celebras_the_cursedAI(Creature* creature) : ScriptedAI(creature)
        {
            Initialize();
        }

        void Initialize()
        {
            WrathTimer = 8000;
            EntanglingRootsTimer = 2000;
            CorruptForcesTimer = 30000;
        }

        uint32 WrathTimer;
        uint32 EntanglingRootsTimer;
        uint32 CorruptForcesTimer;

        void Reset() override
        {
            Initialize();
        }

        void EnterCombat(Unit* /*who*/) override { }

        void JustDied(Unit* /*killer*/) override
        {
            me->SummonCreature(13716, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 600000);
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            //Wrath
            if (WrathTimer <= diff)
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                    DoCast(target, SPELL_WRATH);
                WrathTimer = 8000;
            }
            else WrathTimer -= diff;

            //EntanglingRoots
            if (EntanglingRootsTimer <= diff)
            {
                DoCastVictim(SPELL_ENTANGLINGROOTS);
                EntanglingRootsTimer = 20000;
            }
            else EntanglingRootsTimer -= diff;

            //CorruptForces
            if (CorruptForcesTimer <= diff)
            {
                me->InterruptNonMeleeSpells(false);
                DoCast(me, SPELL_CORRUPT_FORCES);
                CorruptForcesTimer = 20000;
            }
            else CorruptForcesTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };
};

void AddSC_boss_celebras_the_cursed()
{
    new celebras_the_cursed();
}
