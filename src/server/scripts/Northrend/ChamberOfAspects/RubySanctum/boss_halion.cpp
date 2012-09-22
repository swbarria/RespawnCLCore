/*
 * Copyright (C) 2008-2011 TrinityCore <http://www.trinitycore.org/>
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

#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "Spell.h"
#include "Vehicle.h"
#include "MapManager.h"
#include "GameObjectAI.h"
#include "ruby_sanctum.h"

/*
TODO:
- See if corporeality code can be improved
- Script adds and surrounding trash
- Update Texts
*/

enum Texts
{
    SAY_INTRO                          = 0, // Meddlesome insects! You are too late. The Ruby Sanctum is lost!
    SAY_AGGRO                          = 1, // Your world teeters on the brink of annihilation. You will ALL bear witness to the coming of a new age of DESTRUCTION!
    SAY_METEOR_STRIKE                  = 2, // The heavens burn!
    SAY_PHASE_TWO                      = 3, // You will find only suffering within the realm of twilight! Enter if you dare!
    SAY_DEATH                          = 4, // Relish this victory, mortals, for it will be your last! This world will burn with the master's return!
    SAY_KILL                           = 5, // Another "hero" falls.
    SAY_BERSERK                        = 6, // Not good enough.
    EMOTE_HALION_OUT_PHYSICAL          = 7, // Your efforts force %s further out of the physical realm!
    EMOTE_HALION_IN_PHYSICAL           = 8, // Your companions'' efforts force %s further into the physical realm!

    SAY_SPHERE_PULSE                   = 0, // Beware the shadow!
    SAY_PHASE_THREE                    = 1, // I am the light and the darkness! Cower, mortals, before the herald of Deathwing!
    EMOTE_TWILIGHT_HALION_IN_TWILIGHT  = 2, // Your companions'' efforts force %s further into the twilight realm!
    EMOTE_TWILIGHT_HALION_OUT_TWILIGHT = 3, // Your efforts force %s further out of the twilight realm!

    EMOTE_WARN_LASER                   = 0, // The orbiting spheres pulse with dark energy!
};

enum Spells
{
    // Halion
    SPELL_FLAME_BREATH                  = 74525,
    SPELL_CLEAVE                        = 74524,
    SPELL_METEOR_STRIKE                 = 74637,
    SPELL_TAIL_LASH                     = 74531,

    SPELL_FIERY_COMBUSTION              = 74562,
    SPELL_MARK_OF_COMBUSTION            = 74567,
    SPELL_FIERY_COMBUSTION_EXPLOSION    = 74607,
    SPELL_FIERY_COMBUSTION_SUMMON       = 74610,

    // Combustion & Consumption
    SPELL_SCALE_AURA                    = 70507, // Aura created in spell_dbc.
    SPELL_COMBUSTION_DAMAGE_AURA        = 74629,
    SPELL_CONSUMPTION_DAMAGE_AURA       = 74803,

    // Twilight Halion
    SPELL_DARK_BREATH                   = 74806,

    SPELL_MARK_OF_CONSUMPTION           = 74795,
    SPELL_SOUL_CONSUMPTION              = 74792,
    SPELL_SOUL_CONSUMPTION_EXPLOSION    = 74799,
    SPELL_SOUL_CONSUMPTION_SUMMON       = 74800,

    // Living Inferno
    SPELL_BLAZING_AURA                  = 75885,

    // Halion Controller
    SPELL_COSMETIC_FIRE_PILLAR          = 76006,
    SPELL_FIERY_EXPLOSION               = 76010,
    SPELL_CLEAR_DEBUFFS                 = 75396,

    // Meteor Strike
    SPELL_METEOR_STRIKE_COUNTDOWN       = 74641,
    SPELL_METEOR_STRIKE_AOE_DAMAGE      = 74648,
    SPELL_METEOR_STRIKE_FIRE_AURA_1     = 74713,
    SPELL_METEOR_STRIKE_FIRE_AURA_2     = 74718,
    SPELL_BIRTH_NO_VISUAL               = 40031,

    // Shadow Orb
    SPELL_TWILIGHT_CUTTER               = 74768, // Unknown dummy effect (EFFECT_0)
    SPELL_TWILIGHT_CUTTER_TRIGGERED     = 74769,
    SPELL_TWILIGHT_PULSE_PERIODIC       = 78861,
    SPELL_TRACK_ROTATION                = 74758,

    // Misc
    SPELL_TWILIGHT_DIVISION             = 75063, // Phase spell from phase 2 to phase 3
    SPELL_LEAVE_TWILIGHT_REALM          = 74812,
    SPELL_TWILIGHT_PHASING              = 74808, // Phase spell from phase 1 to phase 2
    SPELL_SUMMON_TWILIGHT_PORTAL        = 74809, // Summons go 202794
    SPELL_SUMMON_EXIT_PORTALS           = 74805, // Custom spell created in spell_dbc.
    SPELL_TWILIGHT_MENDING              = 75509,
    SPELL_TWILIGHT_REALM                = 74807,

    SPELL_COPY_DAMAGE                   = 74810, // Not in DBCs but found in sniffs. Not cast.
};

enum Events
{
    // Halion
    EVENT_ACTIVATE_FIREWALL     = 1,
    EVENT_CLEAVE                = 2,
    EVENT_FLAME_BREATH          = 3,
    EVENT_METEOR_STRIKE         = 4,
    EVENT_FIERY_COMBUSTION      = 5,
    EVENT_TAIL_LASH             = 6,

    // Twilight Halion
    EVENT_DARK_BREATH           = 7,
    EVENT_SOUL_CONSUMPTION      = 8,

    // Meteor Strike
    EVENT_SPAWN_METEOR_FLAME    = 9,

    // Halion Controller
    EVENT_START_INTRO           = 10,
    EVENT_INTRO_PROGRESS_1      = 11,
    EVENT_INTRO_PROGRESS_2      = 12,
    EVENT_INTRO_PROGRESS_3      = 13,
    EVENT_CHECK_CORPOREALITY    = 14,
    EVENT_SHADOW_PULSARS_SHOOT  = 15,
    EVENT_TRIGGER_BERSERK       = 16,
    EVENT_TWILIGHT_MENDING      = 17,
};

enum Actions
{
    // Meteor Strike
    ACTION_METEOR_STRIKE_BURN   = 1,
    ACTION_METEOR_STRIKE_AOE    = 2,

    // Halion Controller
    ACTION_PHASE_TWO            = 3,
    ACTION_PHASE_THREE          = 4,
    ACTION_CLEANUP              = 5,

    // Orb Carrier
    ACTION_SHOOT                = 6,
};

enum Phases
{
    PHASE_ALL           = 0,
    PHASE_ONE           = 1,
    PHASE_TWO           = 2,
    PHASE_THREE         = 3,

    PHASE_ONE_MASK      = 1 << PHASE_ONE,
    PHASE_TWO_MASK      = 1 << PHASE_TWO,
    PHASE_THREE_MASK    = 1 << PHASE_THREE,

    PHASE_MASK_NO_EVADE_CHECK = PHASE_ONE_MASK,
};

enum Misc
{
    DATA_TWILIGHT_DAMAGE_TAKEN   = 1,
    DATA_MATERIAL_DAMAGE_TAKEN   = 2,
    DATA_STACKS_DISPELLED        = 3,
    DATA_FIGHT_PHASE             = 4,
};

