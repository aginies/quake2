#include "g_local.h"
#include "stdlog.h"
#include "chaos.h"



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

    ent->chaos_flags = CHAOS_OBSERVER;
    ent->takedamage = DAMAGE_NO;

    gi.setmodel(ent, "");
    ent->movetype = MOVETYPE_NOCLIP;
    ent->client->ps.gunindex = 0;
    ent->solid = SOLID_NOT;
    ent->client->ps.gunindex = 0;
    ent->client->ps.blend[3] = 0;
    ent->client->ps.rdflags &= ~RDF_UNDERWATER;
    ent->client->showscores = true;
    ent->flags &= ~FL_OBSERVER;

    gi.linkentity(ent);
    gi.setmodel (ent, ent->model);
//    gi.configstring(CS_OBSERVING, "Observing.  FIRE to join.  0 for menu.");
//    gi.unicast (ent, false);
//   gi.unicast (ent, true);

//    if((key == 0) || (key == 1)) {
//      ChaosOpenMenu(ent);
//      return true;
//    }

    return true;
}

qboolean IsObserver(edict_t *ent) {
    return ent->chaos_flags & CHAOS_OBSERVER;
}

void Chaos_SetStats(edict_t *self) {
    edict_t *ent = self;
    short *stats = self->client->ps.stats;

    if(!level.intermissiontime) {
    //  already used
    // 1 2 4 6 7 8 9 10 11 15 25 26 27 30
    // Don't use index > 31 this will cause error
//    stats[12] = ent->client->pers.inventory[ITEM_INDEX(FindItem("shells"))];
    stats[29] = ent->client->pers.inventory[ITEM_INDEX(FindItem("explosive shells"))];
    stats[12] = ent->client->pers.inventory[ITEM_INDEX(FindItem("slugs"))];
    stats[28] = ent->client->pers.inventory[ITEM_INDEX(FindItem("buzzes"))];

    stats[31] = ent->client->pers.inventory[ITEM_INDEX(FindItem("grenades"))];
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

    stats[11] = ent->client->pers.inventory[ITEM_INDEX(FindItem("nuke"))];
    stats[27] = ent->client->pers.inventory[ITEM_INDEX(FindItem("gravity vortex"))];
    }

    if(ctf->value)
        SetCTFStats(ent, stats);
}

// statusbar
//"if 12 yt 108 xr -24 picn a_shells xr -74 num 3 12 endif "

char *ammobar =
"if 18 yb -156 xr -24 picn a_xgrenades xr -74 num 3 18 endif "
"if 17 yb -122 xr -24 picn a_lmines xr -74 num 3 17 endif "
"if 29 yt 84 xr -24 picn a_eshells xr -74 num 3 29 endif "
"if 12 yt 156 xr -24 picn a_slugs xr -74 num 3 12 endif "
"if 28 yt 180 xr -24 picn a_buzz xr -74 num 3 28 endif "
"if 22 yt 136 xl 0 picn a_rockets xl 24 num 3 22 endif "
"if 23 yt 160 xl 0 picn a_grockets xl 24 num 3 23 endif "
"if 24 yt 208 xl 0 picn a_cells xl 24 num 3 24 endif "
;

char *arraowsbar =
"if 19 yb -74 xl 0 picn a_arrows xl 24 num 3 19 endif "
"if 20 yb -50 xl 0 picn a_parrows xl 24 num 3 20 endif "
"if 21 yb -98 xl 0 picn a_earrows xl 24 num 3 21 endif "
;

char *grenadesbar =
"if 31 yb -98 xr -24 picn a_grenades1 xr -74 num 3 31 endif "
"if 15 yb -74 xr -24 picn a_fgrenades xr -74 num 3 15 endif "
"if 16 yb -50 xr -24 picn a_pgrenades xr -74 num 3 16 endif "
;

char *nukevortex =
"if 11 yt 88 xl 0 picn a_nuke xl 24 num 3 11 endif "
"if 27 yt 112 xl 0 picn a_vortex xl 24 num 3 27 endif "
;

char *fragsbar =
//  frags
"yt 40 xl 0 picn p_kamikaze xl 24 num 3 14 "
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
//"if 30 yb -16 xv 8 stat_string 30 endif "

// selected item
"if 6 xv 296 pic 6 endif "

"yb -50 "

// picked up item
//"if 7 xv 0 pic 7 xv 26 yb -42 stat_string 8 yb -50 endif "

// timer
"if 9 xv 246 num 2 10 xv 296 pic 9 endif "

//  help / weapon icon
//"if 11 xv 148 pic 11 endif "

