/**
 * This file is part of the MobiusCore project.
 * See AUTHORS file for copyright information.
 */

#include "ScriptMgr.h"
#include "Creature.h"
#include "GameObject.h"
#include "MapManager.h"
#include "ObjectMgr.h"
#include "OutdoorPvPNA.h"
#include "Player.h"
#include "WorldStatePackets.h"

OutdoorPvPNA::OutdoorPvPNA()
{
    m_TypeId = OUTDOOR_PVP_NA;
    m_obj = NULL;
}

void OutdoorPvPNA::HandleKillImpl(Player* player, Unit* killed)
{
    if (killed->GetTypeId() == TYPEID_PLAYER && player->GetTeam() != killed->ToPlayer()->GetTeam())
    {
        player->KilledMonsterCredit(NA_CREDIT_MARKER); // 0 guid, btw it isn't even used in killedmonster function :S
        if (player->GetTeam() == ALLIANCE)
            player->CastSpell(player, NA_KILL_TOKEN_ALLIANCE, true);
        else
            player->CastSpell(player, NA_KILL_TOKEN_HORDE, true);
    }
}

uint32 OPvPCapturePointNA::GetAliveGuardsCount()
{
    uint32 cnt = 0;
    for (std::map<uint32, ObjectGuid::LowType>::iterator itr = m_Creatures.begin(); itr != m_Creatures.end(); ++itr)
    {
        switch (itr->first)
        {
        case NA_NPC_GUARD_01:
        case NA_NPC_GUARD_02:
        case NA_NPC_GUARD_03:
        case NA_NPC_GUARD_04:
        case NA_NPC_GUARD_05:
        case NA_NPC_GUARD_06:
        case NA_NPC_GUARD_07:
        case NA_NPC_GUARD_08:
        case NA_NPC_GUARD_09:
        case NA_NPC_GUARD_10:
        case NA_NPC_GUARD_11:
        case NA_NPC_GUARD_12:
        case NA_NPC_GUARD_13:
        case NA_NPC_GUARD_14:
        case NA_NPC_GUARD_15:
        {
            auto bounds = m_PvP->GetMap()->GetCreatureBySpawnIdStore().equal_range(itr->second);
            for (auto itr2 = bounds.first; itr2 != bounds.second; ++itr2)
                if (itr2->second->IsAlive())
                    ++cnt;
            break;
        }
        default:
            break;
        }
    }
    return cnt;
}

uint32 OPvPCapturePointNA::GetControllingFaction() const
{
    return m_ControllingFaction;
}

void OPvPCapturePointNA::SpawnNPCsForTeam(uint32 team)
{
    creature_type const* creatures = nullptr;
    if (team == ALLIANCE)
        creatures = AllianceControlNPCs;
    else if (team == HORDE)
        creatures = HordeControlNPCs;
    else
        return;
    for (int i = 0; i < NA_CONTROL_NPC_NUM; ++i)
        AddCreature(i, creatures[i].entry, creatures[i].map, creatures[i].x, creatures[i].y, creatures[i].z, creatures[i].o, OutdoorPvP::GetTeamIdByTeam(team), 1000000);
}

void OPvPCapturePointNA::DeSpawnNPCs()
{
    for (int i = 0; i < NA_CONTROL_NPC_NUM; ++i)
        DelCreature(i);
}

void OPvPCapturePointNA::SpawnGOsForTeam(uint32 team)
{
    const go_type * gos = NULL;
    if (team == ALLIANCE)
        gos=AllianceControlGOs;
    else if (team == HORDE)
        gos=HordeControlGOs;
    else
        return;
    for (int i = 0; i < NA_CONTROL_GO_NUM; ++i)
    {
        if (i == NA_ROOST_S ||
            i == NA_ROOST_W ||
            i == NA_ROOST_N ||
            i == NA_ROOST_E ||
            i == NA_BOMB_WAGON_S ||
            i == NA_BOMB_WAGON_W ||
            i == NA_BOMB_WAGON_N ||
            i == NA_BOMB_WAGON_E)
            continue;   // roosts and bomb wagons are spawned when someone uses the matching destroyed roost
        AddObject(i, gos[i].entry, gos[i].map, gos[i].x, gos[i].y, gos[i].z, gos[i].o, gos[i].rot0, gos[i].rot1, gos[i].rot2, gos[i].rot3);
    }
}