enum OrbCarrierSeats
{
    SEAT_NORTH            = 0,
    SEAT_SOUTH            = 1,
    SEAT_EAST             = 2,
    SEAT_WEST             = 3,
};

Position const HalionSpawnPos = {3156.67f, 533.8108f, 72.98822f, 3.159046f};

uint8 const MAX_CORPOREALITY_STATE = 11;

struct CorporealityEntry
{
    uint8 materialPercentage;
    uint32 materialRealmSpell;
    uint32 twilightRealmSpell;
};

CorporealityEntry _corporealityReference[MAX_CORPOREALITY_STATE] = {
    {  0, 74836, 74831},
    { 10, 74835, 74830},
    { 20, 74834, 74829},
    { 30, 74833, 74828},
    { 40, 74832, 74827},
    { 50, 74826, 74826},
    { 60, 74827, 74832},
    { 70, 74828, 74833},
    { 80, 74829, 74834},
    { 90, 74830, 74835},
    {100, 74831, 74836}
};

struct generic_halionAI : public BossAI
{
    generic_halionAI(Creature* creature, uint32 bossId) : BossAI(creature, bossId) { }

    void EnterCombat(Unit* /*who*/)
    {
        Talk(SAY_AGGRO);
        _EnterCombat();
        events.Reset();
        events.ScheduleEvent(EVENT_CLEAVE, urand(8000, 10000));
    }

    void ExecuteEvent(uint32 const eventId)
    {
        switch (eventId)
        {
            case EVENT_CLEAVE:
                DoCastVictim(SPELL_CLEAVE);
                events.ScheduleEvent(EVENT_CLEAVE, urand(8000, 10000));
                break;
        }
    }

    void UpdateAI(uint32 const diff)
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
            ExecuteEvent(eventId);

        DoMeleeAttackIfReady();
    }

    bool DealDamageToOtherHalion(uint64 guid, uint32 schoolMask, uint32 damage)
    {
        Creature* otherHalion = ObjectAccessor::GetCreature(*me, guid);
        if (!otherHalion)
            return false;

        SpellNonMeleeDamage damageInfo(me, otherHalion, SPELL_COPY_DAMAGE, schoolMask);
        damageInfo.damage = damage;
        me->SendSpellNonMeleeDamageLog(&damageInfo);
        me->DealSpellDamage(&damageInfo, false);
        return true;
    }
};

class boss_halion : public CreatureScript
{
    public:
        boss_halion() : CreatureScript("boss_halion") { }

        struct boss_halionAI : public generic_halionAI
        {
            boss_halionAI(Creature* creature) : generic_halionAI(creature, DATA_HALION) { }

            void Reset()
            {
                generic_halionAI::Reset();
                events.SetPhase(PHASE_ONE);

                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                me->RemoveAurasDueToSpell(SPELL_TWILIGHT_PHASING);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }

            void EnterCombat(Unit* who)
            {
                generic_halionAI::EnterCombat(who);

                instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me, 1);
                instance->SetBossState(DATA_HALION, IN_PROGRESS);

                events.SetPhase(PHASE_ONE);
                events.ScheduleEvent(EVENT_ACTIVATE_FIREWALL, 10000);
                events.ScheduleEvent(EVENT_FLAME_BREATH, urand(10000, 12000));
                events.ScheduleEvent(EVENT_METEOR_STRIKE, urand(20000, 25000));
                events.ScheduleEvent(EVENT_FIERY_COMBUSTION, urand(15000, 18000));
                events.ScheduleEvent(EVENT_TAIL_LASH, 10000);

                if (Creature* controller = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_HALION_CONTROLLER)))
                    controller->AI()->SetData(DATA_FIGHT_PHASE, PHASE_ONE);
            }

            void JustDied(Unit* /*killer*/)
            {
                _JustDied();

                Talk(SAY_DEATH);
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

                if (Creature* controller = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_HALION_CONTROLLER)))
                    me->Kill(controller);
            }

            void JustReachedHome()
            {
                _JustReachedHome();
                if (Creature* controller = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_HALION_CONTROLLER)))
                    controller->AI()->Reset();
            }

            Position const* GetMeteorStrikePosition() const { return &_meteorStrikePos; }

            void DamageTaken(Unit* /*attacker*/, uint32& damage, SpellInfo const* spellInfo)
            {
                if (me->HealthBelowPctDamaged(75, damage) && (events.GetPhaseMask() & PHASE_ONE_MASK))
                {
                    events.SetPhase(PHASE_TWO);
                    events.DelayEvents(2600); // 2.5 sec + 0.1 sec lag

                    Talk(SAY_PHASE_TWO);
                    DoCast(me, SPELL_TWILIGHT_PHASING);

                    if (Creature* controller = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_HALION_CONTROLLER)))
                        controller->AI()->SetData(DATA_FIGHT_PHASE, PHASE_TWO);

                    return;
                }

                if (!spellInfo || spellInfo->Id != SPELL_COPY_DAMAGE)
                {
                    if (!DealDamageToOtherHalion(instance->GetData64(DATA_TWILIGHT_HALION), spellInfo ? spellInfo->SchoolMask : SPELL_SCHOOL_MASK_SHADOW, damage))
                        return;

                    // Keep track of damage taken
                    if (events.GetPhaseMask() & PHASE_THREE_MASK)
                        if (Creature* controller = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_HALION_CONTROLLER)))
                            controller->AI()->SetData(DATA_MATERIAL_DAMAGE_TAKEN, damage);
                }
            }

            void UpdateAI(uint32 const diff)
            {
                if (events.GetPhaseMask() & PHASE_TWO_MASK)
                    return;

                generic_halionAI::UpdateAI(diff);
            }

            void ExecuteEvent(uint32 const eventId)
            {
                switch (eventId)
                {
                    case EVENT_ACTIVATE_FIREWALL:
                    {
                        // Flame ring is activated 10 seconds after starting encounter, DOOR_TYPE_ROOM is only instant.
                        if (GameObject* flameRing = ObjectAccessor::GetGameObject(*me, instance->GetData64(DATA_FLAME_RING)))
                            instance->HandleGameObject(instance->GetData64(DATA_FLAME_RING), false, flameRing);

                        if (GameObject* flameRing = ObjectAccessor::GetGameObject(*me, instance->GetData64(DATA_TWILIGHT_FLAME_RING)))
                            instance->HandleGameObject(instance->GetData64(DATA_TWILIGHT_FLAME_RING), false, flameRing);
                        break;
                    }
                    case EVENT_FLAME_BREATH:
                        DoCast(me, SPELL_FLAME_BREATH);
                        events.ScheduleEvent(EVENT_FLAME_BREATH, 25000);
                        break;
                    case EVENT_TAIL_LASH:
                        DoCastAOE(SPELL_TAIL_LASH);
                        events.ScheduleEvent(EVENT_TAIL_LASH, 10000);
                        break;
                    case EVENT_METEOR_STRIKE:
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true, -SPELL_TWILIGHT_REALM))
                        {
                            target->GetPosition(&_meteorStrikePos);
                            me->CastSpell(_meteorStrikePos.GetPositionX(), _meteorStrikePos.GetPositionY(), _meteorStrikePos.GetPositionZ(), SPELL_METEOR_STRIKE, true, NULL, NULL, me->GetGUID());
                            Talk(SAY_METEOR_STRIKE);
                        }
                        events.ScheduleEvent(EVENT_METEOR_STRIKE, 40000);
                        break;
                    }
                    case EVENT_FIERY_COMBUSTION:
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 0.0f, true, -SPELL_TWILIGHT_REALM))
                            DoCast(target, SPELL_FIERY_COMBUSTION);
                        events.ScheduleEvent(EVENT_FIERY_COMBUSTION, 25000);
                        break;
                    }
                    default:
                        generic_halionAI::ExecuteEvent(eventId);
                        break;
                }
            }

            void SetData(uint32 index, uint32 value)
            {
                if (index == DATA_FIGHT_PHASE)
                    events.SetPhase(value);
            }

            uint32 GetData(uint32 index)
            {
                if (index == DATA_FIGHT_PHASE)
                    return events.GetPhaseMask();

                return 0;
            }

        private:
            Position _meteorStrikePos;

        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetRubySanctumAI<boss_halionAI>(creature);
        }
};