//tech
"yt 120 if 26 xr -26 pic 26 endif "

// id view state
//"if 27 xv 0 yb -58 string \"Viewing\" xv 64  stat_string 27 endif "
;


int StatusBar_Update(edict_t *ent) {
    char statusbar[1400];

    if((ent->chaos_flags |= CHAOS_MAINMENU)) {
//        gi.dprintf("DEBUG dans le menu %i !\n", ent->chaos_flags);   
        return 0;
    }
//    else
//    {
//        gi.dprintf("DEBUG PAS dans menu !\n");   
//    }


    if (strcmp(ent->classname, "player") == 0)
    {
    if(!ctf->value) 
    {
        	strlcat(statusbar, ammobar, sizeof(statusbar));
        	strlcat(statusbar, fragsbar, sizeof(statusbar));
        	strlcat(statusbar, arraowsbar, sizeof(statusbar));
        	strlcat(statusbar, grenadesbar, sizeof(statusbar));
        	strlcat(statusbar, nukevortex, sizeof(statusbar));
        strlcat(statusbar, chaos_statusbar, sizeof(statusbar));
    }
    else if(ctf->value) {
	    if (!ent->client->showammo)
        	strlcat(statusbar, ammobar, sizeof(statusbar));
        	strlcat(statusbar, fragsbar, sizeof(statusbar));
        	strlcat(statusbar, arraowsbar, sizeof(statusbar));
        	strlcat(statusbar, grenadesbar, sizeof(statusbar));
        	strlcat(statusbar, nukevortex, sizeof(statusbar));
        strlcat(statusbar, ctf_statusbar, sizeof(statusbar));
    }

//    gi.WriteByte(0x0D);
//    gi.WriteShort(5);
//    gi.WriteString(statusbar);
//    gi.unicast(ent, false);
    return strlen(statusbar);
    }
    return 0;
}

int Layout_Update(edict_t *ent) {
    char statusbar[1400] = "";
    
    ent->chaos_flags = CHAOS_STATUSBAR;

    if (strcmp(ent->classname, "player") == 0)
      {
	if(!ctf->value) 
	  {
	    strlcat(statusbar, ammobar, sizeof(statusbar));
	    strlcat(statusbar, fragsbar, sizeof(statusbar));
	    strlcat(statusbar, nukevortex, sizeof(statusbar));
	    strlcat(statusbar, arraowsbar, sizeof(statusbar));
	    strlcat(statusbar, grenadesbar, sizeof(statusbar));
	    strlcat(statusbar, chaos_statusbar, sizeof(statusbar));
	  }
	else if(ctf->value) {
	  strlcat(statusbar, ammobar, sizeof(statusbar));
	  strlcat(statusbar, fragsbar, sizeof(statusbar));
	  strlcat(statusbar, nukevortex, sizeof(statusbar));
	  strlcat(statusbar, arraowsbar, sizeof(statusbar));
	  strlcat(statusbar, grenadesbar, sizeof(statusbar));
	  strlcat(statusbar, ctf_statusbar, sizeof(statusbar));
    }

    gi.configstring (CS_STATUSBAR, statusbar);
    gi.unicast(ent, false);
      }
    return strlen(statusbar);
}

//
// MENU
//

void ChaosReturnToMain(edict_t *ent, pmenu_t *p)
{
        PMenu_Close(ent);
        ChaosOpenMenu(ent);
}

pmenu_t ccreditsmenu[] = {
        { "*Chaos Deathmatch",PMENU_ALIGN_CENTER, NULL, NULL },
        { NULL,PMENU_ALIGN_CENTER, NULL, NULL },
        { "*Programming",PMENU_ALIGN_CENTER, NULL, NULL },
        { "Flash (flash@telefragged.com)",PMENU_ALIGN_CENTER, NULL, NULL },
        { NULL,PMENU_ALIGN_LEFT, NULL, NULL },
        { "*Current devel",PMENU_ALIGN_CENTER, NULL, NULL },
        { "Guibo (guibo@guibo.com)",PMENU_ALIGN_CENTER, NULL, NULL },
        { NULL,PMENU_ALIGN_LEFT, NULL, NULL },
        { "*Level Design",PMENU_ALIGN_CENTER, NULL, NULL },
        { "Nat (nnp@greennet.net)",PMENU_ALIGN_CENTER, NULL, NULL },
        { "Craig (car188@psu.edu)",PMENU_ALIGN_CENTER, NULL, NULL },
        { NULL,PMENU_ALIGN_LEFT, NULL, NULL },
        { "*Art",PMENU_ALIGN_CENTER, NULL, NULL },
        { "SPA (spa@telefragged.com)",PMENU_ALIGN_CENTER, NULL, NULL },
        { NULL,PMENU_ALIGN_LEFT, NULL, NULL },
        { "*Sound",PMENU_ALIGN_CENTER, NULL, NULL },
        { "SPA (spa@telefragged.com)",PMENU_ALIGN_CENTER, NULL, NULL },
        { "Nat (nnp@greennet.net)",PMENU_ALIGN_CENTER, NULL, NULL },
        { NULL,PMENU_ALIGN_CENTER, NULL, NULL },
        { "Retourner Menu Principal",PMENU_ALIGN_LEFT, NULL, ChaosReturnToMain }
};