void OPvPCapturePointNA::DeSpawnGOs()
{
    for (int i = 0; i < NA_CONTROL_GO_NUM; ++i)
    {
        DelObject(i);
    }
}

void OPvPCapturePointNA::FactionTakeOver(uint32 team)
{
    if (m_ControllingFaction)
        sObjectMgr->RemoveGraveYardLink(NA_HALAA_GRAVEYARD, NA_HALAA_GRAVEYARD_ZONE, m_ControllingFaction, false);

    m_ControllingFaction = team;
    if (m_ControllingFaction)
        sObjectMgr->AddGraveYardLink(NA_HALAA_GRAVEYARD, NA_HALAA_GRAVEYARD_ZONE, m_ControllingFaction, false);
    DeSpawnGOs();
    DeSpawnNPCs();
    SpawnGOsForTeam(team);
    SpawnNPCsForTeam(team);
    m_GuardsAlive = NA_GUARDS_MAX;
    m_capturable = false;
    this->UpdateHalaaWorldState();
    if (team == ALLIANCE)
    {
        m_WyvernStateSouth = WYVERN_NEU_HORDE;
        m_WyvernStateNorth = WYVERN_NEU_HORDE;
        m_WyvernStateEast = WYVERN_NEU_HORDE;
        m_WyvernStateWest = WYVERN_NEU_HORDE;
        m_PvP->TeamApplyBuff(TEAM_ALLIANCE, NA_CAPTURE_BUFF);
        m_PvP->SendUpdateWorldState(NA_UI_HORDE_GUARDS_SHOW, 0);
        m_PvP->SendUpdateWorldState(NA_UI_ALLIANCE_GUARDS_SHOW, 1);
        m_PvP->SendUpdateWorldState(NA_UI_GUARDS_LEFT, m_GuardsAlive);
        m_PvP->SendDefenseMessage(NA_HALAA_GRAVEYARD_ZONE, TEXT_HALAA_TAKEN_ALLIANCE);
    }
    else
    {
        m_WyvernStateSouth = WYVERN_NEU_ALLIANCE;
        m_WyvernStateNorth = WYVERN_NEU_ALLIANCE;
        m_WyvernStateEast = WYVERN_NEU_ALLIANCE;
        m_WyvernStateWest = WYVERN_NEU_ALLIANCE;
        m_PvP->TeamApplyBuff(TEAM_HORDE, NA_CAPTURE_BUFF);
        m_PvP->SendUpdateWorldState(NA_UI_HORDE_GUARDS_SHOW, 1);
        m_PvP->SendUpdateWorldState(NA_UI_ALLIANCE_GUARDS_SHOW, 0);
        m_PvP->SendUpdateWorldState(NA_UI_GUARDS_LEFT, m_GuardsAlive);
        m_PvP->SendDefenseMessage(NA_HALAA_GRAVEYARD_ZONE, TEXT_HALAA_TAKEN_HORDE);
    }
    UpdateWyvernRoostWorldState(NA_ROOST_S);
    UpdateWyvernRoostWorldState(NA_ROOST_N);
    UpdateWyvernRoostWorldState(NA_ROOST_W);
    UpdateWyvernRoostWorldState(NA_ROOST_E);
}

OPvPCapturePointNA::OPvPCapturePointNA(OutdoorPvP* pvp) :
OPvPCapturePoint(pvp), m_capturable(true), m_GuardsAlive(0), m_ControllingFaction(0),
m_WyvernStateNorth(0), m_WyvernStateSouth(0), m_WyvernStateEast(0), m_WyvernStateWest(0),
m_HalaaState(HALAA_N), m_RespawnTimer(NA_RESPAWN_TIME), m_GuardCheckTimer(NA_GUARD_CHECK_TIME)
{
    SetCapturePointData(182210, 530, -1572.57f, 7945.3f, -22.475f, 2.05949f, 0.0f, 0.0f, 0.857167f, 0.515038f);
}