typedef boss_halion::boss_halionAI HalionAI;

class boss_twilight_halion : public CreatureScript
{
    public:
        boss_twilight_halion() : CreatureScript("boss_twilight_halion") { }

        struct boss_twilight_halionAI : public generic_halionAI
        {
            boss_twilight_halionAI(Creature* creature) : generic_halionAI(creature, DATA_TWILIGHT_HALION)
            {
                me->SetPhaseMask(0x20, true);
                events.SetPhase(PHASE_ONE);

                me->SetReactState(REACT_PASSIVE);
            }

            void Reset()
            {
                generic_halionAI::Reset();

                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                me->LoadCreaturesAddon(true);
                me->SetReactState(REACT_PASSIVE);
            }

            void KilledUnit(Unit* victim)
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    Talk(SAY_KILL);

                // Victims should not be in the Twilight Realm
                me->CastSpell(victim, SPELL_LEAVE_TWILIGHT_REALM, true);
            }

            void JustDied(Unit* killer)
            {
                if (Creature* halion = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_HALION)))
                {
                    // Ensure looting
                    if (me->IsDamageEnoughForLootingAndReward())
                        halion->LowerPlayerDamageReq(halion->GetMaxHealth());

                    if (halion->isAlive())
                        killer->Kill(halion);
                }

                if (Creature* controller = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_HALION_CONTROLLER)))
                    controller->Kill(controller);

                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage, SpellInfo const* spellInfo)
            {
                if (me->HealthBelowPctDamaged(50, damage) && (events.GetPhaseMask() & PHASE_TWO_MASK))
                {
                    events.SetPhase(PHASE_THREE);
                    events.DelayEvents(2600); // 2.5 sec + 0.1sec lag

                    me->CastStop();
                    DoCast(me, SPELL_TWILIGHT_DIVISION);
                    Talk(SAY_PHASE_THREE);
                    return;
                }

                if (!spellInfo || spellInfo->Id != SPELL_COPY_DAMAGE)
                {
                    if (!DealDamageToOtherHalion(instance->GetData64(DATA_HALION), spellInfo ? spellInfo->SchoolMask : SPELL_SCHOOL_MASK_SHADOW, damage))
                        return;

                    // Keep track of damage taken.
                    if (events.GetPhaseMask() & PHASE_THREE_MASK)
                        if (Creature* controller = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_HALION_CONTROLLER)))
                            controller->AI()->SetData(DATA_TWILIGHT_DAMAGE_TAKEN, damage);
                }
            }

            void SpellHit(Unit* /*who*/, SpellInfo const* spell)
            {
                if (spell->Id != SPELL_TWILIGHT_DIVISION)
                    return;

                if (Creature* controller = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_HALION_CONTROLLER)))
                    controller->AI()->SetData(DATA_FIGHT_PHASE, PHASE_THREE);
            }

            void ExecuteEvent(uint32 const eventId)
            {
                switch (eventId)
                {
                    case EVENT_DARK_BREATH:
                        DoCast(me, SPELL_DARK_BREATH);
                        events.ScheduleEvent(EVENT_DARK_BREATH, urand(10000, 15000));
                        break;
                    case EVENT_SOUL_CONSUMPTION:
                    {
                        Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 0.0f, true, SPELL_TWILIGHT_REALM);
                        if (!target)
                            target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true, SPELL_TWILIGHT_REALM);
                        if (target)
                            DoCast(target, SPELL_SOUL_CONSUMPTION);
                        events.ScheduleEvent(EVENT_SOUL_CONSUMPTION, 20000);
                        break;
                    }
                    case EVENT_TAIL_LASH:
                        DoCastAOE(SPELL_TAIL_LASH);
                        events.ScheduleEvent(EVENT_TAIL_LASH, 10000);
                        break;
                    default:
                        generic_halionAI::ExecuteEvent(eventId);
                        break;
                }
            }

            void SetData(uint32 index, uint32 value)
            {
                if (index == DATA_FIGHT_PHASE)
                {
                    events.SetPhase(value);
                    // Bind events starting from phase two ONLY
                    if (value == PHASE_TWO)
                    {
                        instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me, 2);

                        me->SetReactState(REACT_AGGRESSIVE);
                        
                        events.Reset();
                        events.ScheduleEvent(EVENT_DARK_BREATH, urand(10000, 15000));
                        events.ScheduleEvent(EVENT_SOUL_CONSUMPTION, 20000);
                        events.ScheduleEvent(EVENT_TAIL_LASH, 10000);
                    }
                }
            }

            uint32 GetData(uint32 index)
            {
                if (index == DATA_FIGHT_PHASE)
                    return events.GetPhaseMask();

                return 0;
            }

        private:
            EventMap events;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetRubySanctumAI<boss_twilight_halionAI>(creature);
        }
};

typedef boss_twilight_halion::boss_twilight_halionAI twilightHalionAI;

class npc_halion_controller : public CreatureScript
{
    public:
        npc_halion_controller() : CreatureScript("npc_halion_controller") { }

        struct npc_halion_controllerAI : public ScriptedAI
        {
            npc_halion_controllerAI(Creature* creature) : ScriptedAI(creature),
                _instance(creature->GetInstanceScript()), _summons(me), _playingIntro(true), _forceReset(false)
            {
                me->SetPhaseMask(me->GetPhaseMask() | 0x20, true);
            }

            void Reset()
            {
                me->SetReactState(REACT_PASSIVE);

                _summons.DespawnAll();
                _events.Reset();

                DoCast(me, SPELL_CLEAR_DEBUFFS);
            }

            void JustSummoned(Creature* who)
            {
                _summons.Summon(who);
            }

            void JustDied(Unit* /*killer*/)
            {
                _events.Reset();
                _summons.DespawnAll();

                DoCast(me, SPELL_CLEAR_DEBUFFS);
            }

            void DoAction(int32 const action)
            {
                switch (action)
                {
                    case ACTION_INTRO_HALION:
                        _events.Reset();
                        _events.ScheduleEvent(EVENT_START_INTRO, 2000);
                        break;
                    default:
                        break;
                }
            }

