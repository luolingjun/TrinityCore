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

#include "ScriptMgr.h"
#include "AreaTrigger.h"
#include "AreaTriggerAI.h"
#include "DB2Structure.h"
#include "GameObject.h"
#include "GameTime.h"
#include "MotionMaster.h"
#include "ObjectAccessor.h"
#include "PathGenerator.h"
#include "ScriptedCreature.h"
#include "Spell.h"
#include "SpellInfo.h"
#include "Player.h"
#include "TemporarySummon.h"
#include "World.h"
#include "ZoneScript.h"

/*######
## at_coilfang_waterfall
######*/

enum CoilfangGOs
{
    GO_COILFANG_WATERFALL   = 184212
};

class AreaTrigger_at_coilfang_waterfall : public AreaTriggerScript
{
    public:
        AreaTrigger_at_coilfang_waterfall() : AreaTriggerScript("at_coilfang_waterfall") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*areaTrigger*/) override
        {
            if (GameObject* go = GetClosestGameObjectWithEntry(player, GO_COILFANG_WATERFALL, 35.0f))
                if (go->getLootState() == GO_READY)
                    go->UseDoorOrButton();

            return false;
        }
};

/*#####
## at_legion_teleporter
#####*/

enum LegionTeleporter
{
    SPELL_TELE_A_TO         = 37387,
    QUEST_GAINING_ACCESS_A  = 10589,

    SPELL_TELE_H_TO         = 37389,
    QUEST_GAINING_ACCESS_H  = 10604
};

class AreaTrigger_at_legion_teleporter : public AreaTriggerScript
{
    public:
        AreaTrigger_at_legion_teleporter() : AreaTriggerScript("at_legion_teleporter") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*areaTrigger*/) override
        {
            if (player->IsAlive() && !player->IsInCombat())
            {
                if (player->GetTeam() == ALLIANCE && player->GetQuestRewardStatus(QUEST_GAINING_ACCESS_A))
                {
                    player->CastSpell(player, SPELL_TELE_A_TO, false);
                    return true;
                }

                if (player->GetTeam() == HORDE && player->GetQuestRewardStatus(QUEST_GAINING_ACCESS_H))
                {
                    player->CastSpell(player, SPELL_TELE_H_TO, false);
                    return true;
                }

                return false;
            }
            return false;
        }
};

/*######
## at_scent_larkorwi
######*/

enum ScentLarkorwi
{
    QUEST_SCENT_OF_LARKORWI                     = 4291,
    NPC_LARKORWI_MATE                           = 9683
};

class AreaTrigger_at_scent_larkorwi : public AreaTriggerScript
{
    public:
        AreaTrigger_at_scent_larkorwi() : AreaTriggerScript("at_scent_larkorwi") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*areaTrigger*/) override
        {
            if (!player->isDead() && player->GetQuestStatus(QUEST_SCENT_OF_LARKORWI) == QUEST_STATUS_INCOMPLETE)
            {
                if (!player->FindNearestCreature(NPC_LARKORWI_MATE, 15))
                    player->SummonCreature(NPC_LARKORWI_MATE, player->GetPositionX()+5, player->GetPositionY(), player->GetPositionZ(), 3.3f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 100s);
            }

            return false;
        }
};

/*######
## at_sholazar_waygate
######*/

enum Waygate
{
    SPELL_SHOLAZAR_TO_UNGORO_TELEPORT           = 52056,
    SPELL_UNGORO_TO_SHOLAZAR_TELEPORT           = 52057,

    AT_SHOLAZAR                                 = 5046,
    AT_UNGORO                                   = 5047,

    QUEST_THE_MAKERS_OVERLOOK                   = 12613,
    QUEST_THE_MAKERS_PERCH                      = 12559,
    QUEST_MEETING_A_GREAT_ONE                   = 13956,
};

class AreaTrigger_at_sholazar_waygate : public AreaTriggerScript
{
    public:
        AreaTrigger_at_sholazar_waygate() : AreaTriggerScript("at_sholazar_waygate") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
        {
            if (!player->isDead() && (player->GetQuestStatus(QUEST_MEETING_A_GREAT_ONE) != QUEST_STATUS_NONE ||
                (player->GetQuestStatus(QUEST_THE_MAKERS_OVERLOOK) == QUEST_STATUS_REWARDED && player->GetQuestStatus(QUEST_THE_MAKERS_PERCH) == QUEST_STATUS_REWARDED)))
            {
                switch (trigger->ID)
                {
                    case AT_SHOLAZAR:
                        player->CastSpell(player, SPELL_SHOLAZAR_TO_UNGORO_TELEPORT, true);
                        break;

                    case AT_UNGORO:
                        player->CastSpell(player, SPELL_UNGORO_TO_SHOLAZAR_TELEPORT, true);
                        break;
                }
            }

            return false;
        }
};

