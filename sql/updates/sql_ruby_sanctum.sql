-- !! TODO LIST
-- !! * Fix all damage from all NPCs.

-- Combustion / Consumption scaling aura
DELETE FROM `spell_dbc` WHERE `id`=70507;
INSERT INTO `spell_dbc` (`Id`,`Attributes`,`AttributesEx`,`AttributesEx2`,`CastingTimeIndex`,`ProcChance`,`DurationIndex`,`RangeIndex`,`StackAmount`,`Effect1`,`EffectBasePoints1`,`EffectImplicitTargetA1`,`EffectApplyAuraName1`,`DmgMultiplier1`,`Comment`) VALUES
(70507,0x00000100,0x00000400,0x0,1,101,21,1,99,6,10,1,61,1, 'Halion - Combustion & Consumption Scale Aura (guessed values)');

-- Copy damage spell
DELETE FROM `spell_dbc` WHERE `id`=74810;
INSERT INTO `spell_dbc` (`Id`,`Attributes`,`AttributesEx`,`AttributesEx2`,`AttributesEx3`,`AttributesEx4`,`AttributesEx5`,`AttributesEx6`,`AttributesEx7`,`CastingTimeIndex`,`Effect1`,`EffectImplicitTargetA1`,`MaxAffectedTargets`,`Comment`) VALUES
(74810,0,0x00010488,0x20000000,0x2004000,0,0x00060008,0x21002000,0,1,3,25,1, 'Halion - Copy Damage (guessed values)');

-- Bosses respawn time
UPDATE `creature` SET `spawntimesecs`=604800 WHERE `id` IN (39751, 39746, 39747);

-- Trash mobs respawn time
UPDATE `creature` SET `spawntimesecs`=1209600 WHERE `map`=724 AND `id` NOT IN (39751,39746,39747);

-- Difficulty entries
UPDATE `creature_template` SET `difficulty_entry_1`=40143, `difficulty_entry_2`=40144, `difficulty_entry_3`=40145 WHERE `entry`=40142;
UPDATE `creature_template` SET `difficulty_entry_1`=40470, `difficulty_entry_2`=40471, `difficulty_entry_3`=40472 WHERE `entry`=40081;

-- ---------------------------------------------------------------- --
-- ----------------------- Template updates ----------------------- --
-- ---------------------------------------------------------------- --

-- Halion
UPDATE `creature_template` SET
    `mindmg`=509,
    `maxdmg`=683,
    `attackpower`=805,
    `dmg_multiplier`=35,
    `faction_A`=14,
    `faction_H`=14,
    `exp`=2
WHERE `entry` IN (39863, 39864, 39944, 39945, 40142);
UPDATE `creature_template` SET `ScriptName`= 'boss_halion',`flags_extra`=`flags_extra`|0x1 WHERE `entry`=39863;

-- Trash mobs
UPDATE `creature_template` SET
    `mindmg`=422,
    `maxdmg`=586,
    `attackpower`=642,
    `dmg_multiplier`=7.5
WHERE `entry` IN (40417, 40418, 40419, 40420, 40421, 40422, 40423, 40424);

-- Pre bosses
UPDATE `creature_template` SET
    `mindmg`=509,
    `maxdmg`=683,
    `attackpower`=805,
    `dmg_multiplier`=35
WHERE `entry` IN (39751, 39920, 39747, 39823, 39746, 39805);

UPDATE `creature_template` SET `flags_extra`=130 WHERE `entry` IN (40041, 40042, 40043, 40044); -- 40041, 40042, 40043 & 40044 - Meteor Strike
UPDATE `creature_template` SET `flags_extra`=130 WHERE `entry`=40029; -- 40029 - Meteor Strike (Initial)
UPDATE `creature_template` SET `flags_extra`=130 WHERE `entry`=40055; -- 40055 - Meteor Strike

UPDATE `creature_template` SET `faction_A`=14,`faction_H`=14,`exp`=2,`mindmg`=509,`maxdmg`=683,`attackpower`=805,`dmg_multiplier`=35 WHERE `entry`=40143; -- 40143 - Halion (1) - The Twilight Destroyer
UPDATE `creature_template` SET `faction_A`=14,`faction_H`=14,`exp`=2,`mindmg`=509,`maxdmg`=683,`attackpower`=805,`dmg_multiplier`=35 WHERE `entry`=40144; -- 40144 - Halion (2) - The Twilight Destroyer
UPDATE `creature_template` SET `faction_A`=14,`faction_H`=14,`exp`=2,`mindmg`=509,`maxdmg`=683,`attackpower`=805,`dmg_multiplier`=35 WHERE `entry`=40145; -- 40145 - Halion (3) - The Twilight Destroyer