            void EnterEvadeMode()
            {
                ScriptedAI::EnterEvadeMode();
                for (uint8 i = DATA_HALION; i <= DATA_TWILIGHT_HALION; i++)
                    if (Creature* halion = ObjectAccessor::GetCreature(*me, _instance->GetData64(i)))
                        if (!halion->IsInEvadeMode())
                            halion->AI()->EnterEvadeMode();
                _events.ScheduleEvent(EVENT_TWILIGHT_MENDING, 1000);
            }

            void UpdateAI(uint32 const diff)
            {
                if (!_playingIntro && !UpdateVictim())
                {
                    // No possible targets were found, means wipe.
                    EnterEvadeMode();
                    return;
                }

                _events.Update(diff);

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_START_INTRO:
                            DoCast(me, SPELL_COSMETIC_FIRE_PILLAR, true);
                            _events.ScheduleEvent(EVENT_INTRO_PROGRESS_1, 4000);
                            break;
                        case EVENT_INTRO_PROGRESS_1:
                            for (uint8 i = DATA_BURNING_TREE_3; i <= DATA_BURNING_TREE_4; ++i)
                                if (GameObject* tree = ObjectAccessor::GetGameObject(*me, _instance->GetData64(i)))
                                    _instance->HandleGameObject(_instance->GetData64(i), true, tree);
                            _events.ScheduleEvent(EVENT_INTRO_PROGRESS_2, 4000);
                            break;
                        case EVENT_INTRO_PROGRESS_2:
                            for (uint8 i = DATA_BURNING_TREE_1; i <= DATA_BURNING_TREE_2; ++i)
                                if (GameObject* tree = ObjectAccessor::GetGameObject(*me, _instance->GetData64(i)))
                                    _instance->HandleGameObject(_instance->GetData64(i), true, tree);
                            _events.ScheduleEvent(EVENT_INTRO_PROGRESS_3, 4000);
                            break;
                        case EVENT_INTRO_PROGRESS_3:
                        {
                            DoCast(me, SPELL_FIERY_EXPLOSION);
                            Creature* halion = me->GetMap()->SummonCreature(NPC_HALION, HalionSpawnPos);
                            Creature* twilightHalion = me->GetMap()->SummonCreature(NPC_TWILIGHT_HALION, HalionSpawnPos);
                            if (halion && twilightHalion)
                                halion->AI()->Talk(SAY_INTRO);

                            _playingIntro = false;
                            break;
                        }
                        case EVENT_TRIGGER_BERSERK:
                            for (uint8 i = DATA_HALION; i <= DATA_TWILIGHT_HALION; i++)
                                if (Creature* halion = ObjectAccessor::GetCreature(*me, _instance->GetData64(i)))
                                    halion->CastSpell(halion, SPELL_BERSERK, true);
                            break;
                        case EVENT_SHADOW_PULSARS_SHOOT:
                            if (Creature* twilightHalion = ObjectAccessor::GetCreature(*me, _instance->GetData64(DATA_TWILIGHT_HALION)))
                                twilightHalion->AI()->Talk(SAY_SPHERE_PULSE);

                            if (Creature* orbCarrier = ObjectAccessor::GetCreature(*me, _instance->GetData64(DATA_ORB_CARRIER)))
                                orbCarrier->AI()->DoAction(ACTION_SHOOT);

                            _events.ScheduleEvent(EVENT_SHADOW_PULSARS_SHOOT, 29000);   // 9 sec channel duration, every 20th second
                            break;
                        case EVENT_TWILIGHT_MENDING:
                            bool repeatEvent = true;
                            for (uint8 i = DATA_HALION; i <= DATA_TWILIGHT_HALION; ++i)
                            {
                                if (Creature* halion = ObjectAccessor::GetCreature(*me, _instance->GetData64(i)))
                                {
                                    DoCast(halion, SPELL_TWILIGHT_MENDING, true);
                                    if (i == DATA_TWILIGHT_HALION && halion->GetHealth() == halion->GetMaxHealth())
                                        repeatEvent = false;
                                }
                            }
                            if (repeatEvent)
                                _events.ScheduleEvent(EVENT_TWILIGHT_MENDING, 1000);
                            break;
                        default:
                            break;
                    }
                }
            }

            void SetData(uint32 id, uint32 value)
            {
                switch (id)
                {
                    case DATA_MATERIAL_DAMAGE_TAKEN:
                        MaterialDamageTaken += value;
                        UpdateCorporeality();
                        break;
                    case DATA_TWILIGHT_DAMAGE_TAKEN:
                        TwilightDamageTaken += value;
                        UpdateCorporeality();
                        break;
                    case DATA_FIGHT_PHASE:
                        switch (value)
                        {
                            case PHASE_ONE:
                                DoZoneInCombat();
                                if (Creature* twilightHalion = ObjectAccessor::GetCreature(*me, _instance->GetData64(DATA_TWILIGHT_HALION)))
                                    DoZoneInCombat(twilightHalion);
                                _events.ScheduleEvent(EVENT_TRIGGER_BERSERK, 8 * MINUTE * IN_MILLISECONDS);
                                break;
                            case PHASE_TWO:
                                // Timer taken from a 4.3.4 solo video and confirmed by TankSpot's 3.3.5 guide.
                                // http://www.tankspot.com/showthread.php?67195-Halion-Encounter-Guide-Live
                                _events.ScheduleEvent(EVENT_SHADOW_PULSARS_SHOOT, 29000);
                                if (Creature* twilightHalion = ObjectAccessor::GetCreature(*me, _instance->GetData64(DATA_TWILIGHT_HALION)))
                                    twilightHalion->AI()->SetData(DATA_FIGHT_PHASE, PHASE_TWO);
                                break;
                            case PHASE_THREE:
                            {
                                _events.ScheduleEvent(EVENT_CHECK_CORPOREALITY, 20000);

                                TwilightDamageTaken = 0;
                                MaterialDamageTaken = 0;
                                materialCorporealityValue = 50;

                                for (uint8 itr = DATA_HALION; itr <= DATA_TWILIGHT_HALION; itr++)
                                {
                                    Creature* halion = ObjectAccessor::GetCreature(*me, _instance->GetData64(itr));
                                    if (!halion)
                                        continue;

                                    uint32 spellID = GetSpell(materialCorporealityValue, (itr == DATA_TWILIGHT_HALION));
                                    if (spellID != 0)
                                        halion->CastSpell(halion, spellID, false);

                                    halion->AI()->SetData(DATA_FIGHT_PHASE, PHASE_THREE);

                                    if (itr == DATA_TWILIGHT_HALION)
                                        continue;

                                    halion->RemoveAurasDueToSpell(SPELL_TWILIGHT_PHASING);
                                    halion->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                }

                                // Summon Twilight portals here
                                DoCast(me, SPELL_SUMMON_EXIT_PORTALS);

                                _instance->DoUpdateWorldState(WORLDSTATE_CORPOREALITY_TOGGLE, 1);
                                _instance->DoUpdateWorldState(WORLDSTATE_CORPOREALITY_MATERIAL, 50);
                                _instance->DoUpdateWorldState(WORLDSTATE_CORPOREALITY_TWILIGHT, 50);
                                break;
                            }
                            default:
                                break;
                        }
                        break;
                    default:
                        break;
                }
            }

        private:
            void UpdateCorporeality()
            {
                uint8 oldValue = materialCorporealityValue;
                if (MaterialDamageTaken > 1.02f * TwilightDamageTaken)
                {
                    if (materialCorporealityValue > 0)
                        materialCorporealityValue -= 10;
                }
                else if (TwilightDamageTaken > 1.0f * MaterialDamageTaken)
                {
                    if (materialCorporealityValue < 100)
                        materialCorporealityValue += 10;
                }

                _instance->DoUpdateWorldState(WORLDSTATE_CORPOREALITY_MATERIAL, materialCorporealityValue);
                _instance->DoUpdateWorldState(WORLDSTATE_CORPOREALITY_TWILIGHT, 100 - materialCorporealityValue);

                for (uint8 itr = DATA_HALION; itr <= DATA_TWILIGHT_HALION; itr++)
                {
                    if (Creature* halion = ObjectAccessor::GetCreature(*me, _instance->GetData64(itr)))
                    {
                        if (uint32 spellID = GetSpell(materialCorporealityValue, itr == DATA_TWILIGHT_HALION))
                        {
                            RemoveCorporeality(halion, itr == DATA_TWILIGHT_HALION);
                            halion->CastSpell(halion, spellID, true);
                        }

                        if (itr == DATA_TWILIGHT_HALION)
                            halion->AI()->Talk(oldValue > materialCorporealityValue ? EMOTE_TWILIGHT_HALION_OUT_TWILIGHT : EMOTE_TWILIGHT_HALION_IN_TWILIGHT, halion->GetGUID());
                        else // if (itr == DATA_HALION)
                            halion->AI()->Talk(oldValue > materialCorporealityValue ? EMOTE_HALION_OUT_PHYSICAL: EMOTE_HALION_IN_PHYSICAL, halion->GetGUID());
                    }
                }
            }

            void RemoveCorporeality(Creature* who, bool isTwilight = false)
            {
                for (uint8 i = 0; i < MAX_CORPOREALITY_STATE; i++)
                {
                    uint32 spellID = (isTwilight ? _corporealityReference[i].twilightRealmSpell : _corporealityReference[i].materialRealmSpell);
                    if (who->HasAura(spellID))
                    {
                        who->RemoveAurasDueToSpell(spellID);
                        break;
                    }
                }
            }

            uint32 GetSpell(uint32 pctValue, bool isTwilight = false) const
            {
                for (uint8 i = 0; i < MAX_CORPOREALITY_STATE; i++)
                    if (_corporealityReference[i].materialPercentage == pctValue)
                        return (isTwilight ? _corporealityReference[i].twilightRealmSpell : _corporealityReference[i].materialRealmSpell);
                return 0;
            }

            EventMap _events;
            InstanceScript* _instance;
            SummonList _summons;

            bool _playingIntro;
            bool _corporealityCheck;
            bool _forceReset;

            uint32 TwilightDamageTaken;
            uint32 MaterialDamageTaken;
            uint8 materialCorporealityValue;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetRubySanctumAI<npc_halion_controllerAI>(creature);
        }
};