/*######
## at_nats_landing
######*/

enum NatsLanding
{
    QUEST_NATS_BARGAIN = 11209,
    SPELL_FISH_PASTE   = 42644,
    NPC_LURKING_SHARK  = 23928
};

class AreaTrigger_at_nats_landing : public AreaTriggerScript
{
    public:
        AreaTrigger_at_nats_landing() : AreaTriggerScript("at_nats_landing") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*areaTrigger*/) override
        {
            if (!player->IsAlive() || !player->HasAura(SPELL_FISH_PASTE))
                return false;

            if (player->GetQuestStatus(QUEST_NATS_BARGAIN) == QUEST_STATUS_INCOMPLETE)
            {
                if (!player->FindNearestCreature(NPC_LURKING_SHARK, 20.0f))
                {
                    if (Creature* shark = player->SummonCreature(NPC_LURKING_SHARK, -4246.243f, -3922.356f, -7.488f, 5.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 100s))
                        shark->AI()->AttackStart(player);

                    return false;
                }
            }
            return true;
        }
};

/*######
## at_brewfest
######*/

enum Brewfest
{
    NPC_TAPPER_SWINDLEKEG       = 24711,
    NPC_IPFELKOFER_IRONKEG      = 24710,

    AT_BREWFEST_DUROTAR         = 4829,
    AT_BREWFEST_DUN_MOROGH      = 4820,

    SAY_WELCOME                 = 4,

    AREATRIGGER_TALK_COOLDOWN   = 5, // in seconds
};

class AreaTrigger_at_brewfest : public AreaTriggerScript
{
    public:
        AreaTrigger_at_brewfest() : AreaTriggerScript("at_brewfest")
        {
            // Initialize for cooldown
            _triggerTimes[AT_BREWFEST_DUROTAR] = _triggerTimes[AT_BREWFEST_DUN_MOROGH] = 0;
        }

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
        {
            uint32 triggerId = trigger->ID;
            // Second trigger happened too early after first, skip for now
            if (GameTime::GetGameTime() - _triggerTimes[triggerId] < AREATRIGGER_TALK_COOLDOWN)
                return false;

            switch (triggerId)
            {
                case AT_BREWFEST_DUROTAR:
                    if (Creature* tapper = player->FindNearestCreature(NPC_TAPPER_SWINDLEKEG, 20.0f))
                        tapper->AI()->Talk(SAY_WELCOME, player);
                    break;
                case AT_BREWFEST_DUN_MOROGH:
                    if (Creature* ipfelkofer = player->FindNearestCreature(NPC_IPFELKOFER_IRONKEG, 20.0f))
                        ipfelkofer->AI()->Talk(SAY_WELCOME, player);
                    break;
                default:
                    break;
            }

            _triggerTimes[triggerId] = GameTime::GetGameTime();
            return false;
        }

    private:
        std::map<uint32, time_t> _triggerTimes;
};

/*######
## at_area_52_entrance
######*/

enum Area52Entrance
{
    SPELL_A52_NEURALYZER  = 34400,
    NPC_SPOTLIGHT         = 19913,
    SUMMON_COOLDOWN       = 5,

    AT_AREA_52_SOUTH      = 4472,
    AT_AREA_52_NORTH      = 4466,
    AT_AREA_52_WEST       = 4471,
    AT_AREA_52_EAST       = 4422,
};

