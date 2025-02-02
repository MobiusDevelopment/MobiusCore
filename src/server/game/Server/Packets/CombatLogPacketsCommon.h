/**
 * This file is part of the MobiusCore project.
 * See AUTHORS file for copyright information.
 */

#ifndef CombatLogPacketsCommon_h__
#define CombatLogPacketsCommon_h__

#include "Packet.h"

class Spell;
class Unit;

namespace WorldPackets
{
    namespace Spells
    {
        struct SpellLogPowerData
        {
            SpellLogPowerData(int32 powerType, int32 amount, int32 cost) : PowerType(powerType), Amount(amount), Cost(cost) { }

            int32 PowerType = 0;
            int32 Amount = 0;
            int32 Cost = 0;
        };

        struct SpellCastLogData
        {
            int64 Health = 0;
            int32 AttackPower = 0;
            int32 SpellPower = 0;
            std::vector<SpellLogPowerData> PowerData;

            void Initialize(Unit const* unit);
            void Initialize(Spell const* spell);
        };

        struct SandboxScalingData
        {
            enum SandboxScalingDataType : uint32
            {
                TYPE_PLAYER_TO_PLAYER               = 1, // NYI
                TYPE_CREATURE_TO_PLAYER_DAMAGE      = 2,
                TYPE_PLAYER_TO_CREATURE_DAMAGE      = 3,
                TYPE_CREATURE_TO_CREATURE_DAMAGE    = 4
            };

            uint32 Type = 0;
            int16 PlayerLevelDelta = 0;
            uint16 PlayerItemLevel = 0;
            uint8 TargetLevel = 0;
            uint8 Expansion = 0;
            uint8 Class = 0;
            uint8 TargetMinScalingLevel = 0;
            uint8 TargetMaxScalingLevel = 0;
            int8 TargetScalingLevelDelta = 0;

            template<class T, class U>
            bool GenerateDataForUnits(T* attacker, U* target);
        };
    }

    namespace CombatLog
    {
        class CombatLogServerPacket : public ServerPacket
        {
        public:
            CombatLogServerPacket(OpcodeServer opcode, size_t initialSize = 200, ConnectionType connection = CONNECTION_TYPE_DEFAULT)
                : ServerPacket(opcode, initialSize, connection), _fullLogPacket(opcode, initialSize, connection) { }

            WorldPacket const* GetFullLogPacket() const { return &_fullLogPacket; }
            WorldPacket const* GetBasicLogPacket() const { return &_worldPacket; }

            Spells::SpellCastLogData LogData;

        protected:
            template<typename T>
            void operator<<(T const& val)
            {
                _worldPacket << val;
                _fullLogPacket << val;
            }

            void WriteLogDataBit()
            {
                _worldPacket.WriteBit(false);
                _fullLogPacket.WriteBit(true);
            }

            void FlushBits()
            {
                _worldPacket.FlushBits();
                _fullLogPacket.FlushBits();
            }

            bool WriteBit(bool bit)
            {
                _worldPacket.WriteBit(bit);
                _fullLogPacket.WriteBit(bit);
                return bit;
            }

            void WriteBits(uint32 value, uint32 bitCount)
            {
                _worldPacket.WriteBits(value, bitCount);
                _fullLogPacket.WriteBits(value, bitCount);
            }

            ByteBuffer& WriteLogData();

            WorldPacket _fullLogPacket;
        };
    }
}

ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Spells::SpellCastLogData const& spellCastLogData);
ByteBuffer& operator<<(ByteBuffer& data, WorldPackets::Spells::SandboxScalingData const& sandboxScalingData);

#endif // CombatLogPacketsCommon_h__