typedef npc_halion_controller::npc_halion_controllerAI controllerAI;

class npc_orb_carrier : public CreatureScript
{
    public:
        npc_orb_carrier() : CreatureScript("npc_orb_carrier") { }

        struct npc_orb_carrierAI : public ScriptedAI
        {
            npc_orb_carrierAI(Creature* creature) : ScriptedAI(creature),
                instance(creature->GetInstanceScript())
            {
                ASSERT(creature->GetVehicleKit());
            }

            void UpdateAI(uint32 const /*diff*/)
            {
                /// According to sniffs this spell is cast every 1 or 2 seconds.
                /// However, refreshing it looks bad, so just cast the spell if
                /// we are not channeling it.
                if (!me->HasUnitState(UNIT_STATE_CASTING))
                    me->CastSpell((Unit*)NULL, SPELL_TRACK_ROTATION, false);

                if (Creature* rotationFocus = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_ORB_ROTATION_FOCUS)))
                    me->UpdateOrientation(me->GetAngle(rotationFocus));
            }

            void DoAction(int32 const action)
            {
                if (action == ACTION_SHOOT)
                {
                    Vehicle* vehicle = me->GetVehicleKit();
                    Unit* southOrb = vehicle->GetPassenger(SEAT_SOUTH);
                    Unit* northOrb = vehicle->GetPassenger(SEAT_NORTH);
                    if (southOrb && northOrb)
                    {
                        if (northOrb->GetTypeId() != TYPEID_UNIT || southOrb->GetTypeId() != TYPEID_UNIT)
                            return;

                        northOrb->ToCreature()->AI()->Talk(EMOTE_WARN_LASER);
                        northOrb->CastSpell(northOrb, SPELL_TWILIGHT_PULSE_PERIODIC, true);
                        southOrb->CastSpell(southOrb, SPELL_TWILIGHT_PULSE_PERIODIC, true);
                        northOrb->CastSpell(southOrb, SPELL_TWILIGHT_CUTTER, false);
                    }

                    if (!IsHeroic())
                        return;

                    Unit* eastOrb = vehicle->GetPassenger(SEAT_EAST);
                    Unit* westOrb = vehicle->GetPassenger(SEAT_WEST);
                    if (eastOrb && westOrb)
                    {
                        if (eastOrb->GetTypeId() != TYPEID_UNIT || westOrb->GetTypeId() != TYPEID_UNIT)
                            return;

                        eastOrb->CastSpell(eastOrb, SPELL_TWILIGHT_PULSE_PERIODIC, true);
                        westOrb->CastSpell(westOrb, SPELL_TWILIGHT_PULSE_PERIODIC, true);
                        eastOrb->CastSpell(westOrb, SPELL_TWILIGHT_CUTTER, false);
                    }
                }
            }
        private:
            InstanceScript* instance;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetRubySanctumAI<npc_orb_carrierAI>(creature);
        }
};

class npc_meteor_strike_initial : public CreatureScript
{
    public:
        npc_meteor_strike_initial() : CreatureScript("npc_meteor_strike_initial") { }

        struct npc_meteor_strike_initialAI : public Scripted_NoMovementAI
        {
            npc_meteor_strike_initialAI(Creature* creature) : Scripted_NoMovementAI(creature),
                _instance(creature->GetInstanceScript())
            { }

            void DoAction(int32 const action)
            {
                switch (action)
                {
                    case ACTION_METEOR_STRIKE_AOE:
                        DoCast(me, SPELL_METEOR_STRIKE_AOE_DAMAGE, true);
                        DoCast(me, SPELL_METEOR_STRIKE_FIRE_AURA_1, true);
                        for (std::list<Creature*>::iterator itr = _meteorList.begin(); itr != _meteorList.end(); ++itr)
                            (*itr)->AI()->DoAction(ACTION_METEOR_STRIKE_BURN);
                        break;
                }
            }