-- 40091 - Orb Rotation Focus
UPDATE `creature_template` SET `modelid1`=11686, `modelid2`=169, `scale`=1, `unit_flags`=0x2000100 WHERE `entry`=40091;

UPDATE `creature_template` SET `InhabitType`=7,`modelid1`=11686,`modelid2`=169,`VehicleId`=718,`unit_flags`=0x2000100 WHERE `entry`=40081; -- 40081 - Orb Carrier 
UPDATE `creature_template` SET `InhabitType`=7,`modelid1`=11686,`modelid2`=169,`VehicleId`=718,`unit_flags`=0x2000100 WHERE `entry`=40470; -- 40470 - Orb Carrier (1)
UPDATE `creature_template` SET `InhabitType`=7,`modelid1`=11686,`modelid2`=169,`VehicleId`=746,`unit_flags`=0x2000100 WHERE `entry`=40471; -- 40471 - Orb Carrier (2)
UPDATE `creature_template` SET `InhabitType`=7,`modelid1`=11686,`modelid2`=169,`VehicleId`=746,`unit_flags`=0x2000100 WHERE `entry`=40472; -- 40472 - Orb Carrier (3)
UPDATE `creature_template` SET `scale`=1,`flags_extra`=130,`exp`=2,`baseattacktime`=2000,`unit_flags`=33554432 WHERE `entry` IN(40001, 40135); -- 40001 & 40135 - Combustion & Consumption

-- 40469, 40468, 40083 & 40100 - Shadow Orb
UPDATE `creature_template` SET `InhabitType`=7,`flags_extra`=2,`unit_flags`=33554432,`baseattacktime`=2000,`speed_walk`=2.4,`speed_run`=0.85714,`faction_A`=14,`faction_H`=14,`exp`=2,`maxlevel`=80,`minlevel`=80,`HoverHeight`=6.25, `ScriptName`= '' WHERE `entry` IN (40469, 40468, 40083, 40100);

UPDATE `creature_template` SET `speed_walk`=3.2,`speed_run`=1.71428573131561, /*`faction_H`=834,`faction_A`=834,*/`unit_flags`=0x40 WHERE `entry` IN (40683,40681); -- Living Inferno & Living Ember

-- Script Names
UPDATE `creature_template` SET `ScriptName`= 'boss_twilight_halion' WHERE `entry`=40142; -- Twilight Halion
UPDATE `creature_template` SET `ScriptName`= 'npc_orb_carrier' WHERE `entry`=40081;
UPDATE `creature_template` SET `ScriptName`= 'npc_combustion_consumption' WHERE `entry` IN(40001, 40135);
UPDATE `creature_template` SET `ScriptName`= 'npc_meteor_strike_initial' WHERE `entry`=40029;
UPDATE `creature_template` SET `ScriptName`= 'npc_meteor_strike' WHERE `entry` IN (40041, 40042, 40043, 40044);

-- Model info update
UPDATE `creature_model_info` SET `bounding_radius`=1, `combat_reach`=2 WHERE `modelid`=16946;
UPDATE `creature_model_info` SET `combat_reach`=18 WHERE `modelid`=31952;
UPDATE `creature_model_info` SET `combat_reach`=12.25 WHERE `modelid`=32179;

-- Spell 75074 cannot be found in any DBC file and is not found in sniffs.
-- thus leaving us with no other choice than editing a WDB field (kids, do not try this at home)
UPDATE `gameobject_template` SET `data10`=74807,`WDBVerified`=-12340 WHERE `entry` IN (202794, 202795);
-- UPDATE `gameobject_template` SET `ScriptName`="go_exit_twilight_realm",`flags`=`flags`|32, WHERE `entry`=202796;

