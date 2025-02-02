/**
 * This file is part of the MobiusCore project.
 * See AUTHORS file for copyright information.
 */

#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "naxxramas.h"
#include "ScriptedCreature.h"

enum Spells
{
    SPELL_HATEFUL_STRIKE                        = 41926,
    SPELL_FRENZY                                = 28131,
    SPELL_BERSERK                               = 26662,
    SPELL_SLIME_BOLT                            = 32309
};

enum Yells
{
    SAY_AGGRO                                   = 0,
    SAY_SLAY                                    = 1,
    SAY_DEATH                                   = 2,
    EMOTE_BERSERK                               = 3,
    EMOTE_ENRAGE                                = 4
};

enum Events
{
    EVENT_NONE,
    EVENT_BERSERK,
    EVENT_HATEFUL,
    EVENT_SLIME
};

enum Misc
{
    ACHIEV_MAKE_QUICK_WERK_OF_HIM_STARTING_EVENT  = 10286
};

enum HatefulThreatAmounts
{
    HATEFUL_THREAT_AMT  = 1000,
};

class boss_patchwerk : public CreatureScript
{
public:
    boss_patchwerk() : CreatureScript("boss_patchwerk") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return GetNaxxramasAI<boss_patchwerkAI>(creature);
    }

    struct boss_patchwerkAI : public BossAI
    {
        boss_patchwerkAI(Creature* creature) : BossAI(creature, BOSS_PATCHWERK)
        {
            Enraged = false;
        }

        bool Enraged;

        void Reset() override
        {
            _Reset();

            instance->DoStopCriteriaTimer(CRITERIA_TIMED_TYPE_EVENT, ACHIEV_MAKE_QUICK_WERK_OF_HIM_STARTING_EVENT);
        }

        void KilledUnit(Unit* /*Victim*/) override
        {
            if (!(rand32() % 5))
                Talk(SAY_SLAY);
        }

        void JustDied(Unit* /*killer*/) override
        {
            _JustDied();
            Talk(SAY_DEATH);
        }

        void EnterCombat(Unit* /*who*/) override
        {
            _EnterCombat();
            Enraged = false;
            Talk(SAY_AGGRO);
            events.ScheduleEvent(EVENT_HATEFUL, Seconds(1));
            events.ScheduleEvent(EVENT_BERSERK, Minutes(6));

            instance->DoStartCriteriaTimer(CRITERIA_TIMED_TYPE_EVENT, ACHIEV_MAKE_QUICK_WERK_OF_HIM_STARTING_EVENT);
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_HATEFUL:
                    {
                        // Hateful Strike targets the highest non-MT threat in melee range on 10man
                        // and the higher HP target out of the two highest non-MT threats in melee range on 25man
                        float MostThreat = 0.0f;
                        Unit* secondThreatTarget = NULL;
                        Unit* thirdThreatTarget = NULL;

                        std::list<HostileReference*>::const_iterator i = me->getThreatManager().getThreatList().begin();
                        for (; i != me->getThreatManager().getThreatList().end(); ++i)
                        { // find second highest
                            Unit* target = (*i)->getTarget();
                            if (target->IsAlive() && target != me->GetVictim() && (*i)->getThreat() >= MostThreat && me->IsWithinMeleeRange(target))
                            {
                                MostThreat = (*i)->getThreat();
                                secondThreatTarget = target;
                            }
                        }

                        if (secondThreatTarget && Is25ManRaid())
                        { // find third highest
                            MostThreat = 0.0f;
                            i = me->getThreatManager().getThreatList().begin();
                            for (; i != me->getThreatManager().getThreatList().end(); ++i)
                            {
                                Unit* target = (*i)->getTarget();
                                if (target->IsAlive() && target != me->GetVictim() && target != secondThreatTarget && (*i)->getThreat() >= MostThreat && me->IsWithinMeleeRange(target))
                                {
                                    MostThreat = (*i)->getThreat();
                                    thirdThreatTarget = target;
                                }
                            }
                        }

                        Unit* pHatefulTarget = NULL;
                        if (!thirdThreatTarget)
                            pHatefulTarget = secondThreatTarget;
                        else if (secondThreatTarget)
                            pHatefulTarget = (secondThreatTarget->GetHealth() < thirdThreatTarget->GetHealth()) ? thirdThreatTarget : secondThreatTarget;

                        if (!pHatefulTarget)
                            pHatefulTarget = me->GetVictim();

                        DoCast(pHatefulTarget, SPELL_HATEFUL_STRIKE, true);

                        // add threat to highest threat targets
                        if (me->GetVictim() && me->IsWithinMeleeRange(me->GetVictim()))
                            me->getThreatManager().addThreat(me->GetVictim(), HATEFUL_THREAT_AMT);
                        if (secondThreatTarget)
                            me->getThreatManager().addThreat(secondThreatTarget, HATEFUL_THREAT_AMT);
                        if (thirdThreatTarget)
                            me->getThreatManager().addThreat(thirdThreatTarget, HATEFUL_THREAT_AMT); // this will only ever be used in 25m

                        events.Repeat(Seconds(1));
                        break;
                    }
                    case EVENT_BERSERK:
                        DoCast(me, SPELL_BERSERK, true);
                        Talk(EMOTE_BERSERK);
                        events.ScheduleEvent(EVENT_SLIME, Seconds(2));
                        break;
                    case EVENT_SLIME:
                        DoCastAOE(SPELL_SLIME_BOLT, true);
                        events.Repeat(Seconds(2));
                        break;
                }
            }

            if (!Enraged && HealthBelowPct(5))
            {
                DoCast(me, SPELL_FRENZY, true);
                Talk(EMOTE_ENRAGE);
                Enraged = true;
            }

            DoMeleeAttackIfReady();
        }
    };

};

void AddSC_boss_patchwerk()
{
    new boss_patchwerk();
}