            void IsSummonedBy(Unit* summoner)
            {
                Creature* owner = summoner->ToCreature();
                if (!owner)
                    return;

                // Let Halion Controller count as summoner
                if (Creature* controller = ObjectAccessor::GetCreature(*me, _instance->GetData64(DATA_HALION_CONTROLLER)))
                    controller->AI()->JustSummoned(me);

                DoCast(me, SPELL_METEOR_STRIKE_COUNTDOWN);
                DoCast(me, SPELL_BIRTH_NO_VISUAL); // Unknown purpose

                if (HalionAI* halionAI = CAST_AI(HalionAI, owner->AI()))
                {
                    Position const* ownerPos = halionAI->GetMeteorStrikePosition();
                    Position newPos;
                    float angle[4];
                    angle[0] = me->GetAngle(ownerPos);
                    angle[1] = me->GetAngle(ownerPos) - static_cast<float>(M_PI/2);
                    angle[2] = me->GetAngle(ownerPos) - static_cast<float>(-M_PI/2);
                    angle[3] = me->GetAngle(ownerPos) - static_cast<float>(M_PI);

                    _meteorList.clear();
                    for (uint8 i = 0; i < 4; i++)
                    {
                        MapManager::NormalizeOrientation(angle[i]);
                        me->SetOrientation(angle[i]);
                        me->GetNearPosition(newPos, 10.0f, 0.0f); // Exact distance
                        if (Creature* meteor = me->SummonCreature(NPC_METEOR_STRIKE_NORTH + i, newPos, TEMPSUMMON_TIMED_DESPAWN, 30000))
                            _meteorList.push_back(meteor);
                    }
                }
            }

            void UpdateAI(uint32 const /*diff*/) { }
            void EnterEvadeMode() { }

        private:
            InstanceScript* _instance;
            std::list<Creature*> _meteorList;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetRubySanctumAI<npc_meteor_strike_initialAI>(creature);
        }
};

class npc_meteor_strike : public CreatureScript
{
    public:
        npc_meteor_strike() : CreatureScript("npc_meteor_strike") { }

        struct npc_meteor_strikeAI : public Scripted_NoMovementAI
        {
            npc_meteor_strikeAI(Creature* creature) : Scripted_NoMovementAI(creature),
                _instance(creature->GetInstanceScript())
            {
                _range = 5.0f;
                _spawnCount = 0;
            }

            void DoAction(int32 const action)
            {
                if (action == ACTION_METEOR_STRIKE_BURN)
                {
                    DoCast(me, SPELL_METEOR_STRIKE_FIRE_AURA_2, true);
                    me->setActive(true);
                    _events.ScheduleEvent(EVENT_SPAWN_METEOR_FLAME, 500);
                }
            }

            void IsSummonedBy(Unit* /*summoner*/)
            {
                // Let Halion Controller count as summoner.
                if (Creature* controller = ObjectAccessor::GetCreature(*me, _instance->GetData64(DATA_HALION_CONTROLLER)))
                    controller->AI()->JustSummoned(me);
            }

            void UpdateAI(uint32 const diff)
            {
                if (_spawnCount > 5)
                    return;

                _events.Update(diff);

                if (_events.ExecuteEvent() == EVENT_SPAWN_METEOR_FLAME)
                {
                    Position pos;
                    me->GetNearPosition(pos, _range, 0.0f);

                    if (Creature* flame = me->SummonCreature(NPC_METEOR_STRIKE_FLAME, pos, TEMPSUMMON_TIMED_DESPAWN, 25000))
                    {
                        if (Creature* controller = ObjectAccessor::GetCreature(*me, _instance->GetData64(DATA_HALION_CONTROLLER)))
                            controller->AI()->JustSummoned(flame);

                        flame->CastSpell(flame, SPELL_METEOR_STRIKE_FIRE_AURA_2, true);
                        ++_spawnCount;
                    }
                    _range += 5.0f;
                    _events.ScheduleEvent(EVENT_SPAWN_METEOR_FLAME, 800);
                }
            }

        private:
            InstanceScript* _instance;
            EventMap _events;
            float _range;
            uint8 _spawnCount;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetRubySanctumAI<npc_meteor_strikeAI>(creature);
        }
};

class npc_combustion_consumption : public CreatureScript
{
    public:
        npc_combustion_consumption() : CreatureScript("npc_combustion_consumption") { }

        struct npc_combustion_consumptionAI : public Scripted_NoMovementAI
        {
            npc_combustion_consumptionAI(Creature* creature) : Scripted_NoMovementAI(creature),
                   _summonerGuid(0), _instance(creature->GetInstanceScript())
            {
                switch (me->GetEntry())
                {
                    case NPC_COMBUSTION:
                        _explosionSpell = SPELL_FIERY_COMBUSTION_EXPLOSION;
                        _damageSpell = SPELL_COMBUSTION_DAMAGE_AURA;
                        me->SetPhaseMask(0x01, true);
                        break;
                    case NPC_CONSUMPTION:
                        _explosionSpell = SPELL_SOUL_CONSUMPTION_EXPLOSION;
                        _damageSpell = SPELL_CONSUMPTION_DAMAGE_AURA;
                        me->SetPhaseMask(0x20, true);
                        break;
                    default: // Should never happen
                        _explosionSpell = 0;
                        _damageSpell = 0;
                        break;
                }
                if (IsHeroic())
                    me->SetPhaseMask(0x01 | 0x20, true);
            }

            void IsSummonedBy(Unit* summoner)
            {
                // Let Halion Controller count as summoner
                if (Creature* controller = ObjectAccessor::GetCreature(*me, _instance->GetData64(DATA_HALION_CONTROLLER)))
                    controller->AI()->JustSummoned(me);

                _summonerGuid = summoner->GetGUID();
            }

            void SetData(uint32 type, uint32 value)
            {
                Unit* summoner = ObjectAccessor::GetUnit(*me, _summonerGuid);

                if (type != DATA_STACKS_DISPELLED || !_damageSpell || !_explosionSpell || !summoner)
                    return;

                me->CastCustomSpell(SPELL_SCALE_AURA, SPELLVALUE_AURA_STACK, value, me);
                DoCast(me, _damageSpell);

                int32 damage = 1200 + (value * 1290); // Needs moar research.
                // Target is TARGET_UNIT_AREA_ALLY_SRC (TARGET_SRC_CASTER)
                summoner->CastCustomSpell(_explosionSpell, SPELLVALUE_BASE_POINT0, damage, summoner);
            }

            void UpdateAI(uint32 const /*diff*/) { }

        private:
            InstanceScript* _instance;
            uint32 _explosionSpell;
            uint32 _damageSpell;
            uint64 _summonerGuid;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetRubySanctumAI<npc_combustion_consumptionAI>(creature);
        }
};

class npc_living_inferno : public CreatureScript
{
    public:
        npc_living_inferno() : CreatureScript("npc_living_inferno") { }

        struct npc_living_infernoAI : public ScriptedAI
        {
            npc_living_infernoAI(Creature* creature) : ScriptedAI(creature) { }

            void JustSummoned(Creature* /*summoner*/)
            {
                me->SetInCombatWithZone();
                DoCast(me, SPELL_BLAZING_AURA);
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetRubySanctumAI<npc_living_infernoAI>(creature);
        }
};

//! Need sniff data
class npc_living_ember : public CreatureScript
{
    public:
        npc_living_ember() : CreatureScript("npc_living_ember") { }

        struct npc_living_emberAI : public ScriptedAI
        {
            npc_living_emberAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset()
            {
                _hasEnraged = false;
            }

