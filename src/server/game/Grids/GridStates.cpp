/**
 * This file is part of the MobiusCore project.
 * See AUTHORS file for copyright information.
 */

#include "GridStates.h"
#include "GridNotifiers.h"
#include "Log.h"
#include "Map.h"
#include "ObjectGridLoader.h"

void InvalidState::Update(Map&, NGridType&, GridInfo&, uint32) const
{
}

void ActiveState::Update(Map& map, NGridType& grid, GridInfo&  info, uint32 diff) const
{
    // Only check grid activity every (grid_expiry/10) ms, because it's really useless to do it every cycle
    info.UpdateTimeTracker(diff);
    if (info.getTimeTracker().Passed())
    {
        if (!grid.GetWorldObjectCountInNGrid<Player>() && !map.ActiveObjectsNearGrid(grid))
        {
            ObjectGridStoper worker;
            TypeContainerVisitor<ObjectGridStoper, GridTypeMapContainer> visitor(worker);
            grid.VisitAllGrids(visitor);
            grid.SetGridState(GRID_STATE_IDLE);
            LOG_DEBUG("maps", "Grid[%u, %u] on map %u moved to IDLE state", grid.getX(), grid.getY(), map.GetId());
        }
        else
            map.ResetGridExpiry(grid, 0.1f);
    }
}

void IdleState::Update(Map& map, NGridType& grid, GridInfo&, uint32) const
{
    map.ResetGridExpiry(grid);
    grid.SetGridState(GRID_STATE_REMOVAL);
    LOG_DEBUG("maps", "Grid[%u, %u] on map %u moved to REMOVAL state", grid.getX(), grid.getY(), map.GetId());
}

void RemovalState::Update(Map& map, NGridType& grid, GridInfo& info, uint32 diff) const
{
    if (!info.getUnloadLock())
    {
        info.UpdateTimeTracker(diff);
        if (info.getTimeTracker().Passed() && !map.UnloadGrid(grid, false))
        {
            LOG_DEBUG("maps", "Grid[%u, %u] for map %u differed unloading due to players or active objects nearby", grid.getX(), grid.getY(), map.GetId());
            map.ResetGridExpiry(grid);
        }
    }
}
