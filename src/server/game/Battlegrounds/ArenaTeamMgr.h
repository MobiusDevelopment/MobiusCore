/**
 * This file is part of the MobiusCore project.
 * See AUTHORS file for copyright information.
 */

#ifndef _ARENATEAMMGR_H
#define _ARENATEAMMGR_H

#include "ArenaTeam.h"
#include <unordered_map>

class GAME_API ArenaTeamMgr
{
private:
    ArenaTeamMgr();
    ~ArenaTeamMgr();

public:
    static ArenaTeamMgr* instance();

    typedef std::unordered_map<uint32, ArenaTeam*> ArenaTeamContainer;

    ArenaTeam* GetArenaTeamById(uint32 arenaTeamId) const;
    ArenaTeam* GetArenaTeamByName(std::string const& arenaTeamName) const;
    ArenaTeam* GetArenaTeamByCaptain(ObjectGuid guid) const;

    void LoadArenaTeams();
    void AddArenaTeam(ArenaTeam* arenaTeam);
    void RemoveArenaTeam(uint32 Id);

    ArenaTeamContainer::iterator GetArenaTeamMapBegin() { return ArenaTeamStore.begin(); }
    ArenaTeamContainer::iterator GetArenaTeamMapEnd()   { return ArenaTeamStore.end(); }

    uint32 GenerateArenaTeamId();
    void SetNextArenaTeamId(uint32 Id) { NextArenaTeamId = Id; }

protected:
    uint32 NextArenaTeamId;
    ArenaTeamContainer ArenaTeamStore;
};

#define sArenaTeamMgr ArenaTeamMgr::instance()

#endif
