/*============================================================================

    This file is part of Lithium II Mod for Quake II
    Copyright (C) 1997, 1998, 1999, 2010 Matthew A. Ayres

    Lithium II Mod is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Lithium II Mod is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Lithium II Mod.  If not, see <http://www.gnu.org/licenses/>.

    Quake II is a trademark of Id Software, Inc.

    Code by Matt "WhiteFang" Ayres, matt@lithium.com

============================================================================*/

#include "g_local.h"

cvar_t *use_nocamp;
cvar_t *camp_threshold;
cvar_t *camp_time;
cvar_t *camp_warn;

void NoCamp_InitGame(void) {
	use_nocamp = gi.cvar("use_nocamp", "0", CVAR_LATCH);
	camp_threshold = gi.cvar("camp_threshold", "250", CVAR_LATCH);
	camp_time = gi.cvar("camp_time", "10.0", CVAR_LATCH);
	camp_warn = gi.cvar("camp_warn", "8.0", CVAR_LATCH);
}

void NoCamp_ClientThink(edict_t *ent, usercmd_t *ucmd) {
	gclient_t *client = ent->client;
	int i, move, sec;

	if(!use_nocamp->value || !camp_threshold->value || !camp_time->value)
		return;
	if(ent->deadflag != DEAD_NO)
		return;

    if(ent->flags != FL_OBSERVER) { return; }

	if(level.time < client->decamp_time) {
		if(client->decamp_count) {
			if(client->decamp_count < 4 && client->decamp_fire && (ucmd->forwardmove || ucmd->sidemove))
                gi.dprintf("");
			if(ucmd->buttons & BUTTON_ATTACK)
				client->decamp_fire++;
		}
		return;
	}

	client->decamp_time = level.time + 0.5;

	client->decamp_move[client->decamp_num] = 
		abs(ent->s.origin[0] - client->decamp_vec[0]) +
		abs(ent->s.origin[1] - client->decamp_vec[1]) +
		abs(ent->s.origin[2] - client->decamp_vec[2]);

	client->decamp_vec[0] = ent->s.origin[0];
	client->decamp_vec[1] = ent->s.origin[1];
	client->decamp_vec[2] = ent->s.origin[2];

	client->decamp_num++;
	if(client->decamp_num == camp_time->value * 2)
		client->decamp_num = 0;
	client->decamp_move[client->decamp_num] = 0;

	move = 0;
	for(i = 0; i < camp_time->value * 2; i++)
		move += client->decamp_move[i];

	if(move > camp_threshold->value * (camp_time->value - 2) / 5) {
		if(client->decamp_count && client->decamp_fire) {
			client->decamp_count = 0;
			client->decamp_fire = 0;
		}
		return;
	}

	sec = camp_warn->value - client->decamp_count / 2 + 2;
	if(sec >= 0 && sec <= camp_warn->value && client->decamp_count > 3)
        gi.dprintf("!!!! %s is doing Camping !!!!\n", ent->client->pers.netname);
        gi.dprintf(" Move your ass %s! (you still have %d seconds to comply)\n", ent->client->pers.netname, sec);

	if(client->decamp_count > camp_warn->value * 2 + 5) {
		//centerprintf(ent, "Killed for camping.");
        gi.dprintf(" %s Killed for camping!\n", ent->client->pers.netname);

		ent->health = 0;
		player_die(ent, ent, ent, 100000, vec3_origin);
		ent->deadflag = DEAD_DEAD;
		//respawn(ent);
	}

	client->decamp_count++;
}