void ChaosCredits(edict_t *ent, pmenu_t *p)
{
        PMenu_Close(ent);
        PMenu_Open(ent, ccreditsmenu, -1, sizeof(ccreditsmenu) / sizeof(pmenu_t));
}

pmenu_t interfacemenu[] = {
        { "*Interface",PMENU_ALIGN_CENTER, NULL, NULL },
        { NULL,PMENU_ALIGN_CENTER, NULL, NULL },
        { NULL,PMENU_ALIGN_CENTER, NULL, NULL },
        { NULL,PMENU_ALIGN_CENTER, NULL, NULL },
        { NULL,PMENU_ALIGN_CENTER, NULL, NULL },
        { "L'interface montre",PMENU_ALIGN_LEFT, NULL, NULL },
        { "*Automatiquement",PMENU_ALIGN_CENTER, NULL, NULL },
        { "les Munitions des",PMENU_ALIGN_LEFT, NULL, NULL },
        { "armes disponibles.",PMENU_ALIGN_LEFT, NULL, NULL },
        { NULL,PMENU_ALIGN_CENTER, NULL, NULL },
        { NULL,PMENU_ALIGN_CENTER, NULL, NULL },
        { NULL,PMENU_ALIGN_CENTER, NULL, NULL },
        { NULL,PMENU_ALIGN_CENTER, NULL, NULL },
        { NULL,PMENU_ALIGN_CENTER, NULL, NULL },
        { "Retourner Menu Principal",PMENU_ALIGN_LEFT, NULL, ChaosReturnToMain }
};

pmenu_t helpmenu[] = {
        { "*Commande de Console",PMENU_ALIGN_CENTER, NULL, NULL },
        { NULL,PMENU_ALIGN_CENTER, NULL, NULL },
        { NULL,PMENU_ALIGN_CENTER, NULL, NULL },
        { "*pull",PMENU_ALIGN_LEFT, NULL, NULL },
        { "Force pour Pousser ",PMENU_ALIGN_LEFT, NULL, NULL },
        { "*push",PMENU_ALIGN_LEFT, NULL, NULL },
        { "Force pour Attirer",PMENU_ALIGN_LEFT, NULL, NULL },
        { "*fkill",PMENU_ALIGN_LEFT, NULL, NULL },
        { "DarK Vador Force!",PMENU_ALIGN_LEFT, NULL, NULL },
        { NULL,PMENU_ALIGN_CENTER, NULL, NULL },
        { "*observer",PMENU_ALIGN_LEFT, NULL, NULL },
        { "Camera libre",PMENU_ALIGN_LEFT, NULL, NULL },
        { "F7 F8 quitter camera",PMENU_ALIGN_RIGHT, NULL, NULL },
        { NULL,PMENU_ALIGN_CENTER, NULL, NULL },
        { "*Dans votre Configuration",PMENU_ALIGN_LEFT, NULL, NULL },
        { "bind MOUSE4 ''fkill''",PMENU_ALIGN_LEFT, NULL, NULL },
        { "bind MOUSE5 ''pull''",PMENU_ALIGN_LEFT, NULL, NULL },
        { "bind F12 ''showammo''",PMENU_ALIGN_LEFT, NULL, NULL },
        { NULL,PMENU_ALIGN_CENTER, NULL, NULL },
        { "Retourner Menu Principal",PMENU_ALIGN_LEFT, NULL, ChaosReturnToMain }
};


