/**
 * This file is part of the MobiusCore project.
 * See AUTHORS file for copyright information.
 */

#ifndef SC_FOLLOWERAI_H
#define SC_FOLLOWERAI_H

#include "ScriptedCreature.h"
#include "ScriptSystem.h"

class Quest;

enum eFollowState
{
    STATE_FOLLOW_NONE       = 0x000,
    STATE_FOLLOW_INPROGRESS = 0x001,                        //must always have this state for any follow
    STATE_FOLLOW_RETURNING  = 0x002,                        //when returning to combat start after being in combat
    STATE_FOLLOW_PAUSED     = 0x004,                        //disables following
    STATE_FOLLOW_COMPLETE   = 0x008,                        //follow is completed and may end
    STATE_FOLLOW_PREEVENT   = 0x010,                        //not implemented (allow pre event to run, before follow is initiated)
    STATE_FOLLOW_POSTEVENT  = 0x020                         //can be set at complete and allow post event to run
};

class GAME_API FollowerAI : public ScriptedAI
{
    public:
        explicit FollowerAI(Creature* creature);
        ~FollowerAI() { }

        //virtual void WaypointReached(uint32 uiPointId) = 0;

        void MovementInform(uint32 motionType, uint32 pointId) override;

        void AttackStart(Unit*) override;

        void MoveInLineOfSight(Unit*) override;

        void EnterEvadeMode(EvadeReason /*why*/ = EVADE_REASON_OTHER) override;

        void JustDied(Unit*) override;

        void JustRespawned() override;

        void UpdateAI(uint32) override;                        //the "internal" update, calls UpdateFollowerAI()
        virtual void UpdateFollowerAI(uint32);        //used when it's needed to add code in update (abilities, scripted events, etc)

        void StartFollow(Player* player, uint32 factionForFollower = 0, const Quest* quest = NULL);

        void SetFollowPaused(bool bPaused);                 //if special event require follow mode to hold/resume during the follow
        void SetFollowComplete(bool bWithEndEvent = false);

        bool HasFollowState(uint32 uiFollowState) { return (m_uiFollowState & uiFollowState) != 0; }

    protected:
        Player* GetLeaderForFollower();

    private:
        void AddFollowState(uint32 uiFollowState) { m_uiFollowState |= uiFollowState; }
        void RemoveFollowState(uint32 uiFollowState) { m_uiFollowState &= ~uiFollowState; }

        bool AssistPlayerInCombatAgainst(Unit* who);

        ObjectGuid m_uiLeaderGUID;
        uint32 m_uiUpdateFollowTimer;
        uint32 m_uiFollowState;

        const Quest* m_pQuestForFollow;                     //normally we have a quest
};

#endif
