/**
 * This file is part of the MobiusCore project.
 * See AUTHORS file for copyright information.
 */

#ifndef DuelPackets_h__
#define DuelPackets_h__

#include "Packet.h"
#include "ObjectGuid.h"

namespace WorldPackets
{
    namespace Duel
    {
        class CanDuel final : public ClientPacket
        {
        public:
            CanDuel(WorldPacket&& packet) : ClientPacket(CMSG_CAN_DUEL, std::move(packet)) { }

            void Read() override;

            ObjectGuid TargetGUID;
        };

        class CanDuelResult final : public ServerPacket
        {
        public:
            CanDuelResult() : ServerPacket(SMSG_CAN_DUEL_RESULT, 16 + 1) { }

            WorldPacket const* Write() override;

            ObjectGuid TargetGUID;
            bool Result = false;
        };

        class DuelComplete final : public ServerPacket
        {
        public:
            DuelComplete() : ServerPacket(SMSG_DUEL_COMPLETE, 1) { }

            WorldPacket const* Write() override;

            bool Started = false;
        };

        class DuelCountdown final : public ServerPacket
        {
        public:
            DuelCountdown(uint32 countdown) : ServerPacket(SMSG_DUEL_COUNTDOWN, 4), Countdown(countdown) { }

            WorldPacket const* Write() override;

            uint32 Countdown = 0;
        };

        class DuelInBounds final : public ServerPacket
        {
        public:
            DuelInBounds() : ServerPacket(SMSG_DUEL_IN_BOUNDS, 0) { }

            WorldPacket const* Write() override { return &_worldPacket; }
        };

        class DuelOutOfBounds final : public ServerPacket
        {
        public:
            DuelOutOfBounds() : ServerPacket(SMSG_DUEL_OUT_OF_BOUNDS, 0) { }

            WorldPacket const* Write() override { return &_worldPacket; }
        };

        class DuelRequested final : public ServerPacket
        {
        public:
            DuelRequested() : ServerPacket(SMSG_DUEL_REQUESTED, 16 * 3) { }

            WorldPacket const* Write() override;

            ObjectGuid ArbiterGUID;
            ObjectGuid RequestedByGUID;
            ObjectGuid RequestedByWowAccount;
        };

        class DuelResponse : public ClientPacket
        {
        public:
            DuelResponse(WorldPacket&& packet) : ClientPacket(CMSG_DUEL_RESPONSE, std::move(packet)) { }

            void Read() override;

            ObjectGuid ArbiterGUID;
            bool Accepted = false;
        };

        class DuelWinner final : public ServerPacket
        {
        public:
            DuelWinner() : ServerPacket(SMSG_DUEL_WINNER, 4 * 4 + 1) { }

            WorldPacket const* Write() override;

            std::string BeatenName;
            std::string WinnerName;
            uint32 BeatenVirtualRealmAddress = 0;
            uint32 WinnerVirtualRealmAddress = 0;
            bool Fled = false;
        };
    }
}

#endif // DuelPackets_h__