bool OutdoorPvPNA::SetupOutdoorPvP()
{
//    m_TypeId = OUTDOOR_PVP_NA; _MUST_ be set in ctor, because of spawns cleanup
    // add the zones affected by the pvp buff
    SetMapFromZone(NA_BUFF_ZONE);
    RegisterZone(NA_BUFF_ZONE);

    // halaa
    m_obj = new OPvPCapturePointNA(this);

    AddCapturePoint(m_obj);
    return true;
}

void OutdoorPvPNA::HandlePlayerEnterZone(Player* player, uint32 zone)
{
    // add buffs
    if (player->GetTeam() == m_obj->GetControllingFaction())
        player->CastSpell(player, NA_CAPTURE_BUFF, true);
    OutdoorPvP::HandlePlayerEnterZone(player, zone);
}

void OutdoorPvPNA::HandlePlayerLeaveZone(Player* player, uint32 zone)
{
    // remove buffs
    player->RemoveAurasDueToSpell(NA_CAPTURE_BUFF);
    OutdoorPvP::HandlePlayerLeaveZone(player, zone);
}

void OutdoorPvPNA::FillInitialWorldStates(WorldPackets::WorldState::InitWorldStates& packet)
{
    m_obj->FillInitialWorldStates(packet);
}

void OPvPCapturePointNA::FillInitialWorldStates(WorldPackets::WorldState::InitWorldStates& packet)
{
    if (m_ControllingFaction == ALLIANCE)
    {
        packet.Worldstates.emplace_back(uint32(NA_UI_HORDE_GUARDS_SHOW), 0);
        packet.Worldstates.emplace_back(uint32(NA_UI_ALLIANCE_GUARDS_SHOW), 1);
    }
    else if (m_ControllingFaction == HORDE)
    {
        packet.Worldstates.emplace_back(uint32(NA_UI_HORDE_GUARDS_SHOW), 1);
        packet.Worldstates.emplace_back(uint32(NA_UI_ALLIANCE_GUARDS_SHOW), 1);
    }
    else
    {
        packet.Worldstates.emplace_back(uint32(NA_UI_HORDE_GUARDS_SHOW), 0);
        packet.Worldstates.emplace_back(uint32(NA_UI_ALLIANCE_GUARDS_SHOW), 0);
    }

    packet.Worldstates.emplace_back(uint32(NA_UI_GUARDS_MAX), int32(NA_GUARDS_MAX));
    packet.Worldstates.emplace_back(uint32(NA_UI_GUARDS_LEFT), int32(m_GuardsAlive));

    packet.Worldstates.emplace_back(uint32(NA_MAP_WYVERN_NORTH_NEU_A), int32((m_WyvernStateNorth & WYVERN_NEU_HORDE) != 0));
    packet.Worldstates.emplace_back(uint32(NA_MAP_WYVERN_NORTH_NEU_A), int32((m_WyvernStateNorth & WYVERN_NEU_ALLIANCE) != 0));
    packet.Worldstates.emplace_back(uint32(NA_MAP_WYVERN_NORTH_H), int32((m_WyvernStateNorth & WYVERN_HORDE) != 0));
    packet.Worldstates.emplace_back(uint32(NA_MAP_WYVERN_NORTH_A), int32((m_WyvernStateNorth & WYVERN_ALLIANCE) != 0));

    packet.Worldstates.emplace_back(uint32(NA_MAP_WYVERN_SOUTH_NEU_H), int32((m_WyvernStateSouth & WYVERN_NEU_HORDE) != 0));
    packet.Worldstates.emplace_back(uint32(NA_MAP_WYVERN_SOUTH_NEU_A), int32((m_WyvernStateSouth & WYVERN_NEU_ALLIANCE) != 0));
    packet.Worldstates.emplace_back(uint32(NA_MAP_WYVERN_SOUTH_H), int32((m_WyvernStateSouth & WYVERN_HORDE) != 0));
    packet.Worldstates.emplace_back(uint32(NA_MAP_WYVERN_SOUTH_A), int32((m_WyvernStateSouth & WYVERN_ALLIANCE) != 0));

    packet.Worldstates.emplace_back(uint32(NA_MAP_WYVERN_WEST_NEU_H), int32((m_WyvernStateWest & WYVERN_NEU_HORDE) != 0));
    packet.Worldstates.emplace_back(uint32(NA_MAP_WYVERN_WEST_NEU_A), int32((m_WyvernStateWest & WYVERN_NEU_ALLIANCE) != 0));
    packet.Worldstates.emplace_back(uint32(NA_MAP_WYVERN_WEST_H), int32((m_WyvernStateWest & WYVERN_HORDE) != 0));
    packet.Worldstates.emplace_back(uint32(NA_MAP_WYVERN_WEST_A), int32((m_WyvernStateWest & WYVERN_ALLIANCE) != 0));

    packet.Worldstates.emplace_back(uint32(NA_MAP_WYVERN_EAST_NEU_H), int32((m_WyvernStateEast & WYVERN_NEU_HORDE) != 0));
    packet.Worldstates.emplace_back(uint32(NA_MAP_WYVERN_EAST_NEU_A), int32((m_WyvernStateEast & WYVERN_NEU_ALLIANCE) != 0));
    packet.Worldstates.emplace_back(uint32(NA_MAP_WYVERN_EAST_H), int32((m_WyvernStateEast & WYVERN_HORDE) != 0));
    packet.Worldstates.emplace_back(uint32(NA_MAP_WYVERN_EAST_A), int32((m_WyvernStateEast & WYVERN_ALLIANCE) != 0));

    packet.Worldstates.emplace_back(uint32(NA_MAP_HALAA_NEUTRAL), int32((m_HalaaState & HALAA_N) != 0));
    packet.Worldstates.emplace_back(uint32(NA_MAP_HALAA_NEU_A), int32((m_HalaaState & HALAA_N_A) != 0));
    packet.Worldstates.emplace_back(uint32(NA_MAP_HALAA_NEU_H), int32((m_HalaaState & HALAA_N_H) != 0));
    packet.Worldstates.emplace_back(uint32(NA_MAP_HALAA_HORDE), int32((m_HalaaState & HALAA_H) != 0));
    packet.Worldstates.emplace_back(uint32(NA_MAP_HALAA_ALLIANCE), int32((m_HalaaState & HALAA_A) != 0));
}

