/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ScenarioPackets.h"
#include "ScenarioMgr.h"

namespace WorldPackets::Scenario
{
ByteBuffer& operator<<(ByteBuffer& data, BonusObjectiveData const& bonusObjective)
{
    data << int32(bonusObjective.BonusObjectiveID);
    data.WriteBit(bonusObjective.ObjectiveComplete);
    data.FlushBits();
    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, ScenarioSpellUpdate const& spell)
{
    data << uint32(spell.SpellID);
    data.WriteBit(spell.Usable);
    data.FlushBits();
    return data;
}

WorldPacket const* ScenarioState::Write()
{
    _worldPacket << ScenarioGUID;
    _worldPacket << int32(ScenarioID);
    _worldPacket << int32(CurrentStep);
    _worldPacket << uint32(DifficultyID);
    _worldPacket << uint32(WaveCurrent);
    _worldPacket << uint32(WaveMax);
    _worldPacket << uint32(TimerDuration);
    _worldPacket << uint32(CriteriaProgress.size());
    _worldPacket << uint32(BonusObjectives.size());
    _worldPacket << uint32(PickedSteps.size());
    _worldPacket << uint32(Spells.size());
    _worldPacket << PlayerGUID;

    if (!PickedSteps.empty())
        _worldPacket.append(PickedSteps.data(), PickedSteps.size());

    _worldPacket.WriteBit(ScenarioComplete);
    _worldPacket.FlushBits();

    for (Achievement::CriteriaProgress const& progress : CriteriaProgress)
        _worldPacket << progress;

    for (BonusObjectiveData const& bonusObjective : BonusObjectives)
        _worldPacket << bonusObjective;

    for (ScenarioSpellUpdate const& spell : Spells)
        _worldPacket << spell;

    return &_worldPacket;
}

WorldPacket const* ScenarioProgressUpdate::Write()
{
    _worldPacket << CriteriaProgress;

    return &_worldPacket;
}

WorldPacket const* ScenarioCompleted::Write()
{
    _worldPacket << uint32(ScenarioID);

    return &_worldPacket;
}

WorldPacket const* ScenarioVacate::Write()
{
    _worldPacket << ScenarioGUID;
    _worldPacket << int32(ScenarioID);
    _worldPacket << int32(TimeRemain);
    _worldPacket.WriteBits(Reason, 2);
    _worldPacket.FlushBits();

    return &_worldPacket;
}

void QueryScenarioPOI::Read()
{
    MissingScenarioPOIs.resize(_worldPacket.read<uint32>());
    for (int32& scenarioPOI : MissingScenarioPOIs)
        _worldPacket >> scenarioPOI;
}

WorldPacket const* ScenarioPOIs::Write()
{
    _worldPacket << uint32(ScenarioPOIDataStats.size());

    for (ScenarioPOIData const& scenarioPOIData : ScenarioPOIDataStats)
    {
        _worldPacket << int32(scenarioPOIData.CriteriaTreeID);
        _worldPacket << uint32(scenarioPOIData.ScenarioPOIs->size());

        for (ScenarioPOI const& scenarioPOI : *scenarioPOIData.ScenarioPOIs)
        {
            _worldPacket << int32(scenarioPOI.BlobIndex);
            _worldPacket << int32(scenarioPOI.MapID);
            _worldPacket << int32(scenarioPOI.UiMapID);
            _worldPacket << int32(scenarioPOI.Priority);
            _worldPacket << int32(scenarioPOI.Flags);
            _worldPacket << int32(scenarioPOI.WorldEffectID);
            _worldPacket << int32(scenarioPOI.PlayerConditionID);
            _worldPacket << int32(scenarioPOI.NavigationPlayerConditionID);
            _worldPacket << uint32(scenarioPOI.Points.size());

            for (ScenarioPOIPoint const& scenarioPOIBlobPoint : scenarioPOI.Points)
            {
                _worldPacket << int32(scenarioPOIBlobPoint.X);
                _worldPacket << int32(scenarioPOIBlobPoint.Y);
                _worldPacket << int32(scenarioPOIBlobPoint.Z);
            }
        }
    }

    return &_worldPacket;
}
}
