/**
 * This file is part of the MobiusCore project.
 * See AUTHORS file for copyright information.
 */

#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "ScriptedCreature.h"
#include "violet_hold.h"

enum Spells
{
    SPELL_CAUTERIZING_FLAMES                    = 59466, // Only in heroic
    SPELL_FIREBOLT                              = 54235,
    SPELL_FLAME_BREATH                          = 54282,
    SPELL_LAVA_BURN                             = 54249
};

class boss_lavanthor : public CreatureScript
{
    public:
        boss_lavanthor() : CreatureScript("boss_lavanthor") { }

        struct boss_lavanthorAI : public BossAI
        {
            boss_lavanthorAI(Creature* creature) : BossAI(creature, DATA_LAVANTHOR) { }

            void JustReachedHome() override
            {
                BossAI::JustReachedHome();
                instance->SetData(DATA_HANDLE_CELLS, DATA_LAVANTHOR);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                scheduler.Update(diff,
                    std::bind(&BossAI::DoMeleeAttackIfReady, this));
            }

            void ScheduleTasks() override
            {
                scheduler.Schedule(Seconds(1), [this](TaskContext task)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 40.0f, true))
                        DoCast(target, SPELL_FIREBOLT);
                    task.Repeat(Seconds(5), Seconds(13));
                });

                scheduler.Schedule(Seconds(5), [this](TaskContext task)
                {
                    DoCastVictim(SPELL_FLAME_BREATH);
                    task.Repeat(Seconds(10), Seconds(15));
                });

                scheduler.Schedule(Seconds(10), [this](TaskContext task)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.0f))
                        DoCast(target, SPELL_LAVA_BURN);
                    task.Repeat(Seconds(15), Seconds(23));
                });

                if (IsHeroic())
                {
                    scheduler.Schedule(Seconds(3), [this](TaskContext task)
                    {
                        DoCastAOE(SPELL_CAUTERIZING_FLAMES);
                        task.Repeat(Seconds(10), Seconds(16));
                    });
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetVioletHoldAI<boss_lavanthorAI>(creature);
        }
};

void AddSC_boss_lavanthor()
{
    new boss_lavanthor();
}