void OutdoorPvPNA::SendRemoveWorldStates(Player* player)
{
    player->SendUpdateWorldState(NA_UI_HORDE_GUARDS_SHOW, 0);
    player->SendUpdateWorldState(NA_UI_ALLIANCE_GUARDS_SHOW, 0);
    player->SendUpdateWorldState(NA_UI_GUARDS_MAX, 0);
    player->SendUpdateWorldState(NA_UI_GUARDS_LEFT, 0);
    player->SendUpdateWorldState(NA_MAP_WYVERN_NORTH_NEU_H, 0);
    player->SendUpdateWorldState(NA_MAP_WYVERN_NORTH_NEU_A, 0);
    player->SendUpdateWorldState(NA_MAP_WYVERN_NORTH_H, 0);
    player->SendUpdateWorldState(NA_MAP_WYVERN_NORTH_A, 0);
    player->SendUpdateWorldState(NA_MAP_WYVERN_SOUTH_NEU_H, 0);
    player->SendUpdateWorldState(NA_MAP_WYVERN_SOUTH_NEU_A, 0);
    player->SendUpdateWorldState(NA_MAP_WYVERN_SOUTH_H, 0);
    player->SendUpdateWorldState(NA_MAP_WYVERN_SOUTH_A, 0);
    player->SendUpdateWorldState(NA_MAP_WYVERN_WEST_NEU_H, 0);
    player->SendUpdateWorldState(NA_MAP_WYVERN_WEST_NEU_A, 0);
    player->SendUpdateWorldState(NA_MAP_WYVERN_WEST_H, 0);
    player->SendUpdateWorldState(NA_MAP_WYVERN_WEST_A, 0);
    player->SendUpdateWorldState(NA_MAP_WYVERN_EAST_NEU_H, 0);
    player->SendUpdateWorldState(NA_MAP_WYVERN_EAST_NEU_A, 0);
    player->SendUpdateWorldState(NA_MAP_WYVERN_EAST_H, 0);
    player->SendUpdateWorldState(NA_MAP_WYVERN_EAST_A, 0);
    player->SendUpdateWorldState(NA_MAP_HALAA_NEUTRAL, 0);
    player->SendUpdateWorldState(NA_MAP_HALAA_NEU_A, 0);
    player->SendUpdateWorldState(NA_MAP_HALAA_NEU_H, 0);
    player->SendUpdateWorldState(NA_MAP_HALAA_HORDE, 0);
    player->SendUpdateWorldState(NA_MAP_HALAA_ALLIANCE, 0);
}

