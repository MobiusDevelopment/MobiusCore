/**
 * This file is part of the MobiusCore project.
 * See AUTHORS file for copyright information.
 */

#include "InstancePackets.h"

WorldPacket const* WorldPackets::Instance::UpdateLastInstance::Write()
{
    _worldPacket << uint32(MapID);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Instance::UpdateInstanceOwnership::Write()
{
    _worldPacket << int32(IOwnInstance);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Instance::InstanceInfo::Write()
{
    _worldPacket << int32(LockList.size());

    for (InstanceLockInfos const& lockInfos : LockList)
        _worldPacket << lockInfos;

    return &_worldPacket;
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Instance::InstanceLockInfos const& lockInfos)
{
    data << lockInfos.MapID;
    data << lockInfos.DifficultyID;
    data << lockInfos.InstanceID;
    data << lockInfos.TimeRemaining;
    data << lockInfos.CompletedMask;

    data.WriteBit(lockInfos.Locked);
    data.WriteBit(lockInfos.Extended);

    data.FlushBits();

    return data;
}

WorldPacket const* WorldPackets::Instance::InstanceReset::Write()
{
    _worldPacket << uint32(MapID);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Instance::InstanceResetFailed::Write()
{
    _worldPacket << uint32(MapID);
    _worldPacket.WriteBits(ResetFailedReason, 2);
    _worldPacket.FlushBits();

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Instance::InstanceSaveCreated::Write()
{
    _worldPacket.WriteBit(Gm);
    _worldPacket.FlushBits();

    return &_worldPacket;
}

void WorldPackets::Instance::InstanceLockResponse::Read()
{
    AcceptLock = _worldPacket.ReadBit();
}

WorldPacket const* WorldPackets::Instance::RaidGroupOnly::Write()
{
    _worldPacket << Delay;
    _worldPacket << Reason;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Instance::PendingRaidLock::Write()
{
    _worldPacket << int32(TimeUntilLock);
    _worldPacket << uint32(CompletedMask);
    _worldPacket.WriteBit(Extending);
    _worldPacket.WriteBit(WarningOnly);
    _worldPacket.FlushBits();

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Instance::RaidInstanceMessage::Write()
{
    _worldPacket << uint8(Type);
    _worldPacket << uint32(MapID);
    _worldPacket << uint32(DifficultyID);
    _worldPacket.WriteBit(Locked);
    _worldPacket.WriteBit(Extended);
    _worldPacket.FlushBits();

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Instance::InstanceEncounterEngageUnit::Write()
{
    _worldPacket << Unit;
    _worldPacket << uint8(TargetFramePriority);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Instance::InstanceEncounterDisengageUnit::Write()
{
    _worldPacket << Unit;

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Instance::InstanceEncounterChangePriority::Write()
{
    _worldPacket << Unit;
    _worldPacket << uint8(TargetFramePriority);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Instance::InstanceEncounterStart::Write()
{
    _worldPacket << uint32(InCombatResCount);
    _worldPacket << uint32(MaxInCombatResCount);
    _worldPacket << uint32(CombatResChargeRecovery);
    _worldPacket << uint32(NextCombatResChargeTime);
    _worldPacket.WriteBit(InProgress);
    _worldPacket.FlushBits();

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Instance::InstanceEncounterGainCombatResurrectionCharge::Write()
{
    _worldPacket << int32(InCombatResCount);
    _worldPacket << uint32(CombatResChargeRecovery);

    return &_worldPacket;
}

WorldPacket const* WorldPackets::Instance::BossKillCredit::Write()
{
    _worldPacket << uint32(DungeonEncounterID);

    return &_worldPacket;
}
