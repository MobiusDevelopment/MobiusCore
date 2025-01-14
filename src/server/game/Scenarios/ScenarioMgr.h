/**
 * This file is part of the MobiusCore project.
 * See AUTHORS file for copyright information.
 */

#ifndef ScenarioMgr_h__
#define ScenarioMgr_h__

#include "Common.h"
#include "Hash.h"
#include "SharedDefines.h"
#include <map>
#include <unordered_map>
#include <vector>

class InstanceScenario;
class Map;
struct ScenarioEntry;
struct ScenarioStepEntry;

struct ScenarioData
{
    ScenarioEntry const* Entry;
    std::map<uint8, ScenarioStepEntry const*> Steps;
};

/*
    Scenario data should be loaded on demand.
    The server will get data from the database which scenario ids is linked with which map id/difficulty/player team.
    The first time a scenario is loaded, the map loads and stores the scenario data for future scenario instance launches.
*/

struct ScenarioDBData
{
    uint32 MapID;
    uint8 DifficultyID;
    uint32 Scenario_A;
    uint32 Scenario_H;
};

typedef std::unordered_map<std::pair<uint32, uint8>, ScenarioDBData> ScenarioDBDataContainer;
typedef std::map<uint32, ScenarioData> ScenarioDataContainer;

enum ScenarioType
{
    SCENARIO_TYPE_SCENARIO          = 0,
    SCENARIO_TYPE_CHALLENGE_MODE    = 1,
    SCENARIO_TYPE_SOLO              = 2,
    SCENARIO_TYPE_DUNGEON           = 10,
};

struct ScenarioPOIPoint
{
    int32 X;
    int32 Y;

    ScenarioPOIPoint() : X(0), Y(0) { }
    ScenarioPOIPoint(int32 _X, int32 _Y) : X(_X), Y(_Y) { }
};

struct ScenarioPOI
{
    int32 BlobIndex;
    int32 MapID;
    int32 WorldMapAreaID;
    int32 Floor;
    int32 Priority;
    int32 Flags;
    int32 WorldEffectID;
    int32 PlayerConditionID;
    std::vector<ScenarioPOIPoint> Points;

    ScenarioPOI() : BlobIndex(0), MapID(0), WorldMapAreaID(0), Floor(0), Priority(0), Flags(0), WorldEffectID(0), PlayerConditionID(0) { }

    ScenarioPOI(int32 _BlobIndex, int32 _MapID, int32 _WorldMapAreaID, int32 _Floor, int32 _Priority, int32 _Flags, int32 _WorldEffectID,
        int32 _PlayerConditionID, std::vector<ScenarioPOIPoint> points) :
        BlobIndex(_BlobIndex), MapID(_MapID), WorldMapAreaID(_WorldMapAreaID), Floor(_Floor), Priority(_Priority), Flags(_Flags), WorldEffectID(_WorldEffectID),
        PlayerConditionID(_PlayerConditionID), Points(std::move(points)) { }

    ScenarioPOI(ScenarioPOI&& scenarioPOI) :
        BlobIndex(scenarioPOI.BlobIndex), MapID(scenarioPOI.MapID), WorldMapAreaID(scenarioPOI.WorldMapAreaID), Floor(scenarioPOI.Floor), Priority(scenarioPOI.Priority),
        Flags(scenarioPOI.Flags), WorldEffectID(scenarioPOI.WorldEffectID), PlayerConditionID(scenarioPOI.PlayerConditionID), Points(std::move(scenarioPOI.Points)) { }
};

typedef std::vector<ScenarioPOI> ScenarioPOIVector;
typedef std::unordered_map<uint32, ScenarioPOIVector> ScenarioPOIContainer;

class GAME_API ScenarioMgr
{
private:
    ScenarioMgr() { }
    ~ScenarioMgr() { }

public:
    static ScenarioMgr* Instance();

    InstanceScenario* CreateInstanceScenario(Map const* map, TeamId team) const;

    void LoadDBData();
    void LoadDB2Data();
    void LoadScenarioPOI();

    ScenarioPOIVector const* GetScenarioPOIs(int32 criteriaTreeID) const;

private:
    ScenarioDataContainer _scenarioData;
    ScenarioPOIContainer _scenarioPOIStore;
    ScenarioDBDataContainer _scenarioDBData;

    ScenarioMgr(ScenarioMgr const&) = delete;
    ScenarioMgr& operator=(ScenarioMgr const&) = delete;
};

#define sScenarioMgr ScenarioMgr::Instance()

#endif // ScenarioMgr_h__
