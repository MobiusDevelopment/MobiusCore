/**
 * This file is part of the MobiusCore project.
 * See AUTHORS file for copyright information.
 */

#include "ScriptMgr.h"
#include "Creature.h"
#include "GameObject.h"
#include "InstanceScript.h"
#include "zulgurub.h"

DoorData const doorData[] =
{
    { GO_VENOXIS_COIL,                  DATA_VENOXIS,   DOOR_TYPE_ROOM },
    { GO_ARENA_DOOR_1,                  DATA_MANDOKIR,  DOOR_TYPE_ROOM },
    { GO_FORCEFIELD,                    DATA_KILNARA,   DOOR_TYPE_ROOM },
    { GO_ZANZIL_DOOR,                   DATA_ZANZIL,    DOOR_TYPE_ROOM },
    //{ GO_THE_CACHE_OF_MADNESS_DOOR,     DATA_xxxxxxx,   DOOR_TYPE_ROOM },
    { 0,                                0,              DOOR_TYPE_ROOM }
};

class instance_zulgurub : public InstanceMapScript
{
    public:
        instance_zulgurub() : InstanceMapScript(ZGScriptName, 859) { }

        struct instance_zulgurub_InstanceMapScript : public InstanceScript
        {
            instance_zulgurub_InstanceMapScript(InstanceMap* map) : InstanceScript(map)
            {
                SetHeaders(DataHeader);
                SetBossNumber(EncounterCount);
                LoadDoorData(doorData);
            }

            void OnCreatureCreate(Creature* creature) override
            {
                switch (creature->GetEntry())
                {
                    case NPC_VENOXIS:
                        venoxisGUID = creature->GetGUID();
                        break;
                    case NPC_MANDOKIR:
                        mandokirGUID = creature->GetGUID();
                        break;
                    case NPC_KILNARA:
                        kilnaraGUID = creature->GetGUID();
                        break;
                    case NPC_ZANZIL:
                        zanzilGUID = creature->GetGUID();
                        break;
                    case NPC_JINDO:
                        jindoGUID = creature->GetGUID();
                        break;
                    case NPC_HAZZARAH:
                        hazzarahGUID = creature->GetGUID();
                        break;
                    case NPC_RENATAKI:
                        renatakiGUID = creature->GetGUID();
                        break;
                    case NPC_WUSHOOLAY:
                        wushoolayGUID = creature->GetGUID();
                        break;
                    case NPC_GRILEK:
                        grilekGUID = creature->GetGUID();
                        break;
                    case NPC_JINDO_TRIGGER:
                        jindoTiggerGUID = creature->GetGUID();
                        break;
                    default:
                        break;
                }
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                switch (go->GetEntry())
                {
                    case GO_VENOXIS_COIL:
                    case GO_ARENA_DOOR_1:
                    case GO_FORCEFIELD:
                    case GO_ZANZIL_DOOR:
                    case GO_THE_CACHE_OF_MADNESS_DOOR:
                        AddDoor(go, true);
                        break;
                    default:
                        break;
                }
            }

            void OnGameObjectRemove(GameObject* go) override
            {
                switch (go->GetEntry())
                {
                    case GO_VENOXIS_COIL:
                    case GO_ARENA_DOOR_1:
                    case GO_FORCEFIELD:
                    case GO_ZANZIL_DOOR:
                    case GO_THE_CACHE_OF_MADNESS_DOOR:
                        AddDoor(go, false);
                        break;
                    default:
                        break;
                }
            }

            bool SetBossState(uint32 type, EncounterState state) override
            {
                if (!InstanceScript::SetBossState(type, state))
                    return false;

                switch (type)
                {
                    case DATA_VENOXIS:
                    case DATA_MANDOKIR:
                    case DATA_KILNARA:
                    case DATA_ZANZIL:
                    case DATA_JINDO:
                    case DATA_HAZZARAH:
                    case DATA_RENATAKI:
                    case DATA_WUSHOOLAY:
                    case DATA_GRILEK:
                        break;
                    default:
                        break;
                }

                return true;
            }

            /*
            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                }
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                }

                return 0;
            }
            */

            ObjectGuid GetGuidData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_VENOXIS:
                        return venoxisGUID;
                    case DATA_MANDOKIR:
                        return mandokirGUID;
                    case DATA_KILNARA:
                        return kilnaraGUID;
                    case DATA_ZANZIL:
                        return zanzilGUID;
                    case DATA_JINDO:
                        return jindoGUID;
                    case DATA_HAZZARAH:
                        return hazzarahGUID;
                    case DATA_RENATAKI:
                        return renatakiGUID;
                    case DATA_WUSHOOLAY:
                        return wushoolayGUID;
                    case DATA_GRILEK:
                        return grilekGUID;
                    case DATA_JINDOR_TRIGGER:
                        return jindoTiggerGUID;
                    default:
                        break;
                }

                return ObjectGuid::Empty;
            }

        protected:
            ObjectGuid venoxisGUID;
            ObjectGuid mandokirGUID;
            ObjectGuid kilnaraGUID;
            ObjectGuid zanzilGUID;
            ObjectGuid jindoGUID;
            ObjectGuid hazzarahGUID;
            ObjectGuid renatakiGUID;
            ObjectGuid wushoolayGUID;
            ObjectGuid grilekGUID;
            ObjectGuid jindoTiggerGUID;
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_zulgurub_InstanceMapScript(map);
        }
};

void AddSC_instance_zulgurub()
{
    new instance_zulgurub();
}
