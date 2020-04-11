/*
 * InspIRCd -- Internet Relay Chat Daemon
 *
 *   Copyright (C) 2019 Robby <robby@chatbelgie.be>
 *   Copyright (C) 2016 Johanna A <johanna-a@users.noreply.github.com>
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
#include "modules/isupport.h"

class ModuleClassBan
	: public Module
	, public ISupport::EventListener
{
 public:
	ModuleClassBan()
		: Module(VF_VENDOR | VF_OPTCOMMON, "Adds the n extended ban which check whether users are in a connect class matching the specified glob pattern.")
		, ISupport::EventListener(this)
	{
	}

	ModResult OnCheckBan(User* user, Channel* c, const std::string& mask) override
	{
		LocalUser* localUser = IS_LOCAL(user);
		if ((localUser) && (mask.length() > 2) && (mask[0] == 'n') && (mask[1] == ':'))
		{
			if (InspIRCd::Match(localUser->GetClass()->name, mask.substr(2)))
				return MOD_RES_DENY;
		}
		return MOD_RES_PASSTHRU;
	}

	void OnBuildISupport(ISupport::TokenMap& tokens) override
	{
		tokens["EXTBAN"].push_back('n');
	}
};

MODULE_INIT(ModuleClassBan)