class AreaTrigger_at_area_52_entrance : public AreaTriggerScript
{
    public:
        AreaTrigger_at_area_52_entrance() : AreaTriggerScript("at_area_52_entrance")
        {
            _triggerTimes[AT_AREA_52_SOUTH] = _triggerTimes[AT_AREA_52_NORTH] = _triggerTimes[AT_AREA_52_WEST] = _triggerTimes[AT_AREA_52_EAST] = 0;
        }

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
        {
            float x = 0.0f, y = 0.0f, z = 0.0f;

            if (!player->IsAlive())
                return false;

            uint32 triggerId = trigger->ID;
            if (GameTime::GetGameTime() - _triggerTimes[triggerId] < SUMMON_COOLDOWN)
                return false;

            switch (triggerId)
            {
                case AT_AREA_52_EAST:
                    x = 3044.176f;
                    y = 3610.692f;
                    z = 143.61f;
                    break;
                case AT_AREA_52_NORTH:
                    x = 3114.87f;
                    y = 3687.619f;
                    z = 143.62f;
                    break;
                case AT_AREA_52_WEST:
                    x = 3017.79f;
                    y = 3746.806f;
                    z = 144.27f;
                    break;
                case AT_AREA_52_SOUTH:
                    x = 2950.63f;
                    y = 3719.905f;
                    z = 143.33f;
                    break;
            }

            player->SummonCreature(NPC_SPOTLIGHT, x, y, z, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 5s);
            player->AddAura(SPELL_A52_NEURALYZER, player);
            _triggerTimes[trigger->ID] = GameTime::GetGameTime();
            return false;
        }

    private:
        std::map<uint32, time_t> _triggerTimes;
};

/*######
 ## at_frostgrips_hollow
 ######*/

enum FrostgripsHollow
{
    QUEST_THE_LONESOME_WATCHER      = 12877,

    NPC_STORMFORGED_MONITOR         = 29862,
    NPC_STORMFORGED_ERADICTOR       = 29861,

    TYPE_WAYPOINT                   = 0,
    DATA_START                      = 0
};

Position const stormforgedMonitorPosition = {6963.95f, 45.65f, 818.71f, 4.948f};
Position const stormforgedEradictorPosition = {6983.18f, 7.15f, 806.33f, 2.228f};

class AreaTrigger_at_frostgrips_hollow : public AreaTriggerScript
{
public:
    AreaTrigger_at_frostgrips_hollow() : AreaTriggerScript("at_frostgrips_hollow")
    {
        stormforgedMonitorGUID.Clear();
        stormforgedEradictorGUID.Clear();
    }

    bool OnTrigger(Player* player, AreaTriggerEntry const* /*areaTrigger*/) override
    {
        if (player->GetQuestStatus(QUEST_THE_LONESOME_WATCHER) != QUEST_STATUS_INCOMPLETE)
            return false;

        Creature* stormforgedMonitor = ObjectAccessor::GetCreature(*player, stormforgedMonitorGUID);
        if (stormforgedMonitor)
            return false;

        Creature* stormforgedEradictor = ObjectAccessor::GetCreature(*player, stormforgedEradictorGUID);
        if (stormforgedEradictor)
            return false;

        stormforgedMonitor = player->SummonCreature(NPC_STORMFORGED_MONITOR, stormforgedMonitorPosition, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 1min);
        if (stormforgedMonitor)
        {
            stormforgedMonitorGUID = stormforgedMonitor->GetGUID();
            stormforgedMonitor->SetWalk(false);
            /// The npc would search an alternative way to get to the last waypoint without this unit state.
            stormforgedMonitor->AddUnitState(UNIT_STATE_IGNORE_PATHFINDING);
            stormforgedMonitor->GetMotionMaster()->MovePath((NPC_STORMFORGED_MONITOR * 100) << 3, false);
        }

        stormforgedEradictor = player->SummonCreature(NPC_STORMFORGED_ERADICTOR, stormforgedEradictorPosition, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 1min);
        if (stormforgedEradictor)
        {
            stormforgedEradictorGUID = stormforgedEradictor->GetGUID();
            stormforgedEradictor->GetMotionMaster()->MovePath((NPC_STORMFORGED_ERADICTOR * 100) << 3, false);
        }

        return true;
    }

private:
    ObjectGuid stormforgedMonitorGUID;
    ObjectGuid stormforgedEradictorGUID;
};

struct areatrigger_stormwind_teleport_unit : AreaTriggerAI
{
    enum MiscIds
    {
        SPELL_DUST_IN_THE_STORMWIND        = 312593,

        NPC_KILL_CREDIT_TELEPORT_STORMWIND = 160561
    };

    areatrigger_stormwind_teleport_unit(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger) { }

    void OnUnitEnter(Unit* unit) override
    {
        Player* player = unit->ToPlayer();
        if (!player)
            return;

        player->CastSpell(unit, SPELL_DUST_IN_THE_STORMWIND);
        player->KilledMonsterCredit(NPC_KILL_CREDIT_TELEPORT_STORMWIND);
    }
};