            void EnterCombat(Unit* /*who*/)
            {
                _enrageTimer = 20000;
                _hasEnraged = false;
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim() || me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (!_hasEnraged && _enrageTimer <= diff)
                {
                    _hasEnraged = true;
                    DoCast(me, SPELL_BERSERK);
                }
                else _enrageTimer -= diff;

                DoMeleeAttackIfReady();
            }

        private:
            uint32 _enrageTimer;
            bool _hasEnraged;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetRubySanctumAI<npc_living_emberAI>(creature);
        }
};

class go_twilight_portal : public GameObjectScript
{
    public:
        go_twilight_portal() : GameObjectScript("go_twilight_portal") { }

        struct go_twilight_portalAI : public GameObjectAI
        {
            go_twilight_portalAI(GameObject* gameobject) : GameObjectAI(gameobject),
                _instance(gameobject->GetInstanceScript()), _deleted(false)
            {
                switch (gameobject->GetEntry())
                {
                    case GO_HALION_PORTAL_EXIT:
                        gameobject->SetPhaseMask(0x20, true);
                        break;
                    case GO_HALION_PORTAL_1:
                    case GO_HALION_PORTAL_2:
                        gameobject->SetPhaseMask(0x1, true);
                        break;
                    default:
                        break;
                }
            }

            bool GossipHello(Player* player)
            {
                if (uint32 spellID = go->GetGOInfo()->goober.spellId)
                    player->CastSpell(player, spellID, true);
                return false;
            }

            void UpdateAI(uint32 /*diff*/)
            {
                if (_instance->GetBossState(DATA_HALION) == IN_PROGRESS)
                    return;

                if (!_deleted)
                {
                    _deleted = true;
                    go->Delete();
                }
            }

        private:
            InstanceScript* _instance;
            bool _deleted;
        };

        GameObjectAI* GetAI(GameObject* gameobject) const
        {
            return GetRubySanctumAI<go_twilight_portalAI>(gameobject);
        }
};

class spell_halion_meteor_strike_marker : public SpellScriptLoader
{
    public:
        spell_halion_meteor_strike_marker() : SpellScriptLoader("spell_halion_meteor_strike_marker") { }

        class spell_halion_meteor_strike_marker_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_halion_meteor_strike_marker_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (!GetCaster())
                    return;

                if (GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE)
                    if (Creature* creCaster = GetCaster()->ToCreature())
                        creCaster->AI()->DoAction(ACTION_METEOR_STRIKE_AOE);
            }

            void Register()
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_halion_meteor_strike_marker_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_halion_meteor_strike_marker_AuraScript();
        }
};

class spell_halion_combustion_consumption : public SpellScriptLoader
{
    public:
        spell_halion_combustion_consumption(char const* scriptName, uint32 spell) : SpellScriptLoader(scriptName), _spellID(spell) { }

        class spell_halion_combustion_consumption_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_halion_combustion_consumption_AuraScript);

        public:
            spell_halion_combustion_consumption_AuraScript(uint32 spellID) : AuraScript(), _spellID(spellID) { }

            bool Validate(SpellEntry const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(_spellID))
                    return false;
                return true;
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_DEATH)
                    return;

                if (GetTarget()->HasAura(_spellID))
                    GetTarget()->RemoveAurasDueToSpell(_spellID, 0, 0, AURA_REMOVE_BY_EXPIRE);
            }

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                GetTarget()->CastSpell(GetTarget(), _spellID, true);
            }

            void AddMarkStack(AuraEffect const* /*aurEff*/)
            {
                GetTarget()->CastSpell(GetTarget(), _spellID, true);
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_halion_combustion_consumption_AuraScript::AddMarkStack, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
                AfterEffectApply += AuraEffectApplyFn(spell_halion_combustion_consumption_AuraScript::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_halion_combustion_consumption_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
            }

            uint32 _spellID;
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_halion_combustion_consumption_AuraScript(_spellID);
        }

    private:
        uint32 _spellID;
};

class spell_halion_marks : public SpellScriptLoader
{
    public:
        spell_halion_marks(char const* scriptName, uint32 summonSpell) : SpellScriptLoader(scriptName), _summonSpell(summonSpell) { }

        class spell_halion_marks_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_halion_marks_AuraScript);

        public:
            spell_halion_marks_AuraScript(uint32 summonSpell) : AuraScript(), _summonSpell(summonSpell) { }

            bool Validate(SpellEntry const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(_summonSpell))
                    return false;
                return true;
            }

            /// We were purged. Force removed stacks to zero
            /// and trigger the appropriated remove handler.
            /// See spell_halion_combustion_consumption_AuraScript::OnRemove
            void BeforeDispel(DispelInfo* dispelData)
            {
                Unit* dispelledUnit = dispelData->GetDispelled();
                // Prevent any stack from being removed at this point.
                dispelData->SetRemovedCharges(0);

                if (dispelledUnit->HasAura(SPELL_FIERY_COMBUSTION))
                    dispelledUnit->RemoveAurasDueToSpell(SPELL_FIERY_COMBUSTION, 0, 0, AURA_REMOVE_BY_EXPIRE);
                else if (dispelledUnit->HasAura(SPELL_SOUL_CONSUMPTION))
                    dispelledUnit->RemoveAurasDueToSpell(SPELL_SOUL_CONSUMPTION, 0, 0, AURA_REMOVE_BY_EXPIRE);
            }

            void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                Unit* target = GetTarget();

                if (GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_EXPIRE)
                    return;

                uint8 stacks = aurEff->GetBase()->GetStackAmount();

                CustomSpellValues values;
                values.AddSpellMod(SPELLVALUE_BASE_POINT1, stacks);

                target->CastCustomSpell(_summonSpell, values, target, TRIGGERED_FULL_MASK, NULL, NULL, GetCasterGUID());
            }

            void Register()
            {
                OnDispel += AuraDispelFn(spell_halion_marks_AuraScript::BeforeDispel);
                AfterEffectRemove += AuraEffectRemoveFn(spell_halion_marks_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }

            uint32 _summonSpell;
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_halion_marks_AuraScript(_summonSpell);
        }

    private:
        uint32 _summonSpell;
};

class spell_halion_damage_aoe_summon : public SpellScriptLoader
{
    public:
        spell_halion_damage_aoe_summon() : SpellScriptLoader("spell_halion_damage_aoe_summon") { }

        class spell_halion_damage_aoe_summon_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_halion_damage_aoe_summon_SpellScript);

            void HandleSummon(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                Unit* caster = GetCaster();
                uint32 entry = uint32(GetSpellInfo()->Effects[effIndex].MiscValue);
                SummonPropertiesEntry const* properties = sSummonPropertiesStore.LookupEntry(uint32(GetSpellInfo()->Effects[effIndex].MiscValueB));
                uint32 duration = uint32(GetSpellInfo()->GetDuration());

                Position pos;
                caster->GetPosition(&pos);
                if (Creature* summon = caster->GetMap()->SummonCreature(entry, pos, properties, duration, caster, GetSpellInfo()->Id))
                    if (summon->IsAIEnabled)
                        summon->AI()->SetData(DATA_STACKS_DISPELLED, GetSpellValue()->EffectBasePoints[EFFECT_1]);
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_halion_damage_aoe_summon_SpellScript::HandleSummon, EFFECT_0, SPELL_EFFECT_SUMMON);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_halion_damage_aoe_summon_SpellScript();
        }
};

