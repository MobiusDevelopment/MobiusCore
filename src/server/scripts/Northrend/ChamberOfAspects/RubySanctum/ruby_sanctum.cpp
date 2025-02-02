/**
 * This file is part of the MobiusCore project.
 * See AUTHORS file for copyright information.
 */

#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "MotionMaster.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "ruby_sanctum.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "SpellScript.h"

enum Texts
{
    SAY_XERESTRASZA_EVENT       = 0,
    SAY_XERESTRASZA_EVENT_1     = 1,
    SAY_XERESTRASZA_EVENT_2     = 2,
    SAY_XERESTRASZA_EVENT_3     = 3,
    SAY_XERESTRASZA_EVENT_4     = 4,
    SAY_XERESTRASZA_EVENT_5     = 5,
    SAY_XERESTRASZA_EVENT_6     = 6,
    SAY_XERESTRASZA_EVENT_7     = 7,
    SAY_XERESTRASZA_INTRO       = 8,
};

enum Events
{
    EVENT_XERESTRASZA_EVENT_1   = 1,
    EVENT_XERESTRASZA_EVENT_2   = 2,
    EVENT_XERESTRASZA_EVENT_3   = 3,
    EVENT_XERESTRASZA_EVENT_4   = 4,
    EVENT_XERESTRASZA_EVENT_5   = 5,
    EVENT_XERESTRASZA_EVENT_6   = 6,
    EVENT_XERESTRASZA_EVENT_7   = 7,
};

enum Spells
{
    SPELL_RALLY                 = 75416
};

Position const xerestraszaMovePos = {3151.236f, 379.8733f, 86.31996f, 0.0f};

class npc_xerestrasza : public CreatureScript
{
    public:
        npc_xerestrasza() : CreatureScript("npc_xerestrasza") { }

        struct npc_xerestraszaAI : public ScriptedAI
        {
            npc_xerestraszaAI(Creature* creature) : ScriptedAI(creature)
            {
                _isIntro = true;
                _introDone = false;
            }

            void Reset() override
            {
                _events.Reset();
                me->RemoveFlag(UNIT_NPC_FLAGS, GOSSIP_OPTION_QUESTGIVER);
            }

            void DoAction(int32 action) override
            {
                if (action == ACTION_BALTHARUS_DEATH)
                {
                    me->setActive(true);
                    _isIntro = false;

                    Talk(SAY_XERESTRASZA_EVENT);
                    me->SetWalk(true);
                    me->GetMotionMaster()->MovePoint(0, xerestraszaMovePos);

                    _events.ScheduleEvent(EVENT_XERESTRASZA_EVENT_1, 16000);
                    _events.ScheduleEvent(EVENT_XERESTRASZA_EVENT_2, 25000);
                    _events.ScheduleEvent(EVENT_XERESTRASZA_EVENT_3, 32000);
                    _events.ScheduleEvent(EVENT_XERESTRASZA_EVENT_4, 42000);
                    _events.ScheduleEvent(EVENT_XERESTRASZA_EVENT_5, 51000);
                    _events.ScheduleEvent(EVENT_XERESTRASZA_EVENT_6, 61000);
                    _events.ScheduleEvent(EVENT_XERESTRASZA_EVENT_7, 69000);
                }
                else if (action == ACTION_INTRO_BALTHARUS && !_introDone)
                {
                    _introDone = true;
                    Talk(SAY_XERESTRASZA_INTRO);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (_isIntro)
                    return;

                _events.Update(diff);

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_XERESTRASZA_EVENT_1:
                            Talk(SAY_XERESTRASZA_EVENT_1);
                            break;
                        case EVENT_XERESTRASZA_EVENT_2:
                            Talk(SAY_XERESTRASZA_EVENT_2);
                            break;
                        case EVENT_XERESTRASZA_EVENT_3:
                            Talk(SAY_XERESTRASZA_EVENT_3);
                            break;
                        case EVENT_XERESTRASZA_EVENT_4:
                            Talk(SAY_XERESTRASZA_EVENT_4);
                            break;
                        case EVENT_XERESTRASZA_EVENT_5:
                            Talk(SAY_XERESTRASZA_EVENT_5);
                            break;
                        case EVENT_XERESTRASZA_EVENT_6:
                            Talk(SAY_XERESTRASZA_EVENT_6);
                            break;
                        case EVENT_XERESTRASZA_EVENT_7:
                            me->SetFlag(UNIT_NPC_FLAGS, GOSSIP_OPTION_QUESTGIVER);
                            Talk(SAY_XERESTRASZA_EVENT_7);
                            me->setActive(false);
                            break;
                        default:
                            break;
                    }
                }
            }

        private:
            EventMap _events;
            bool _isIntro;
            bool _introDone;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetRubySanctumAI<npc_xerestraszaAI>(creature);
        }
};

class at_baltharus_plateau : public AreaTriggerScript
{
    public:
        at_baltharus_plateau() : AreaTriggerScript("at_baltharus_plateau") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*areaTrigger*/, bool /*entered*/) override
        {
            // Only trigger once
            if (InstanceScript* instance = player->GetInstanceScript())
            {
                if (Creature* xerestrasza = instance->GetCreature(DATA_XERESTRASZA))
                    xerestrasza->AI()->DoAction(ACTION_INTRO_BALTHARUS);

                if (Creature* baltharus = instance->GetCreature(DATA_BALTHARUS_THE_WARBORN))
                    baltharus->AI()->DoAction(ACTION_INTRO_BALTHARUS);
            }

            return true;
        }
};

// 75415 - Rallying Shout
class spell_ruby_sanctum_rallying_shout : public SpellScriptLoader
{
    public:
        spell_ruby_sanctum_rallying_shout() : SpellScriptLoader("spell_ruby_sanctum_rallying_shout") { }

        class spell_ruby_sanctum_rallying_shout_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ruby_sanctum_rallying_shout_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                return ValidateSpellInfo({ SPELL_RALLY });
            }

            void CountTargets(std::list<WorldObject*>& targets)
            {
                _targetCount = targets.size();
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (_targetCount && !GetCaster()->HasAura(SPELL_RALLY))
                    GetCaster()->CastCustomSpell(SPELL_RALLY, SPELLVALUE_AURA_STACK, _targetCount, GetCaster(), TRIGGERED_FULL_MASK);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ruby_sanctum_rallying_shout_SpellScript::CountTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
                OnEffectHit += SpellEffectFn(spell_ruby_sanctum_rallying_shout_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }

        private:
            uint32 _targetCount = 0;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_ruby_sanctum_rallying_shout_SpellScript();
        }
};

void AddSC_ruby_sanctum()
{
    new npc_xerestrasza();
    new at_baltharus_plateau();
    new spell_ruby_sanctum_rallying_shout();
}
