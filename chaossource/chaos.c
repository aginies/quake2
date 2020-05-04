#include "g_local.h"

qboolean Observer(edict_t *ent, qboolean check) 
{
    if (check && ent->deadflag != DEAD_DEAD && ent->health < ent->max_health) {
        gi.cprintf(ent, PRINT_HIGH, "You must be dead or have full health to become an observer.\n");
        return false;
    }

    if (ctf->value && ent->solid != SOLID_NOT) {
        CTFDeadDropFlag(ent);
        CTFDeadDropTech(ent);
        ent->client->resp.ctf_team = CTF_NOTEAM;
    }

    gi.setmodel(ent, "");
    ent->movetype = MOVETYPE_NOCLIP;
    ent->client->ps.gunindex = 0;
    ent->solid = SOLID_NOT;
    ///---------------
    // from MoveClientToIntermission
    ent->client->ps.gunindex = 0;
    ent->client->ps.blend[3] = 0;
    ent->client->ps.rdflags &= ~RDF_UNDERWATER;
    ent->client->showscores = true;
    ent->flags &= ~FL_OBSERVER;

    gi.linkentity(ent);
    gi.setmodel (ent, ent->model);

//    char string[1400] = "";
    //Com_sprintf(string, sizeof(string), "Press F8 to play again", 32, 0, NULL);
//    gi.WriteByte(svc_layout);
//    gi.WriteString(string);
//   gi.unicast (ent, true);

    return true;
}

void Chaos_SetStats(edict_t *self) {

    edict_t *ent = self;
    short *stats = self->client->ps.stats;

    // 25 already used
    stats[10] = ent->client->pers.inventory[ITEM_INDEX(FindItem("shells"))];
    stats[29] = ent->client->pers.inventory[ITEM_INDEX(FindItem("explosive shells"))];
    stats[12] = ent->client->pers.inventory[ITEM_INDEX(FindItem("slugs"))];
    stats[27] = ent->client->pers.inventory[ITEM_INDEX(FindItem("buzzes"))];

    stats[26] = ent->client->pers.inventory[ITEM_INDEX(FindItem("grenades"))];
    stats[15] = ent->client->pers.inventory[ITEM_INDEX(FindItem("flash grenades"))];
    stats[16] = ent->client->pers.inventory[ITEM_INDEX(FindItem("poison grenades"))];
    stats[17] = ent->client->pers.inventory[ITEM_INDEX(FindItem("Laser Mines"))];
    stats[18] = ent->client->pers.inventory[ITEM_INDEX(FindItem("proximity mines"))];

    stats[19] = ent->client->pers.inventory[ITEM_INDEX(FindItem("arrows"))];
    stats[20] = ent->client->pers.inventory[ITEM_INDEX(FindItem("poison arrows"))];
    stats[21] = ent->client->pers.inventory[ITEM_INDEX(FindItem("explosive arrows"))];

    stats[22] = ent->client->pers.inventory[ITEM_INDEX(FindItem("rockets"))];
    stats[23] = ent->client->pers.inventory[ITEM_INDEX(FindItem("homing missiles"))];

    stats[24] = ent->client->pers.inventory[ITEM_INDEX(FindItem("cells"))];

//    stats[27] = ent->client->pers.inventory[ITEM_INDEX(FindItem("nuke"))];
//    stats[28] = ent->client->pers.inventory[ITEM_INDEX(FindItem("gravity vortex"))];

    if(ctf->value)
        SetCTFStats(ent, stats);
}

//===============================
// statusbar

void StatusBar_UpdateAll(void) {
    int i;
    edict_t *ent;

    for(i = 0; i < game.maxclients; i++) {
        ent = g_edicts + 1 + i;
        if(!ent->inuse)
            continue;
        return;
    }
}

char *ammobar =
"yt 40 xr -24 picn a_shells xr -74 num 3 10 "
"yt 64 xr -24 picn a_eshells xr -74 num 3 29 "
"yt 88 xr -24 picn a_slugs xr -74 num 3 12 "
"yt 112 xr -24 picn a_buzz xr -74 num 3 27 "
"yt 160 xr -24 picn a_grenades1 xr -74 num 3 26 "
"yt 184 xr -24 picn a_fgrenades xr -74 num 3 15 "
"yt 208 xr -24 picn a_pgrenades xr -74 num 3 16 "
"yt 232 xr -24 picn a_lmines xr -74 num 3 17 "
"yt 256 xr -24 picn a_xgrenades xr -74 num 3 18 "
"yt 40 xl 0 picn a_arrows xl 24 num 3 19 "
"yt 64 xl 0 picn a_parrows xl 24 num 3 20 "
"yt 88 xl 0 picn a_earrows xl 24 num 3 21 "
"yt 136 xl 0 picn a_rockets xl 24 num 3 22 "
"yt 160 xl 0 picn a_grockets xl 24 num 3 23 "
"yt 208 xl 0 picn a_cells xl 24 num 3 24 "
;
//"yt 232 xl 0 picn a_nuke xl 24 num 3 27"
//"yt 256 xl 0 picn a_vortex xl 24 num 3 28"

char *bottombar =
"yb -24 "

// ammo
"if 2 xv 100 anum xv 150 pic 2 endif "

// armor
"if 4 xv 200 rnum xv 250 pic 4 endif "

// selected item
"if 6 xv 296 pic 6 endif "

"yb -50 "

// timer
"if 9 xv 246 num 2 10 xv 296 pic 9 endif "

//  help / weapon icon
"if 11 xv 148 pic 11 endif "

// picked up item
"if 7 xv 0 pic 7 xv 26 yb -42 stat_string 8 endif "

// config string output (observing / safety msg)
"if 30 yb -16 xv 8 stat_string 30 endif "

// health
"if 1 yb -24 xv 0 hnum xv 50 pic 0 endif "
;

char *chaos_statusbar =
"yb -24 "

// health
"xv 0 hnum xv 50 pic 0 "

// ammo
"if 2 xv 100 anum xv 150 pic 2 endif "

// armor
"if 4 xv 200 rnum xv 250 pic 4 endif "

// config string output (observing / safety msg)
"if 30 yb -16 xv 8 stat_string 30 endif "

// selected item
"if 6 xv  296 pic 6 endif "

"yb -50 "

// picked up item
"if 7 xv  0 pic 7 xv  26 yb  -42 stat_string 8 yb  -50 endif "

// timer
"if 9 xv 246 num 2 10 xv 296 pic 9 endif "

//  help / weapon icon 
"if 11 xv 148 pic 11 endif "

//  frags
//"yt 136 xl 0 picn a_rockets xl 24 num 3 22 "
"yt 304 xl 0 picn p_kamikaze xl 24 num 3 14 "

//tech
//"yt 120 if 26 xr -26 pic 26 endif "

// id view state
//"if 27 xv 0 yb -58 string \"Viewing\" xv 64  stat_string 27 endif "
;


int StatusBar_Update(edict_t *ent) {
    char statusbar[1400] = "";

    if(!ctf->value) 
    {
        strlcat(statusbar, ammobar, sizeof(statusbar));
        strlcat(statusbar, chaos_statusbar, sizeof(statusbar));
    }
    else if(ctf->value) {
        strlcat(statusbar, ammobar, sizeof(statusbar));
        strlcat(statusbar, ctf_statusbar, sizeof(statusbar));
    }

    gi.configstring (CS_STATUSBAR, ctf_statusbar);

    gi.WriteByte(0x0D);
    gi.WriteShort(5);
    gi.WriteString(statusbar);
    gi.unicast(ent, false);
    return strlen(statusbar);
}