bool OutdoorPvPNA::Update(uint32 diff)
{
    return m_obj->Update(diff);
}

bool OPvPCapturePointNA::HandleCustomSpell(Player* player, uint32 spellId, GameObject* /*go*/)
{
    std::vector<uint32> nodes;
    nodes.resize(2);
    bool retval = false;
    switch (spellId)
    {
    case NA_SPELL_FLY_NORTH:
        nodes[0] = FlightPathStartNodes[NA_ROOST_N];
        nodes[1] = FlightPathEndNodes[NA_ROOST_N];
        player->ActivateTaxiPathTo(nodes);
        player->SetFlag(PLAYER_FLAGS, PLAYER_FLAGS_IN_PVP);
        player->UpdatePvP(true, true);
        retval = true;
        break;
    case NA_SPELL_FLY_SOUTH:
        nodes[0] = FlightPathStartNodes[NA_ROOST_S];
        nodes[1] = FlightPathEndNodes[NA_ROOST_S];
        player->ActivateTaxiPathTo(nodes);
        player->SetFlag(PLAYER_FLAGS, PLAYER_FLAGS_IN_PVP);
        player->UpdatePvP(true, true);
        retval = true;
        break;
    case NA_SPELL_FLY_WEST:
        nodes[0] = FlightPathStartNodes[NA_ROOST_W];
        nodes[1] = FlightPathEndNodes[NA_ROOST_W];
        player->ActivateTaxiPathTo(nodes);
        player->SetFlag(PLAYER_FLAGS, PLAYER_FLAGS_IN_PVP);
        player->UpdatePvP(true, true);
        retval = true;
        break;
    case NA_SPELL_FLY_EAST:
        nodes[0] = FlightPathStartNodes[NA_ROOST_E];
        nodes[1] = FlightPathEndNodes[NA_ROOST_E];
        player->ActivateTaxiPathTo(nodes);
        player->SetFlag(PLAYER_FLAGS, PLAYER_FLAGS_IN_PVP);
        player->UpdatePvP(true, true);
        retval = true;
        break;
    default:
        break;
    }

    if (retval)
    {
        //Adding items
        uint32 noSpaceForCount = 0;

        // check space and find places
        ItemPosCountVec dest;

        int32 count = 10;
        uint32 itemid = 24538;
                                                                // bomb id count
        InventoryResult msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, itemid, count, &noSpaceForCount);
        if (msg != EQUIP_ERR_OK)                               // convert to possible store amount
            count -= noSpaceForCount;

        if (count == 0 || dest.empty())                         // can't add any
        {
            return true;
        }

        Item* item = player->StoreNewItem(dest, itemid, true);

        if (count > 0 && item)
        {
            player->SendNewItem(item, count, true, false);
        }

        return true;
    }
    return false;
}

