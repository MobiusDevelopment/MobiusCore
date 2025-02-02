/**
 * This file is part of the MobiusCore project.
 * See AUTHORS file for copyright information.
 */

#include "ScriptMgr.h"
#include "blackrock_depths.h"
#include "InstanceScript.h"
#include "ObjectAccessor.h"
#include "ScriptedCreature.h"

enum Yells
{
    SAY_AGGRO                                              = 0,
    SAY_SLAY                                               = 1
};

enum Spells
{
    SPELL_HANDOFTHAURISSAN                                 = 17492,
    SPELL_AVATAROFFLAME                                    = 15636
};

enum Events
{
    EVENT_HANDOFTHAURISSAN                                 = 1,
    EVENT_AVATAROFFLAME                                    = 2
};

class boss_emperor_dagran_thaurissan : public CreatureScript
{
    public:
        boss_emperor_dagran_thaurissan() : CreatureScript("boss_emperor_dagran_thaurissan") { }

        struct boss_draganthaurissanAI : public ScriptedAI
        {
            boss_draganthaurissanAI(Creature* creature) : ScriptedAI(creature)
            {
                _instance = me->GetInstanceScript();
            }

            void Reset() override
            {
                _events.Reset();
            }

            void EnterCombat(Unit* /*who*/) override
            {
                Talk(SAY_AGGRO);
                me->CallForHelp(VISIBLE_RANGE);
                _events.ScheduleEvent(EVENT_HANDOFTHAURISSAN, 4000);
                _events.ScheduleEvent(EVENT_AVATAROFFLAME, 25000);
            }

            void KilledUnit(Unit* who) override
            {
                if (who->GetTypeId() == TYPEID_PLAYER)
                    Talk(SAY_SLAY);
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (Creature* moira = ObjectAccessor::GetCreature(*me, _instance->GetGuidData(DATA_MOIRA)))
                {
                    moira->AI()->EnterEvadeMode();
                    moira->setFaction(35);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                _events.Update(diff);

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_HANDOFTHAURISSAN:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                                DoCast(target, SPELL_HANDOFTHAURISSAN);
                            _events.ScheduleEvent(EVENT_HANDOFTHAURISSAN, 5000);
                            break;
                        case EVENT_AVATAROFFLAME:
                            DoCastVictim(SPELL_AVATAROFFLAME);
                            _events.ScheduleEvent(EVENT_AVATAROFFLAME, 18000);
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            InstanceScript* _instance;
            EventMap _events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetBlackrockDepthsAI<boss_draganthaurissanAI>(creature);
        }
};

void AddSC_boss_draganthaurissan()
{
    new boss_emperor_dagran_thaurissan();
}