class spell_halion_twilight_realm_handlers : public SpellScriptLoader
{
    public:
        spell_halion_twilight_realm_handlers(const char* scriptName, uint32 beforeHitSpell, bool isApplyHandler) : SpellScriptLoader(scriptName),
            _beforeHitSpell(beforeHitSpell), _isApplyHandler(isApplyHandler)
        { }

        class spell_halion_twilight_realm_handlers_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_halion_twilight_realm_handlers_AuraScript);

        public:
            spell_halion_twilight_realm_handlers_AuraScript(uint32 beforeHitSpell, bool isApplyHandler) : AuraScript(),
                _isApplyHandler(isApplyHandler), _beforeHitSpell(beforeHitSpell)
            { }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*handle*/)
            {
                GetTarget()->RemoveAurasDueToSpell(SPELL_TWILIGHT_REALM);
            }

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*handle*/)
            {
                Unit* target = GetTarget();
                if (!target)
                    return;

                target->RemoveAurasDueToSpell(_beforeHitSpell, 0, 0, AURA_REMOVE_BY_ENEMY_SPELL);
                if (InstanceScript* instance = target->GetInstanceScript())
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_UNK7);
            }

            void Register()
            {
                if (!_isApplyHandler)
                {
                    AfterEffectApply += AuraEffectApplyFn(spell_halion_twilight_realm_handlers_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                    AfterEffectRemove += AuraEffectRemoveFn(spell_halion_twilight_realm_handlers_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                }
                else
                    AfterEffectApply += AuraEffectApplyFn(spell_halion_twilight_realm_handlers_AuraScript::OnApply, EFFECT_0, SPELL_AURA_PHASE, AURA_EFFECT_HANDLE_REAL);
            }

            bool _isApplyHandler;
            uint32 _beforeHitSpell;
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_halion_twilight_realm_handlers_AuraScript(_beforeHitSpell, _isApplyHandler);
        }

    private:
        uint32 _beforeHitSpell;
        bool _isApplyHandler;
};

class spell_halion_clear_debuffs : public SpellScriptLoader
{
    public:
        spell_halion_clear_debuffs() : SpellScriptLoader("spell_halion_clear_debuffs") { }

        class spell_halion_clear_debuffs_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_halion_clear_debuffs_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_CLEAR_DEBUFFS))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_TWILIGHT_REALM))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex effIndex)
            {
                if (GetHitUnit()->HasAura(GetSpellInfo()->Effects[effIndex].CalcValue()))
                    GetHitUnit()->RemoveAurasDueToSpell(GetSpellInfo()->Effects[effIndex].CalcValue());
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_halion_clear_debuffs_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_halion_clear_debuffs_SpellScript();
        }
};

class TwilightCutterSelector
{
    public:
        TwilightCutterSelector(Unit* caster, Unit* cutterCaster) : _caster(caster), _cutterCaster(cutterCaster) {}

        bool operator()(WorldObject* unit)
        {
            return !unit->IsInBetween(_caster, _cutterCaster, 4.0f);
        }

    private:
        Unit* _caster;
        Unit* _cutterCaster;
};

class spell_halion_twilight_cutter : public SpellScriptLoader
{
    public:
        spell_halion_twilight_cutter() : SpellScriptLoader("spell_halion_twilight_cutter") { }

        class spell_halion_twilight_cutter_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_halion_twilight_cutter_SpellScript);

            void RemoveNotBetween(std::list<WorldObject*>& unitList)
            {
                if (unitList.empty())
                    return;

                Unit* caster = GetCaster();
                if (Aura* cutter = caster->GetAura(SPELL_TWILIGHT_CUTTER))
                {
                    if (Unit* cutterCaster = cutter->GetCaster())
                    {
                        unitList.remove_if(TwilightCutterSelector(caster, cutterCaster));
                        return;
                    }
                }

                // In case cutter caster werent found for some reason
                unitList.clear();
            }

            void Register()
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_halion_twilight_cutter_SpellScript::RemoveNotBetween, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_halion_twilight_cutter_SpellScript();
        }
};

class spell_halion_twilight_phasing : public SpellScriptLoader
{
    public:
        spell_halion_twilight_phasing() : SpellScriptLoader("spell_halion_twilight_phasing") { }

        class spell_halion_twilight_phasing_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_halion_twilight_phasing_SpellScript);

            void Phase()
            {
                Unit* caster = GetCaster();
                caster->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                caster->CastSpell(caster->GetPositionX(), caster->GetPositionY(), caster->GetPositionZ(), SPELL_SUMMON_TWILIGHT_PORTAL, true);
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_halion_twilight_phasing_SpellScript::Phase);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_halion_twilight_phasing_SpellScript();
        }
};

class spell_halion_summon_exit_portals : public SpellScriptLoader
{
    public:
        spell_halion_summon_exit_portals() : SpellScriptLoader("spell_halion_summon_exit_portals") { }

        class spell_halion_summon_exit_portals_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_halion_summon_exit_portals_SpellScript);

            void OnSummon(SpellEffIndex effIndex)
            {
                WorldLocation summonPos = *GetExplTargetDest();
                Position offset = {0.0f, 20.0f, 0.0f, 0.0f};
                if (effIndex == EFFECT_1)
                    offset.m_positionY = -20.0f;

                summonPos.RelocateOffset(offset);

                SetExplTargetDest(summonPos);
                GetHitDest()->RelocateOffset(offset);
            }

            void Register()
            {
                OnEffectLaunch += SpellEffectFn(spell_halion_summon_exit_portals_SpellScript::OnSummon, EFFECT_0, SPELL_EFFECT_SUMMON_OBJECT_WILD);
                OnEffectLaunch += SpellEffectFn(spell_halion_summon_exit_portals_SpellScript::OnSummon, EFFECT_1, SPELL_EFFECT_SUMMON_OBJECT_WILD);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_halion_summon_exit_portals_SpellScript();
        }
};

void AddSC_boss_halion()
{
    new boss_halion();
    new boss_twilight_halion();
    
    new npc_halion_controller();
    new npc_meteor_strike_initial();
    new npc_meteor_strike();
    new npc_combustion_consumption();
    new npc_orb_carrier();
    new npc_living_inferno();
    new npc_living_ember();

    new go_twilight_portal();

    new spell_halion_meteor_strike_marker();
    new spell_halion_combustion_consumption("spell_halion_soul_consumption", SPELL_MARK_OF_CONSUMPTION);
    new spell_halion_combustion_consumption("spell_halion_fiery_combustion", SPELL_MARK_OF_COMBUSTION);
    new spell_halion_marks("spell_halion_mark_of_combustion", SPELL_FIERY_COMBUSTION_SUMMON);
    new spell_halion_marks("spell_halion_mark_of_consumption", SPELL_SOUL_CONSUMPTION_SUMMON);
    new spell_halion_damage_aoe_summon();
    new spell_halion_twilight_realm_handlers("spell_halion_leave_twilight_realm", SPELL_SOUL_CONSUMPTION, false);
    new spell_halion_twilight_realm_handlers("spell_halion_enter_twilight_realm", SPELL_FIERY_COMBUSTION, true);
    new spell_halion_summon_exit_portals();
    new spell_halion_twilight_phasing();
    new spell_halion_twilight_cutter();
    new spell_halion_clear_debuffs();
}