int32 OPvPCapturePointNA::HandleOpenGo(Player* player, GameObject* go)
{
    int32 retval = OPvPCapturePoint::HandleOpenGo(player, go);
    if (retval >= 0)
    {
        const go_type * gos = NULL;
        if (m_ControllingFaction == ALLIANCE)
            gos=AllianceControlGOs;
        else if (m_ControllingFaction == HORDE)
            gos=HordeControlGOs;
        else
            return -1;

        int32 del = -1;
        int32 del2 = -1;
        int32 add = -1;
        int32 add2 = -1;

        switch (retval)
        {
        case NA_DESTROYED_ROOST_S:
            del = NA_DESTROYED_ROOST_S;
            add = NA_ROOST_S;
            add2 = NA_BOMB_WAGON_S;
            if (m_ControllingFaction == HORDE)
                m_WyvernStateSouth = WYVERN_ALLIANCE;
            else
                m_WyvernStateSouth = WYVERN_HORDE;
            UpdateWyvernRoostWorldState(NA_ROOST_S);
            break;
        case NA_DESTROYED_ROOST_N:
            del = NA_DESTROYED_ROOST_N;
            add = NA_ROOST_N;
            add2 = NA_BOMB_WAGON_N;
            if (m_ControllingFaction == HORDE)
                m_WyvernStateNorth = WYVERN_ALLIANCE;
            else
                m_WyvernStateNorth = WYVERN_HORDE;
            UpdateWyvernRoostWorldState(NA_ROOST_N);
            break;
        case NA_DESTROYED_ROOST_W:
            del = NA_DESTROYED_ROOST_W;
            add = NA_ROOST_W;
            add2 = NA_BOMB_WAGON_W;
            if (m_ControllingFaction == HORDE)
                m_WyvernStateWest = WYVERN_ALLIANCE;
            else
                m_WyvernStateWest = WYVERN_HORDE;
            UpdateWyvernRoostWorldState(NA_ROOST_W);
            break;
        case NA_DESTROYED_ROOST_E:
            del = NA_DESTROYED_ROOST_E;
            add = NA_ROOST_E;
            add2 = NA_BOMB_WAGON_E;
            if (m_ControllingFaction == HORDE)
                m_WyvernStateEast = WYVERN_ALLIANCE;
            else
                m_WyvernStateEast = WYVERN_HORDE;
            UpdateWyvernRoostWorldState(NA_ROOST_E);
            break;
        case NA_BOMB_WAGON_S:
            del = NA_BOMB_WAGON_S;
            del2 = NA_ROOST_S;
            add = NA_DESTROYED_ROOST_S;
            if (m_ControllingFaction == HORDE)
                m_WyvernStateSouth = WYVERN_NEU_ALLIANCE;
            else
                m_WyvernStateSouth = WYVERN_NEU_HORDE;
            UpdateWyvernRoostWorldState(NA_ROOST_S);
            break;
        case NA_BOMB_WAGON_N:
            del = NA_BOMB_WAGON_N;
            del2 = NA_ROOST_N;
            add = NA_DESTROYED_ROOST_N;
            if (m_ControllingFaction == HORDE)
                m_WyvernStateNorth = WYVERN_NEU_ALLIANCE;
            else
                m_WyvernStateNorth = WYVERN_NEU_HORDE;
            UpdateWyvernRoostWorldState(NA_ROOST_N);
            break;
        case NA_BOMB_WAGON_W:
            del = NA_BOMB_WAGON_W;
            del2 = NA_ROOST_W;
            add = NA_DESTROYED_ROOST_W;
            if (m_ControllingFaction == HORDE)
                m_WyvernStateWest = WYVERN_NEU_ALLIANCE;
            else
                m_WyvernStateWest = WYVERN_NEU_HORDE;
            UpdateWyvernRoostWorldState(NA_ROOST_W);
            break;
        case NA_BOMB_WAGON_E:
            del = NA_BOMB_WAGON_E;
            del2 = NA_ROOST_E;
            add = NA_DESTROYED_ROOST_E;
            if (m_ControllingFaction == HORDE)
                m_WyvernStateEast = WYVERN_NEU_ALLIANCE;
            else
                m_WyvernStateEast = WYVERN_NEU_HORDE;
            UpdateWyvernRoostWorldState(NA_ROOST_E);
            break;
        default:
            return -1;
            break;
        }

        if (del>-1)
            DelObject(del);

        if (del2>-1)
            DelObject(del2);

        if (add>-1)
            AddObject(add, gos[add].entry, gos[add].map, gos[add].x, gos[add].y, gos[add].z, gos[add].o, gos[add].rot0, gos[add].rot1, gos[add].rot2, gos[add].rot3);

        if (add2>-1)
            AddObject(add2, gos[add2].entry, gos[add2].map, gos[add2].x, gos[add2].y, gos[add2].z, gos[add2].o, gos[add2].rot0, gos[add2].rot1, gos[add2].rot2, gos[add2].rot3);

        return retval;
    }
    return -1;
}