-- Spell scripts
DELETE FROM `spell_script_names` WHERE `ScriptName`= 'spell_halion_meteor_strike_marker';
DELETE FROM `spell_script_names` WHERE `ScriptName`= 'spell_halion_fiery_combustion';
DELETE FROM `spell_script_names` WHERE `ScriptName`= 'spell_halion_soul_consumption';
DELETE FROM `spell_script_names` WHERE `ScriptName`= 'spell_halion_mark_of_combustion';
DELETE FROM `spell_script_names` WHERE `ScriptName`= 'spell_halion_mark_of_consumption';
DELETE FROM `spell_script_names` WHERE `ScriptName`= 'spell_halion_combustion_consumption_summon';
DELETE FROM `spell_script_names` WHERE `ScriptName`= 'spell_halion_leave_twilight_realm';
DELETE FROM `spell_script_names` WHERE `ScriptName`= 'spell_halion_enter_twilight_realm';
DELETE FROM `spell_script_names` WHERE `ScriptName`= 'spell_halion_twilight_phasing';
DELETE FROM `spell_script_names` WHERE `ScriptName`= 'spell_halion_twilight_cutter';
DELETE FROM `spell_script_names` WHERE `ScriptName`= 'spell_halion_clear_debuffs';
INSERT INTO `spell_script_names` (`spell_id`,`ScriptName`) VALUES
(74641, 'spell_halion_meteor_strike_marker'),
(74562, 'spell_halion_fiery_combustion'),
(74792, 'spell_halion_soul_consumption'),
(74567, 'spell_halion_mark_of_combustion'),
(74795, 'spell_halion_mark_of_consumption'),
(74610, 'spell_halion_combustion_consumption_summon'),
(74800, 'spell_halion_combustion_consumption_summon'),
(74812, 'spell_halion_leave_twilight_realm'),
(74807, 'spell_halion_enter_twilight_realm'),
(74808, 'spell_halion_twilight_phasing'),
(74769, 'spell_halion_twilight_cutter'),
(77844, 'spell_halion_twilight_cutter'),
(77845, 'spell_halion_twilight_cutter'),
(75396, 'spell_halion_clear_debuffs'),
(77846, 'spell_halion_twilight_cutter');

-- Texts
DELETE FROM `creature` WHERE `id`=40146;
DELETE FROM `creature_text` WHERE `entry`=39863;
DELETE FROM `creature_text` WHERE `entry`=40142;
DELETE FROM `creature_text` WHERE `entry`=40146;
DELETE FROM `creature_text` WHERE `entry`=40083;
INSERT INTO `creature_text` (`entry`,`groupid`,`id`,`text`,`type`,`language`,`probability`,`emote`,`duration`,`sound`,`comment`) VALUES 
(39863,0,0, 'Meddlesome insects! You are too late. The Ruby Sanctum is lost!',14,0,100,1,0,17499, 'Halion'),
(39863,1,0, 'Your world teeters on the brink of annihilation. You will ALL bear witness to the coming of a new age of DESTRUCTION!',14,0,100,0,0,17500, 'Halion'),
(39863,2,0, 'The heavens burn!',14,0,100,0,0,17505, 'Halion'),
(39863,3,0, 'You will find only suffering within the realm of twilight! Enter if you dare!',14,0,100,0,0,17507, 'Halion'),
(39863,4,0, 'Relish this victory, mortals, for it will be your last! This world will burn with the master''s return!',14,0,100,0,0,17503, 'Halion'),
(39863,5,0, 'Another "hero" falls.',14,0,100,0,0,17501, 'Halion'),
(39863,6,0, 'Not good enough.',14,0,100,0,0,17504, 'Halion'),

(40142,0,0, 'Beware the shadow!',14,0,100,0,0,17506, 'Halion'),
(40142,1,0, 'I am the light and the darkness! Cower, mortals, before the herald of Deathwing!',14,0,100,0,0,17508, 'Halion'),

(40146,0,0, 'Your companion''s efforts have forced Halion further out of the Physical realm!',42,0,100,0,0,0, 'Halion Controller'),
(40146,1,0, 'Your efforts have forced Halion further into the Physical realm!',42,0,100,0,0,0, 'Halion Controller'),
(40146,2,0, 'Your companion''s efforts have forced Halion further out of the Twilight realm!',42,0,100,0,0,0, 'Halion Controller'),
(40146,3,0, 'Your efforts have forced Halion further into the Twilight realm!',42,0,100,0,0,0, 'Halion Controller'),
(40146,4,0, 'Without pressure in both realms, Halion begins to regenerate.',42,0,100,0,0,0, 'Halion Controller'),

(40083,0,0, 'The orbiting spheres pulse with dark energy!',41,0,100,0,0,0, 'Shadow Orb');

-- Spawns
SET @OGUID = 850000; -- Set guid (1 required)
DELETE FROM `gameobject` WHERE `id`=203624;
INSERT INTO `gameobject` (`guid`,`id`,`map`,`spawnMask`,`phaseMask`,`position_x`,`position_y`,`position_z`,`orientation`,`rotation0`,`rotation1`,`rotation2`,`rotation3`,`spawntimesecs`,`animprogress`,`state`) VALUES
(@OGUID,203624,724,15,0x20,3157.372,533.9948,72.8887,1.034892,0,0,0.4946623,0.8690853,120,0,0); -- GO_TWILIGHT_FLAME_RING