void HandleBuffAreaTrigger(Player* player)
{
    if (GameObject* buffObject = player->FindNearestGameObjectWithOptions(4.0f, { .StringId = "bg_buff_object" }))
    {
        buffObject->ActivateObject(GameObjectActions::Disturb, 0, player);
        buffObject->DespawnOrUnsummon();
    }
}

struct areatrigger_battleground_buffs : AreaTriggerAI
{
    areatrigger_battleground_buffs(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger) { }

    void OnUnitEnter(Unit* unit) override
    {
        if (!unit->IsPlayer())
            return;

        HandleBuffAreaTrigger(unit->ToPlayer());
    }
};

class AreaTrigger_at_battleground_buffs : public AreaTriggerScript
{
public:
    AreaTrigger_at_battleground_buffs() : AreaTriggerScript("at_battleground_buffs") { }

    bool OnTrigger(Player* player, AreaTriggerEntry const* /*areaTrigger*/) override
    {
        HandleBuffAreaTrigger(player);
        return true;
    }
};

struct areatrigger_action_capture_flag : AreaTriggerAI
{
    areatrigger_action_capture_flag(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger) { }

    void OnUnitEnter(Unit* unit) override
    {
        if (!unit->IsPlayer())
            return;

        Player* player = unit->ToPlayer();
        if (ZoneScript* zoneScript = at->GetZoneScript())
            if (zoneScript->CanCaptureFlag(at, player))
                zoneScript->OnCaptureFlag(at, player);
    }
};

// 18235 - Void Orb
struct at_void_orb_harbinger : AreaTriggerAI
{
    at_void_orb_harbinger(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger) { }

    enum Spells
    {
        SPELL_VOID_ORB_DAMAGE = 273502,
    };

    void OnInitialize() override
    {
        if (Unit* caster = at->GetCaster())
        {
            at->SetOrientation(caster->GetOrientation());

            Position destPos = caster->GetPosition();
            at->MovePositionToFirstCollision(destPos, 35.0f, 0.0f);

            PathGenerator path(at);
            path.CalculatePath(destPos.GetPositionX(), destPos.GetPositionY(), destPos.GetPositionZ(), false);

            at->InitSplines(path.GetPath());
        }
    }

    void OnDestinationReached() override
    {
        at->Remove();
    }

    void OnUnitEnter(Unit* unit) override
    {
        Unit* caster = at->GetCaster();
        if (!caster)
            return;

        if (caster->IsFriendlyTo(unit))
            return;

        caster->CastSpell(unit, SPELL_VOID_ORB_DAMAGE);
    }
};

// 18242 - Abyssal Portal
struct at_abyssal_portal_harbinger : AreaTriggerAI
{
    at_abyssal_portal_harbinger(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger), _remainingSummons(0) { }

    enum Spells
    {
        SPELL_ABYSSAL_PORTAL_SUMMON = 273587
    };

    void OnCreate(Spell const* creatingSpell) override
    {
        if (Unit* caster = at->GetCaster())
            _remainingSummons = creatingSpell->GetSpellInfo()->GetEffect(EFFECT_0).CalcValue(caster);

        _scheduler.Schedule(500ms, [this](TaskContext task)
        {
            if (Unit* caster = at->GetCaster())
                caster->CastSpell(at->GetRandomNearPosition(3.0f), SPELL_ABYSSAL_PORTAL_SUMMON, true);

            _remainingSummons--;
            if (_remainingSummons > 0)
                task.Repeat(1s);
        });
    }

    void OnUpdate(uint32 diff) override
    {
        _scheduler.Update(diff);
    }

private:
    TaskScheduler _scheduler;
    uint8 _remainingSummons;
};

void AddSC_areatrigger_scripts()
{
    new AreaTrigger_at_coilfang_waterfall();
    new AreaTrigger_at_legion_teleporter();
    new AreaTrigger_at_scent_larkorwi();
    new AreaTrigger_at_sholazar_waygate();
    new AreaTrigger_at_nats_landing();
    new AreaTrigger_at_brewfest();
    new AreaTrigger_at_area_52_entrance();
    new AreaTrigger_at_frostgrips_hollow();
    RegisterAreaTriggerAI(areatrigger_stormwind_teleport_unit);
    RegisterAreaTriggerAI(areatrigger_battleground_buffs);
    new AreaTrigger_at_battleground_buffs();
    RegisterAreaTriggerAI(areatrigger_action_capture_flag);
    RegisterAreaTriggerAI(at_void_orb_harbinger);
    RegisterAreaTriggerAI(at_abyssal_portal_harbinger);
}