bool OPvPCapturePointNA::Update(uint32 diff)
{
    // let the controlling faction advance in phase
    bool capturable = false;
    if (m_ControllingFaction == ALLIANCE && m_activePlayers[0].size() > m_activePlayers[1].size())
        capturable = true;
    else if (m_ControllingFaction == HORDE && m_activePlayers[0].size() < m_activePlayers[1].size())
        capturable = true;

    if (m_GuardCheckTimer < diff)
    {
        m_GuardCheckTimer = NA_GUARD_CHECK_TIME;
        uint32 cnt = GetAliveGuardsCount();
        if (cnt != m_GuardsAlive)
        {
            m_GuardsAlive = cnt;
            if (m_GuardsAlive == 0)
                m_capturable = true;
            // update the guard count for the players in zone
            m_PvP->SendUpdateWorldState(NA_UI_GUARDS_LEFT, m_GuardsAlive);
        }
    } else m_GuardCheckTimer -= diff;

    if (m_capturable || capturable)
    {
        if (m_RespawnTimer < diff)
        {
            // if the guards have been killed, then the challenger has one hour to take over halaa.
            // in case they fail to do it, the guards are respawned, and they have to start again.
            if (m_ControllingFaction)
                FactionTakeOver(m_ControllingFaction);
            m_RespawnTimer = NA_RESPAWN_TIME;
        } else m_RespawnTimer -= diff;

        return OPvPCapturePoint::Update(diff);
    }
    return false;
}

void OPvPCapturePointNA::ChangeState()
{
    uint32 artkit = 21;
    switch (m_State)
    {
        case OBJECTIVESTATE_NEUTRAL:
            m_HalaaState = HALAA_N;
            break;
        case OBJECTIVESTATE_ALLIANCE:
            m_HalaaState = HALAA_A;
            FactionTakeOver(ALLIANCE);
            artkit = 2;
            break;
        case OBJECTIVESTATE_HORDE:
            m_HalaaState = HALAA_H;
            FactionTakeOver(HORDE);
            artkit = 1;
            break;
        case OBJECTIVESTATE_NEUTRAL_ALLIANCE_CHALLENGE:
            m_HalaaState = HALAA_N_A;
            break;
        case OBJECTIVESTATE_NEUTRAL_HORDE_CHALLENGE:
            m_HalaaState = HALAA_N_H;
            break;
        case OBJECTIVESTATE_ALLIANCE_HORDE_CHALLENGE:
            m_HalaaState = HALAA_N_A;
            artkit = 2;
            break;
        case OBJECTIVESTATE_HORDE_ALLIANCE_CHALLENGE:
            m_HalaaState = HALAA_N_H;
            artkit = 1;
        break;
    }

    auto bounds = sMapMgr->FindMap(530, 0)->GetGameObjectBySpawnIdStore().equal_range(m_capturePointSpawnId);
    for (auto itr = bounds.first; itr != bounds.second; ++itr)
        itr->second->SetGoArtKit(artkit);

    UpdateHalaaWorldState();
}

void OPvPCapturePointNA::UpdateHalaaWorldState()
{
    m_PvP->SendUpdateWorldState(NA_MAP_HALAA_NEUTRAL, uint32((m_HalaaState & HALAA_N) != 0));
    m_PvP->SendUpdateWorldState(NA_MAP_HALAA_NEU_A, uint32((m_HalaaState & HALAA_N_A) != 0));
    m_PvP->SendUpdateWorldState(NA_MAP_HALAA_NEU_H, uint32((m_HalaaState & HALAA_N_H) != 0));
    m_PvP->SendUpdateWorldState(NA_MAP_HALAA_HORDE, uint32((m_HalaaState & HALAA_H) != 0));
    m_PvP->SendUpdateWorldState(NA_MAP_HALAA_ALLIANCE, uint32((m_HalaaState & HALAA_A) != 0));
}