SET @GUID = 850000; -- Set guid (3 required)
DELETE FROM `creature` WHERE `id` IN (40081,40091); -- ,40151);
INSERT INTO `creature` (`guid`,`id`,`map`,`spawnMask`,`phaseMask`,`modelid`,`equipment_id`,`position_x`,`position_y`,`position_z`,`orientation`,`spawntimesecs`,`spawndist`,`currentwaypoint`,`curhealth`,`curmana`,`MovementType`,`npcflag`,`unit_flags`,`dynamicflags`) VALUES
(@GUID,40091,724,1,0x20,0,0,3113.711,533.5382,72.96869,1.936719,300,0,0,1,0,2,0,0,0), -- Orb Rotation Focus
(@GUID+1,40081,724,1,0x20,0,0,3153.75,533.1875,72.97205,0,300,0,0,1,0,0,0,0,0); -- Orb Carrier
-- (@GUID+2,40151,724,1,0x21,0,0,3153.75,533.1875,72.97205,0,300,0,0,1,0,0,0,0,0); -- Combat Stalker (TODO: move to cpp. sniff prooves it is not spawned pre fight)

-- Pathing for Orb Rotation Focus Entry: 40091
SET @PATH = @GUID * 10;
UPDATE `creature` SET `spawndist`=0,`MovementType`=2 WHERE `guid`=@GUID;
DELETE FROM `creature_addon` WHERE `guid`=@GUID;
INSERT INTO `creature_addon` (`guid`,`path_id`,`bytes2`,`mount`,`auras`) VALUES (@GUID,@PATH,1,0, '');
DELETE FROM `waypoint_data` WHERE `id`=@PATH;
INSERT INTO `waypoint_data` (`id`,`point`,`position_x`,`position_y`,`position_z`,`orientation`,`delay`,`move_flag`,`action`,`action_chance`,`wpguid`) VALUES
(@PATH,1,3117.59,547.7952,72.96869,0,0,0,0,100,0),
(@PATH,2,3127.461,558.7396,72.96869,0,0,0,0,100,0),
(@PATH,3,3138.042,567.9514,72.98305,0,0,0,0,100,0),
(@PATH,4,3154.09,574.9636,72.98305,0,0,0,0,100,0),
(@PATH,5,3172.565,567.493,72.86058,0,0,0,0,100,0),
(@PATH,6,3181.981,555.8889,72.9127,0,0,0,0,100,0),
(@PATH,7,3189.923,533.3542,73.0377,0,0,0,0,100,0),
(@PATH,8,3182.315,513.4202,72.9771,0,0,0,0,100,0),
(@PATH,9,3177.168,504.3802,72.7271,0,0,0,0,100,0),
(@PATH,10,3167.878,496.8368,72.50312,0,0,0,0,100,0),
(@PATH,11,3152.238,490.4705,72.62009,0,0,0,0,100,0),
(@PATH,12,3138.174,499.3056,72.87009,0,0,0,0,100,0),
(@PATH,13,3126.83,506.0799,72.95515,0,0,0,0,100,0),
(@PATH,14,3120.68,515.3524,72.95515,0,0,0,0,100,0),
(@PATH,15,3113.711,533.5382,72.96869,0,0,0,0,100,0);

-- Vehicle accessory for Orb Carrier
DELETE FROM `vehicle_template_accessory` WHERE `entry` IN (40081,40470,40471,40472);
INSERT INTO `vehicle_template_accessory` (`entry`,`accessory_entry`,`seat_id`,`minion`,`description`,`summontype`,`summontimer`) VALUES
(40081,40083,0,1, 'Orb Carrier',6,30000),
(40081,40100,1,1, 'Orb Carrier',6,30000),

(40470,40083,0,1, 'Orb Carrier',6,30000),
(40470,40100,1,1, 'Orb Carrier',6,30000),

(40471,40083,0,1, 'Orb Carrier',6,30000),
(40471,40100,1,1, 'Orb Carrier',6,30000),
(40471,40468,2,1, 'Orb Carrier',6,30000),
(40471,40469,3,1, 'Orb Carrier',6,30000),

(40472,40083,0,1, 'Orb Carrier',6,30000),
(40472,40100,1,1, 'Orb Carrier',6,30000),
(40472,40468,2,1, 'Orb Carrier',6,30000),
(40472,40469,3,1, 'Orb Carrier',6,30000);

