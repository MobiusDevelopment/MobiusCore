/**
 * This file is part of the MobiusCore project.
 * See AUTHORS file for copyright information.
 */

#ifndef SERVER_SMARTSCRIPT_H
#define SERVER_SMARTSCRIPT_H

#include "Define.h"
#include "SmartScriptMgr.h"

class Creature;
class GameObject;
class Player;
class SpellInfo;
class Unit;
class WorldObject;
struct AreaTriggerEntry;
struct SceneTemplate;

class GAME_API SmartScript
{
    public:
        SmartScript();
        ~SmartScript();

        void OnInitialize(WorldObject* obj, AreaTriggerEntry const* at = nullptr, SceneTemplate const* scene = nullptr);
        void GetScript();
        void FillScript(SmartAIEventList e, WorldObject* obj, AreaTriggerEntry const* at, SceneTemplate const* scene);

        void ProcessEventsFor(SMART_EVENT e, Unit* unit = nullptr, uint32 var0 = 0, uint32 var1 = 0, bool bvar = false, const SpellInfo* spell = nullptr, GameObject* gob = nullptr, std::string const& varString = "");
        void ProcessEvent(SmartScriptHolder& e, Unit* unit = nullptr, uint32 var0 = 0, uint32 var1 = 0, bool bvar = false, const SpellInfo* spell = nullptr, GameObject* gob = nullptr, std::string const& varString = "");
        bool CheckTimer(SmartScriptHolder const& e) const;
        void RecalcTimer(SmartScriptHolder& e, uint32 min, uint32 max);
        void UpdateTimer(SmartScriptHolder& e, uint32 const diff);
        void InitTimer(SmartScriptHolder& e);
        void ProcessAction(SmartScriptHolder& e, Unit* unit = nullptr, uint32 var0 = 0, uint32 var1 = 0, bool bvar = false, const SpellInfo* spell = nullptr, GameObject* gob = nullptr, std::string const& varString = "");
        void ProcessTimedAction(SmartScriptHolder& e, uint32 const& min, uint32 const& max, Unit* unit = nullptr, uint32 var0 = 0, uint32 var1 = 0, bool bvar = false, const SpellInfo* spell = nullptr, GameObject* gob = nullptr, std::string const& varString = "");
        ObjectList* GetTargets(SmartScriptHolder const& e, Unit* invoker = nullptr);
        ObjectList* GetWorldObjectsInDist(float dist);
        void InstallTemplate(SmartScriptHolder const& e);
        SmartScriptHolder CreateSmartEvent(SMART_EVENT e, uint32 event_flags, uint32 event_param1, uint32 event_param2, uint32 event_param3, uint32 event_param4, uint32 event_param5, SMART_ACTION action, uint32 action_param1, uint32 action_param2, uint32 action_param3, uint32 action_param4, uint32 action_param5, uint32 action_param6, SMARTAI_TARGETS t, uint32 target_param1, uint32 target_param2, uint32 target_param3, uint32 phaseMask = 0);
        void AddEvent(SMART_EVENT e, uint32 event_flags, uint32 event_param1, uint32 event_param2, uint32 event_param3, uint32 event_param4, uint32 event_param5, SMART_ACTION action, uint32 action_param1, uint32 action_param2, uint32 action_param3, uint32 action_param4, uint32 action_param5, uint32 action_param6, SMARTAI_TARGETS t, uint32 target_param1, uint32 target_param2, uint32 target_param3, uint32 phaseMask = 0);
        void SetPathId(uint32 id) { mPathId = id; }
        uint32 GetPathId() const { return mPathId; }

        WorldObject* GetBaseObject();
        WorldObject* GetBaseObjectOrUnit(Unit* unit);
        static bool IsUnit(WorldObject* obj);
        static bool IsPlayer(WorldObject* obj);
        static bool IsCreature(WorldObject* obj);
        static bool IsCharmedCreature(WorldObject* obj);
        static bool IsGameObject(WorldObject* obj);

        void OnUpdate(const uint32 diff);
        void OnMoveInLineOfSight(Unit* who);

        Unit* DoSelectLowestHpFriendly(float range, uint32 MinHPDiff);
        void DoFindFriendlyCC(std::list<Creature*>& _list, float range);
        void DoFindFriendlyMissingBuff(std::list<Creature*>& list, float range, uint32 spellid);
        Unit* DoFindClosestFriendlyInRange(float range, bool playerOnly);

        bool IsSmart(Creature* c = NULL);
        bool IsSmartGO(GameObject* g = NULL);

        void StoreTargetList(ObjectList* targets, uint32 id);
        ObjectList* GetTargetList(uint32 id);

        void StoreCounter(uint32 id, uint32 value, uint32 reset);
        uint32 GetCounterValue(uint32 id) const;

        GameObject* FindGameObjectNear(WorldObject* searchObject, ObjectGuid::LowType guid) const;
        Creature* FindCreatureNear(WorldObject* searchObject, ObjectGuid::LowType guid) const;

        ObjectListMap* mTargetStorage;

        void OnReset();
        void ResetBaseObject();

        //TIMED_ACTIONLIST (script type 9 aka script9)
        void SetScript9(SmartScriptHolder& e, uint32 entry);
        Unit* GetLastInvoker(Unit* invoker = nullptr);
        ObjectGuid mLastInvoker;
        typedef std::unordered_map<uint32, uint32> CounterMap;
        CounterMap mCounterList;

    private:
        void IncPhase(uint32 p)
        {
            // protect phase from overflowing
            mEventPhase = std::min<uint32>(SMART_EVENT_PHASE_12, mEventPhase + p);
        }

        void DecPhase(uint32 p)
        {
            if (p >= mEventPhase)
                mEventPhase = 0;
            else
                mEventPhase -= p;
        }

        bool IsInPhase(uint32 p) const
        {
            if (mEventPhase == 0)
                return false;
            return ((1 << (mEventPhase - 1)) & p) != 0;
        }

        void SetPhase(uint32 p = 0) { mEventPhase = p; }

        SmartAIEventList mEvents;
        SmartAIEventList mInstallEvents;
        SmartAIEventList mTimedActionList;
        bool isProcessingTimedActionList;
        Creature* me;
        ObjectGuid meOrigGUID;
        GameObject* go;
        ObjectGuid goOrigGUID;
        AreaTriggerEntry const* trigger;
        SceneTemplate const* sceneTemplate;
        SmartScriptType mScriptType;
        uint32 mEventPhase;

        uint32 mPathId;
        SmartAIEventStoredList mStoredEvents;
        std::vector<uint32> mRemIDs;

        uint32 mTextTimer;
        uint32 mLastTextID;
        uint32 mTalkerEntry;
        bool mUseTextTimer;

        SMARTAI_TEMPLATE mTemplate;
        void InstallEvents();

        void RemoveStoredEvent(uint32 id);
};

#endif