void OPvPCapturePointNA::UpdateWyvernRoostWorldState(uint32 roost)
{
    switch (roost)
    {
        case NA_ROOST_S:
            m_PvP->SendUpdateWorldState(NA_MAP_WYVERN_SOUTH_NEU_H, uint32((m_WyvernStateSouth & WYVERN_NEU_HORDE) != 0));
            m_PvP->SendUpdateWorldState(NA_MAP_WYVERN_SOUTH_NEU_A, uint32((m_WyvernStateSouth & WYVERN_NEU_ALLIANCE) != 0));
            m_PvP->SendUpdateWorldState(NA_MAP_WYVERN_SOUTH_H, uint32((m_WyvernStateSouth & WYVERN_HORDE) != 0));
            m_PvP->SendUpdateWorldState(NA_MAP_WYVERN_SOUTH_A, uint32((m_WyvernStateSouth & WYVERN_ALLIANCE) != 0));
            break;
        case NA_ROOST_N:
            m_PvP->SendUpdateWorldState(NA_MAP_WYVERN_NORTH_NEU_H, uint32((m_WyvernStateNorth & WYVERN_NEU_HORDE) != 0));
            m_PvP->SendUpdateWorldState(NA_MAP_WYVERN_NORTH_NEU_A, uint32((m_WyvernStateNorth & WYVERN_NEU_ALLIANCE) != 0));
            m_PvP->SendUpdateWorldState(NA_MAP_WYVERN_NORTH_H, uint32((m_WyvernStateNorth & WYVERN_HORDE) != 0));
            m_PvP->SendUpdateWorldState(NA_MAP_WYVERN_NORTH_A, uint32((m_WyvernStateNorth & WYVERN_ALLIANCE) != 0));
            break;
        case NA_ROOST_W:
            m_PvP->SendUpdateWorldState(NA_MAP_WYVERN_WEST_NEU_H, uint32((m_WyvernStateWest & WYVERN_NEU_HORDE) != 0));
            m_PvP->SendUpdateWorldState(NA_MAP_WYVERN_WEST_NEU_A, uint32((m_WyvernStateWest & WYVERN_NEU_ALLIANCE) != 0));
            m_PvP->SendUpdateWorldState(NA_MAP_WYVERN_WEST_H, uint32((m_WyvernStateWest & WYVERN_HORDE) != 0));
            m_PvP->SendUpdateWorldState(NA_MAP_WYVERN_WEST_A, uint32((m_WyvernStateWest & WYVERN_ALLIANCE) != 0));
            break;
        case NA_ROOST_E:
            m_PvP->SendUpdateWorldState(NA_MAP_WYVERN_EAST_NEU_H, uint32((m_WyvernStateEast & WYVERN_NEU_HORDE) != 0));
            m_PvP->SendUpdateWorldState(NA_MAP_WYVERN_EAST_NEU_A, uint32((m_WyvernStateEast & WYVERN_NEU_ALLIANCE) != 0));
            m_PvP->SendUpdateWorldState(NA_MAP_WYVERN_EAST_H, uint32((m_WyvernStateEast & WYVERN_HORDE) != 0));
            m_PvP->SendUpdateWorldState(NA_MAP_WYVERN_EAST_A, uint32((m_WyvernStateEast & WYVERN_ALLIANCE) != 0));
            break;
    }
}

class OutdoorPvP_nagrand : public OutdoorPvPScript
{
    public:
        OutdoorPvP_nagrand() : OutdoorPvPScript("outdoorpvp_na") { }

        OutdoorPvP* GetOutdoorPvP() const override
        {
            return new OutdoorPvPNA();
        }
};

void AddSC_outdoorpvp_na()
{
    new OutdoorPvP_nagrand();
}
