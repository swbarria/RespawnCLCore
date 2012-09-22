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

/* ScriptData
Name: bf_commandscript
%Complete: 100
Comment: All bf related commands
Category: commandscripts
EndScriptData */

#include "ScriptMgr.h"
#include "Chat.h"
#include "BattlefieldMgr.h"
enum Battlefield
{
    WINTERGRASP   = 1,
};

class bf_commandscript : public CommandScript
{
public:
    bf_commandscript() : CommandScript("bf_commandscript") { }

    ChatCommand* GetCommands() const
    {
        static ChatCommand battlefieldcommandTable[] =
        {
            { "start",          SEC_ADMINISTRATOR,  false, &HandleBattlefieldStart,            "", NULL },
            { "stop",           SEC_ADMINISTRATOR,  false, &HandleBattlefieldEnd,              "", NULL },
            { "timer",          SEC_ADMINISTRATOR,  false, &HandleBattlefieldTimer,            "", NULL },
            { "enable",         SEC_ADMINISTRATOR,  false, &HandleBattlefieldEnable,           "", NULL },
            { NULL,             0,                  false, NULL,                               "", NULL }
        };
        static ChatCommand commandTable[] =
        {
            { "wg",             SEC_ADMINISTRATOR,  false, NULL,            "", battlefieldcommandTable },
            { NULL,             0,                  false, NULL,                               "", NULL }
        };
        return commandTable;
    }

    static bool HandleBattlefieldStart(ChatHandler* handler, const char* args)
    {
        Battlefield* bf = sBattlefieldMgr->GetBattlefieldByBattleId(WINTERGRASP);

        if (!bf)
            return false;

        bf->StartBattle();
        handler->SendGlobalGMSysMessage("Wintergrasp Started");
        return true;
    }

    static bool HandleBattlefieldEnd(ChatHandler* handler, const char* args)
    {
        Battlefield* bf = sBattlefieldMgr->GetBattlefieldByBattleId(WINTERGRASP);
        bf->EndBattle(true);
        handler->SendGlobalGMSysMessage("Wintergrasp Stopped");
        return true;
    }

    static bool HandleBattlefieldEnable(ChatHandler* handler, const char* args)
    {
        Battlefield* bf = sBattlefieldMgr->GetBattlefieldByBattleId(WINTERGRASP);
        if (bf->IsEnabled())
        {
            bf->ToggleBattlefield(false);
            handler->SendGlobalGMSysMessage("Wintergrasp is disabled");
        }
        else
        {
            bf->ToggleBattlefield(true);
            handler->SendGlobalGMSysMessage("Wintergrasp is enabled");
        }

        return true;
    }

    static bool HandleBattlefieldTimer(ChatHandler* handler, const char* args)
    {
        uint32 time = 0;
        char* time_str = strtok(NULL, " ");
        if (!time_str)
            return false;
        time = atoi(time_str);
        Battlefield* bf = sBattlefieldMgr->GetBattlefieldByBattleId(WINTERGRASP);
        bf->SetTimer(time * IN_MILLISECONDS);
        bf->SendInitWorldStatesToAll();
        handler->SendGlobalGMSysMessage("Wintergrasp (Command timer used)");
        return true;
    }
};

void AddSC_bf_commandscript()
{
    new bf_commandscript();
}
