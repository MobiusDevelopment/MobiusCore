/**
 * This file is part of the MobiusCore project.
 * See AUTHORS file for copyright information.
 */

#ifndef SERVER_PLAYERAI_H
#define SERVER_PLAYERAI_H

#include "UnitAI.h"

class Spell;

class GAME_API PlayerAI : public UnitAI
{
    public:
        explicit PlayerAI(Player* player);

        void OnCharmed(bool /*apply*/) override { } // charm AI application for players is handled by Unit::SetCharmedBy / Unit::RemoveCharmedBy

        Creature* GetCharmer() const;
        // helper functions to determine player info
        uint16 GetSpec(Player const* who = nullptr) const;
        static bool IsPlayerHealer(Player const* who);
        bool IsHealer(Player const* who = nullptr) const { return (!who || who == me) ? _isSelfHealer : IsPlayerHealer(who); }
        static bool IsPlayerRangedAttacker(Player const* who);
        bool IsRangedAttacker(Player const* who = nullptr) const { return (!who || who == me) ? _isSelfRangedAttacker : IsPlayerRangedAttacker(who); }

    protected:
        struct TargetedSpell : public std::pair<Spell*, Unit*>
        {
            TargetedSpell() : pair<Spell*, Unit*>() { }
            TargetedSpell(Spell* first, Unit* second) : pair<Spell*, Unit*>(first, second) { }
            explicit operator bool() { return !!first; }
        };
        typedef std::pair<TargetedSpell, uint32> PossibleSpell;
        typedef std::vector<PossibleSpell> PossibleSpellVector;

        Player* const me;
        void SetIsRangedAttacker(bool state) { _isSelfRangedAttacker = state; } // this allows overriding of the default ranged attacker detection

        enum SpellTarget
        {
            TARGET_NONE,
            TARGET_VICTIM,
            TARGET_CHARMER,
            TARGET_SELF
        };
        /* Check if the specified spell can be cast on that target.
           Caller is responsible for cleaning up created Spell object from pointer. */
        TargetedSpell VerifySpellCast(uint32 spellId, Unit* target);
        /* Check if the specified spell can be cast on that target.
        Caller is responsible for cleaning up created Spell object from pointer. */
        TargetedSpell VerifySpellCast(uint32 spellId, SpellTarget target);

        /* Helper method - checks spell cast, then pushes it onto provided vector if valid. */
        template<typename T> inline void VerifyAndPushSpellCast(PossibleSpellVector& spells, uint32 spellId, T target, uint32 weight)
        {
            if (TargetedSpell spell = VerifySpellCast(spellId, target))
                spells.push_back({ spell,weight });
        }

        /* Helper method - selects one spell from the vector and returns it, while deleting everything else.
           This invalidates the vector, and empties it to prevent accidental misuse. */
        TargetedSpell SelectSpellCast(PossibleSpellVector& spells);
        /* Helper method - casts the included spell at the included target */
        void DoCastAtTarget(TargetedSpell spell);

        virtual Unit* SelectAttackTarget() const;
        void DoRangedAttackIfReady();
        void DoAutoAttackIfReady();

        // Cancels all shapeshifts that the player could voluntarily cancel
        void CancelAllShapeshifts();

    private:
        uint16 const _selfSpec;
        bool const _isSelfHealer;
        bool _isSelfRangedAttacker;
};

class GAME_API SimpleCharmedPlayerAI : public PlayerAI
{
    public:
        SimpleCharmedPlayerAI(Player* player) : PlayerAI(player), _castCheckTimer(500), _chaseCloser(false), _forceFacing(true) { }
        void UpdateAI(uint32 diff) override;
        void OnCharmed(bool apply) override;

    protected:
        Unit* SelectAttackTarget() const override;

    private:
        TargetedSpell SelectAppropriateCastForSpec();
        uint32 _castCheckTimer;
        bool _chaseCloser;
        bool _forceFacing;
};

#endif