-- Vehicle spellclicks
DELETE FROM `npc_spellclick_spells` WHERE `npc_entry` IN (40081,40470,40471,40472);
INSERT INTO `npc_spellclick_spells` (`npc_entry`,`spell_id`,`cast_flags`,`user_type`) VALUES
(40081, 46598, 0, 1), -- Ride Vehicle Hardcoded
(40470, 46598, 0, 1),
(40471, 46598, 0, 1),
(40472, 46598, 0, 1);

-- Conditions
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=13 AND `SourceEntry`=74758;
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=13 AND `SourceEntry`=75509;
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=13 AND `SourceEntry`=75886;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES
(13,1,75886,0,0,31,0,3,40683,0,0,0, "", "Blazing Aura can only target Living Embers"),
(13,1,75886,0,0,31,0,3,40684,0,0,0, "", "Blazing Aura can only target Living Embers"),
(13,3,75509,0,0,31,0,3,40142,0,0,0, "", "Twilight Mending can only target Halion"),
(13,3,75509,0,0,31,0,3,39863,0,0,0, "", "Twilight Mending can only target Halion"),
(13,1,74758,0,0,31,0,3,40091,0,0,0, "", "Track Rotation can only target Orb Rotation Focus");

-- All the SQL updates here are from sniff

UPDATE `creature_template` SET
    `speed_walk`=1.6,
    `speed_run`=1.42857146263123,
    `minlevel`=83, `maxlevel`=83,
    `faction_H`=14, `faction_A`=14,
    `unit_flags`=0x88840,
    `BaseAttackTime`=1800,
    `DynamicFlags`=0xC
WHERE `entry`=40142; -- Twilight Halion

UPDATE `creature_template` SET
    `speed_walk`=1.6,
    `speed_run`=1.42857146263123,
    `minlevel`=83, `maxlevel`=83,
    `faction_H`=14, `faction_A`=14,
    `unit_flags`=0x8040,
    `BaseAttackTime`=1500
WHERE `entry`=39863; -- Material Halion

UPDATE `creature_template` SET
    `Faction_H`=14, `Faction_A`=14,
    `unit_flags`=0x2000000,
    `Unit_Class`=2
WHERE `entry`=40029; -- Meteor Strike

UPDATE `creature_template` SET
    `Faction_H`=14, `Faction_A`=14,
    `unit_flags`=0x2000000
WHERE `entry`=40001; -- Combustion

UPDATE `creature_template` SET
    `InhabitType`=5,
    `speed_walk`=1.2,
    `speed_run`=0.428571432828903,
    `VehicleId`=718,
    `minlevel`=80,
    `maxlevel`=80,
    `faction_H`=14,
    `faction_A`=14,
    `unit_flags`=0x2000100
WHERE `entry` in (40081,40470,40471,40472); -- Orb Carrier

UPDATE `creature_template` SET
    `speed_walk`=2.2,
    `speed_run`=0.785714268684387,
    `minlevel`=80,
    `maxlevel`=80,
    `faction_H`=14,
    `faction_A`=14,
    `unit_flags`=0x2000100
WHERE `entry` IN (40091,43280,43281,43282); -- Orb Rotation Focus

UPDATE `creature_template` SET `difficulty_entry_1`=43280,`difficulty_entry_2`=43281,`difficulty_entry_3`=43282 WHERE `entry`=40091;

UPDATE `creature_template` SET
    `flags_extra`=130,
    `ScriptName`= 'npc_halion_controller',
    `exp`=2,
    `speed_walk`=2.8,
    `speed_run`=1,
    `minlevel`=80,
    `maxlevel`=80,
    `faction_A`=14,
    `faction_H`=14,
    `unit_flags`=0x2000100
WHERE `entry`=40146; -- 40146 - Halion Controller

DELETE FROM `creature_template_addon` WHERE `entry` IN (40142, 40146, 40001, 40135, 40100, 40469, 40468, 40083, 39863, 40081, 40091);
INSERT INTO `creature_template_addon` (`entry`, `mount`, `bytes1`, `bytes2`, `auras`) VALUES
(40142, 0, 0x0, 0x1, '75476'),
(40146, 0, 0x0, 0x1, ''),
(40001, 0, 0x0, 0x1, '74629'),
(40135, 0, 0x0, 0x1, '74803'),
(40469, 0, 0x2000000, 0x1, ''),
(40468, 0, 0x2000000, 0x1, ''),
(40083, 0, 0x2000000, 0x1, ''),
(40100, 0, 0x2000000, 0x1, ''),
(39863, 0, 0x0, 0x1, '78243'),
(40081, 0, 0x2000000, 0x1, ''),
(40091, 0, 0x0, 0x1, '');