pmenu_t mainmenu[] = {
        { "*Chaos Deathmatch",PMENU_ALIGN_CENTER, NULL, NULL },
        { "Devel Version 1.16",PMENU_ALIGN_RIGHT, NULL, NULL },
        { NULL,PMENU_ALIGN_CENTER, NULL, NULL },
        { "* ! VERSION XP !",PMENU_ALIGN_CENTER, NULL, NULL },
        { " ! Reportez moi les Bugs !",PMENU_ALIGN_CENTER, NULL, NULL },
        { NULL,PMENU_ALIGN_CENTER, NULL, NULL },
        { "Afficher ce menu: menu",PMENU_ALIGN_LEFT, NULL, NULL },
        { NULL,PMENU_ALIGN_LEFT, NULL, NULL },
        { NULL,PMENU_ALIGN_LEFT, NULL, NULL },
        { "*Bots",PMENU_ALIGN_LEFT, NULL,MenuBots },
        { NULL,PMENU_ALIGN_CENTER, NULL, NULL },
        { "*Interface",PMENU_ALIGN_LEFT, NULL, MenuInterface },
        { NULL,PMENU_ALIGN_CENTER, NULL, NULL },
        { "*Aide",PMENU_ALIGN_LEFT, NULL, MenuHelp },
        { NULL,PMENU_ALIGN_CENTER, NULL, NULL },
        { "*Credits",PMENU_ALIGN_LEFT, NULL, ChaosCredits },
        { NULL,PMENU_ALIGN_LEFT, NULL, NULL },
        { NULL,PMENU_ALIGN_LEFT, NULL, NULL },
        { "[^] et [$] pour le curseur", PMENU_ALIGN_LEFT, NULL, NULL },
        { "[ENTER] Selection",PMENU_ALIGN_LEFT, NULL, NULL },
        { "[ESC] Sortir",PMENU_ALIGN_LEFT, NULL, NULL },
};

pmenu_t botsmenu[] = {
        { "*Bots Administration",PMENU_ALIGN_CENTER, NULL, NULL },
        { NULL,PMENU_ALIGN_CENTER, NULL, NULL },
        { NULL,PMENU_ALIGN_CENTER, NULL, NULL },
        { "*Sur serveur distant",PMENU_ALIGN_LEFT, NULL, NULL },
        { "*Password Obligatoire!",PMENU_ALIGN_LEFT, NULL, NULL },
        { "Exemple:",PMENU_ALIGN_LEFT, NULL, NULL },
        { "seta rconpassword ''PASS''",PMENU_ALIGN_CENTER, NULL, NULL },
        { NULL,PMENU_ALIGN_LEFT, NULL, NULL },
        { NULL,PMENU_ALIGN_CENTER, NULL, NULL },
        { NULL,PMENU_ALIGN_LEFT, NULL, NULL },
        { "Ajouter un Bot (niveau 1)",PMENU_ALIGN_LEFT, NULL, AddBotsCmd },
        { "Ajouter un Bot (niveau 2)",PMENU_ALIGN_LEFT, NULL, AddBotsCmd2 },
        { NULL,PMENU_ALIGN_CENTER, NULL, NULL },
        { "Retirer Tous les bots",PMENU_ALIGN_LEFT, NULL, KillBotsCmd },
        { NULL,PMENU_ALIGN_LEFT, NULL, NULL },
        { NULL,PMENU_ALIGN_LEFT, NULL, NULL },
        { NULL,PMENU_ALIGN_LEFT, NULL, NULL },
        { "Retourner Menu Principal",PMENU_ALIGN_LEFT, NULL, ChaosReturnToMain }
};



void ChaosOpenMenu(edict_t *ent)
{
        PMenu_Open(ent, mainmenu, 1, sizeof(mainmenu) / sizeof(pmenu_t));
}

void MenuInterface(edict_t *ent, pmenu_t *p)
{
        PMenu_Close(ent);
        PMenu_Open(ent, interfacemenu, -1, sizeof(interfacemenu) / sizeof(pmenu_t));
}

void MenuHelp(edict_t *ent, pmenu_t *p)
{
        PMenu_Close(ent);
        PMenu_Open(ent, helpmenu, -1, sizeof(helpmenu) / sizeof(pmenu_t));
}

void MenuBots(edict_t *ent, pmenu_t *p)
{
        PMenu_Close(ent);
        PMenu_Open(ent, botsmenu, -1, sizeof(botsmenu) / sizeof(pmenu_t));
}

void KillBotsCmd (edict_t *ent, pmenu_t *p)
{
    stuffcmd(ent, "rcon sv killbot all\n");
}

void AddBotsCmd (edict_t *ent, pmenu_t *p)
{
    stuffcmd(ent, "rcon sv addbots 1 1\n");
}
void AddBotsCmd2 (edict_t *ent, pmenu_t *p)
{
    stuffcmd(ent, "rcon sv addbots 1 2\n");
}

