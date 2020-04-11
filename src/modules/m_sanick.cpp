/*
 * InspIRCd -- Internet Relay Chat Daemon
 *
 *   Copyright (C) 2013, 2018 Sadie Powell <sadie@witchery.services>
 *   Copyright (C) 2012-2014, 2016 Attila Molnar <attilamolnar@hush.com>
 *   Copyright (C) 2012, 2019 Robby <robby@chatbelgie.be>
 *   Copyright (C) 2010 Craig Edwards <brain@inspircd.org>
 *   Copyright (C) 2009-2010 Daniel De Graaf <danieldg@inspircd.org>
 *   Copyright (C) 2007-2008 Robin Burchell <robin+git@viroteck.net>
 *   Copyright (C) 2007, 2009 Dennis Friis <peavey@inspircd.org>
 *
 * This file is part of InspIRCd.  InspIRCd is free software: you can
 * redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "inspircd.h"

/** Handle /SANICK
 */
class CommandSanick : public Command
{
 public:
	CommandSanick(Module* Creator) : Command(Creator,"SANICK", 2)
	{
		allow_empty_last_param = false;
		flags_needed = 'o';
		syntax = "<nick> <newnick>";
		translation = { TR_NICK, TR_TEXT };
	}

	CmdResult Handle(User* user, const Params& parameters) override
	{
		User* target = ServerInstance->Users.Find(parameters[0]);

		/* Do local sanity checks and bails */
		if (IS_LOCAL(user))
		{
			if (target && target->server->IsULine())
			{
				user->WriteNumeric(ERR_NOPRIVILEGES, "Cannot use an SA command on a U-lined client");
				return CMD_FAILURE;
			}

			if ((!target) || (target->registered != REG_ALL))
			{
				user->WriteNotice("*** No such nickname: '" + parameters[0] + "'");
				return CMD_FAILURE;
			}

			if (!ServerInstance->IsNick(parameters[1]))
			{
				user->WriteNotice("*** Invalid nickname: '" + parameters[1] + "'");
				return CMD_FAILURE;
			}
		}

		/* Have we hit target's server yet? */
		if (target && IS_LOCAL(target))
		{
			std::string oldnick = user->nick;
			std::string newnick = target->nick;
			if (target->ChangeNick(parameters[1]))
			{
				ServerInstance->SNO.WriteGlobalSno('a', oldnick+" used SANICK to change "+newnick+" to "+parameters[1]);
			}
			else
			{
				ServerInstance->SNO.WriteGlobalSno('a', oldnick+" failed SANICK (from "+newnick+" to "+parameters[1]+")");
			}
		}

		return CMD_SUCCESS;
	}

	RouteDescriptor GetRouting(User* user, const Params& parameters) override
	{
		return ROUTE_OPT_UCAST(parameters[0]);
	}
};


class ModuleSanick : public Module
{
 private:
	CommandSanick cmd;

 public:
	ModuleSanick()
		: Module(VF_VENDOR | VF_OPTCOMMON, "Adds the /SANICK command which allows server operators to change the nickname of a user.")
		, cmd(this)
	{
	}
};

MODULE_INIT(ModuleSanick)
