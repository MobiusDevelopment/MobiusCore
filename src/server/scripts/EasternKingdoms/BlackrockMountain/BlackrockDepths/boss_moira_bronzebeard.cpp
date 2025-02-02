/**
 * This file is part of the MobiusCore project.
 * See AUTHORS file for copyright information.
 */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"

enum Spells
{
    SPELL_HEAL                                             = 10917,
    SPELL_RENEW                                            = 10929,
    SPELL_SHIELD                                           = 10901,
    SPELL_MINDBLAST                                        = 10947,
    SPELL_SHADOWWORDPAIN                                   = 10894,
    SPELL_SMITE                                            = 10934
};

enum Events
{
    EVENT_MINDBLAST                                        = 1,
    EVENT_SHADOW_WORD_PAIN                                 = 2,
    EVENT_SMITE                                            = 3,
    EVENT_HEAL                                             = 4  // not used atm
};

class boss_moira_bronzebeard : public CreatureScript
{
    public:
        boss_moira_bronzebeard() : CreatureScript("boss_moira_bronzebeard") { }

        struct boss_moira_bronzebeardAI : public ScriptedAI
        {
            boss_moira_bronzebeardAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                _events.Reset();
            }

            void EnterCombat(Unit* /*who*/) override
            {
                //_events.ScheduleEvent(EVENT_HEAL, 12000); // not used atm // These times are probably wrong
                _events.ScheduleEvent(EVENT_MINDBLAST, 16000);
                _events.ScheduleEvent(EVENT_SHADOW_WORD_PAIN, 2000);
                _events.ScheduleEvent(EVENT_SMITE, 8000);
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
                        case EVENT_MINDBLAST:
                            DoCastVictim(SPELL_MINDBLAST);
                            _events.ScheduleEvent(EVENT_MINDBLAST, 14000);
                            break;
                        case EVENT_SHADOW_WORD_PAIN:
                            DoCastVictim(SPELL_SHADOWWORDPAIN);
                            _events.ScheduleEvent(EVENT_SHADOW_WORD_PAIN, 18000);
                            break;
                        case EVENT_SMITE:
                            DoCastVictim(SPELL_SMITE);
                            _events.ScheduleEvent(EVENT_SMITE, 10000);
                            break;
                        default:
                            break;
                    }
                }
            }

        private:
            EventMap _events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_moira_bronzebeardAI(creature);
        }
};

void AddSC_boss_moira_bronzebeard()
{
    new boss_moira_bronzebeard();
}
