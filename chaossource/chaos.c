#include "g_local.h"
#include "stdlog.h"
#include "chaos.h"



int shells = 10000;

void ShellTouch(edict_t *self, edict_t *other, cplane_t *plane, csurface_t *surf)
{
    if (random() < 0.5)
        gi.sound (self, CHAN_WEAPON, gi.soundindex ("weapons/tink1.wav"), 1, ATTN_STATIC, 0);
    else
        gi.sound (self, CHAN_WEAPON, gi.soundindex ("weapons/tink2.wav"), 1, ATTN_STATIC, 0);
}


void ShellDie(edict_t *self)
{
    G_FreeEdict(self);
    --shells;
}

void EjectShell (edict_t *self, vec3_t start, int toggle )
{
    edict_t *shell;
    vec3_t  forward, right, up;
    float           r;

    if (sv_shelloff->value)
        return;

    gi.dprintf("DEBUG EjectShell !\n");

    shell = G_Spawn();
    ++shells;

    AngleVectors (self->client->v_angle, forward, right, up);

        // zucc spent a fair amount of time hacking these until they look ok,
        // several of them could be improved however.


    VectorMA (start, ((toggle==1)?-4:4), right, start);
    VectorMA (start, 6, forward, start);
    VectorMA (start, -9, up, start);


    if ( (forward[2] >= -1) && (forward[2] < -0.99) ) {
        VectorMA (start, 5, forward, start);
        VectorMA (start, -0.5, up, start); }

    else if ( (forward[2] >= -0.99) && (forward[2] < -0.98) ) {
        VectorMA (start, 5, forward, start);
        VectorMA (start, -.1, up, start); }
    else if ( (forward[2] >= -0.98) && (forward[2] < -0.97) ) {
        VectorMA (start, 5.1, forward, start);
        VectorMA (start, 0.3, up, start); }
    else if ( (forward[2] >= -0.97) && (forward[2] < -0.96) ) {
        VectorMA (start, 5.2, forward, start);
        VectorMA (start, 0.7, up, start); }
    else if ( (forward[2] >= -0.96) && (forward[2] < -0.95) ) {
        VectorMA (start, 5.2, forward, start);
        VectorMA (start, 1.1, up, start); }
   else if ( (forward[2] >= -0.95) && (forward[2] < -0.94) ) {
        VectorMA (start, 5.3, forward, start);
        VectorMA (start, 1.5, up, start); }
    else if ( (forward[2] >= -0.94) && (forward[2] < -0.93) ) {
        VectorMA (start, 5.4, forward, start);
        VectorMA (start, 1.9, up, start); }
    else if ( (forward[2] >= -0.93) && (forward[2] < -0.92) ) {
        VectorMA (start, 5.5, forward, start);
        VectorMA (start, 2.3, up, start); }
    else if ( (forward[2] >= -0.92) && (forward[2] < -0.91) ) {
        VectorMA (start, 5.6, forward, start);
        VectorMA (start, 2.7, up, start); }
    else if ( (forward[2] >= -0.91) && (forward[2] < -0.9) ) {
        VectorMA (start, 5.7, forward, start);
        VectorMA (start, 3.1, up, start); }

    else if ( (forward[2] >= -0.9) && (forward[2] < -0.85) ) {
        VectorMA (start, 5.8, forward, start);
        VectorMA (start, 3.5, up, start); }
    else if ( (forward[2] >= -0.85) && (forward[2] < -0.8) ) {
        VectorMA (start, 6, forward, start);
        VectorMA (start, 4, up, start); }
    else if ( (forward[2] >= -0.8) && (forward[2] < -0.6) ) {
        VectorMA (start, 6.5, forward, start);
        VectorMA (start, 4.5, up , start); }
    else if ( (forward[2] >= -0.6) && (forward[2] < -0.4) ) {
        VectorMA (start, 8, forward, start);
        VectorMA (start, 5.5, up , start); }
    else if ( (forward[2] >= -0.4) && (forward[2] < -0.2) ) {
        VectorMA (start, 9.5, forward, start);
        VectorMA (start, 6, up , start); }
    else if ( (forward[2] >= -0.2) && (forward[2] < 0) ) {
        VectorMA (start, 11, forward, start);
       VectorMA (start, 6.5, up , start); }
    else if ( (forward[2] >= 0) && (forward[2] < 0.2) ) {
        VectorMA (start, 12, forward, start);
        VectorMA (start, 7, up, start); }
    else if ( (forward[2] >= 0.2) && (forward[2] < 0.4) ) {
        VectorMA (start, 14, forward, start);
        VectorMA (start, 6.5, up, start); }
    else if ( (forward[2] >= 0.4) && (forward[2] < 0.6) ) {
        VectorMA (start, 16, forward, start);
        VectorMA (start, 6, up, start); }
    else if ( (forward[2] >= 0.6) && (forward[2] < 0.8) ) {
        VectorMA (start, 18, forward, start);
        VectorMA (start, 5, up, start); }
    else if ( (forward[2] >= 0.8) && (forward[2] < 0.85) ) {
        VectorMA (start, 18, forward, start);
        VectorMA (start, 4, up, start); }
    else if ( (forward[2] >= 0.85) && (forward[2] < 0.9) ) {
        VectorMA (start, 18, forward, start);
        VectorMA (start, 2.5, up, start); }

    else if ( (forward[2] >= 0.9) && (forward[2] < 0.91) ) {
        VectorMA (start, 18.2, forward, start);
        VectorMA (start, 2.2, up, start); }
    else if ( (forward[2] >= 0.91) && (forward[2] < 0.92) ) {
        VectorMA (start, 18.4, forward, start);
        VectorMA (start, 1.9, up, start); }
    else if ( (forward[2] >= 0.92) && (forward[2] < 0.93) ) {
        VectorMA (start, 18.6, forward, start);
        VectorMA (start, 1.6, up, start); }
    else if ( (forward[2] >= 0.93) && (forward[2] < 0.94) ) {
        VectorMA (start, 18.8, forward, start);
        VectorMA (start, 1.3, up, start); }
    else if ( (forward[2] >= 0.94) && (forward[2] < 0.95) ) {
        VectorMA (start, 19, forward, start);
        VectorMA (start, 1, up, start); }
   else if ( (forward[2] >= 0.95) && (forward[2] < 0.96) ) {
        VectorMA (start, 19.2, forward, start);
        VectorMA (start, 0.7, up, start); }
    else if ( (forward[2] >= 0.96) && (forward[2] < 0.97) ) {
        VectorMA (start, 19.4, forward, start);
        VectorMA (start, 0.4, up, start); }
    else if ( (forward[2] >= 0.97) && (forward[2] < 0.98) ) {
        VectorMA (start, 19.6, forward, start);
        VectorMA (start, -0.2, up, start); }
    else if ( (forward[2] >= 0.98) && (forward[2] < 0.99) ) {
        VectorMA (start, 19.8, forward, start);
        VectorMA (start, -0.6, up, start); }

    else if ( (forward[2] >= 0.99) && (forward[2] <= 1) ) {
        VectorMA (start, 20, forward, start);
        VectorMA (start, -1, up , start); }

    VectorCopy (start , shell->s.origin);

    VectorMA (shell->velocity, (toggle==1?-1:1)*(35 + random() * 60), right, shell->velocity);

    VectorMA (shell->avelocity, 500, right, shell->avelocity);

    shell->movetype = MOVETYPE_BOUNCE;
    shell->solid = SOLID_BBOX;

    r = random();
    if (r < 0.1)
        shell->s.frame = 0;
    else if (r < 0.2)
        shell->s.frame = 1;
    else if (r < 0.3)
        shell->s.frame = 2;
    else if (r < 0.5)
        shell->s.frame = 3;
    else if (r < 0.6)
        shell->s.frame = 4;
    else if (r < 0.7)
        shell->s.frame = 5;
    else if (r < 0.8)
      shell->s.frame = 6;
    else if (r < 0.9)
        shell->s.frame = 7;
    else
        shell->s.frame = 8;

    shell->owner = self;
    shell->touch = ShellTouch;
    shell->nextthink = level.time + 1.2 - (shells * .05);
    shell->think = ShellDie;
    shell->classname = "shell";

    gi.linkentity (shell);
}



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
    // Index already used for various reasons, will pickup some not mandatory
    // reassigning the stats[NUMBER]
    // 1 2 4 6 7 8 9 10 11 15 25 26 27 30
    // Don't use index > 31 this will cause error this needs a change in the quake2 client
    // and in the mods, so not very compatible.... 
    // q_shared.h:#define	MAX_STATS				32
    // q_shared.h:	short		stats[MAX_STATS];
    //
    //stats[12] = ent->client->pers.inventory[ITEM_INDEX(FindItem("shells"))];
    stats[29] = ent->client->pers.inventory[ITEM_INDEX(FindItem("explosive shells"))];
    stats[12] = ent->client->pers.inventory[ITEM_INDEX(FindItem("slugs"))];
    stats[28] = ent->client->pers.inventory[ITEM_INDEX(FindItem("buzzes"))];
    stats[30] = ent->client->pers.inventory[ITEM_INDEX(FindItem("c4"))];

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
"if 30 yt 64 xl 0 picn a_c4cells xl 24 num 3 30 endif "
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
"if 7 xv 0 pic 7 xv 26 yb -42 stat_string 8 yb -50 endif "

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
        { "*Current devel > 1.16.1",PMENU_ALIGN_CENTER, NULL, NULL },
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
        { "*  Dynamiquement",PMENU_ALIGN_CENTER, NULL, NULL },
        { "les Munitions des",PMENU_ALIGN_LEFT, NULL, NULL },
        { "armes disponibles.",PMENU_ALIGN_LEFT, NULL, NULL },
        { NULL,PMENU_ALIGN_CENTER, NULL, NULL },
        { "Si il n'y a plus de ",PMENU_ALIGN_LEFT, NULL, NULL },
        { "munitions son status",PMENU_ALIGN_LEFT, NULL, NULL },
        { "disparait.",PMENU_ALIGN_LEFT, NULL, NULL },
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
//        { "*observer",PMENU_ALIGN_LEFT, NULL, NULL },
//        { "Camera libre",PMENU_ALIGN_LEFT, NULL, NULL },
//        { "F7 F8 quitter camera",PMENU_ALIGN_RIGHT, NULL, NULL },
        { NULL,PMENU_ALIGN_CENTER, NULL, NULL },
        { "*Dans votre Configuration",PMENU_ALIGN_LEFT, NULL, NULL },
        { NULL,PMENU_ALIGN_CENTER, NULL, NULL },
        { "bind MOUSE2 ''push''",PMENU_ALIGN_LEFT, NULL, NULL },
        { "bind MOUSE3 ''pull''",PMENU_ALIGN_LEFT, NULL, NULL },
        { "bind MOUSE4 ''fkill''",PMENU_ALIGN_LEFT, NULL, NULL },
        { "bind F12 ''menu''",PMENU_ALIGN_LEFT, NULL, NULL },
        { NULL,PMENU_ALIGN_CENTER, NULL, NULL },
        { "Retourner Menu Principal",PMENU_ALIGN_LEFT, NULL, ChaosReturnToMain }
};

pmenu_t helpservermenu[] = {
        { "*Parametres serveur",PMENU_ALIGN_CENTER, NULL, NULL },
        { NULL,PMENU_ALIGN_CENTER, NULL, NULL },
        { NULL,PMENU_ALIGN_CENTER, NULL, NULL },
        { "*fast_respawn [SECONDE]",PMENU_ALIGN_LEFT, NULL, NULL },
        { "permet de respawn les armes",PMENU_ALIGN_LEFT, NULL, NULL },
        { "dans le délaie de X secondes",PMENU_ALIGN_LEFT, NULL, NULL },
        { NULL,PMENU_ALIGN_CENTER, NULL, NULL },
        { "*Dans votre ligne de commande:",PMENU_ALIGN_LEFT, NULL, NULL },
        { NULL,PMENU_ALIGN_CENTER, NULL, NULL },
        { "+set fast_respawn 4",PMENU_ALIGN_LEFT, NULL, NULL },
        { NULL,PMENU_ALIGN_CENTER, NULL, NULL },
        { NULL,PMENU_ALIGN_CENTER, NULL, NULL },
        { "Retourner Menu Principal",PMENU_ALIGN_LEFT, NULL, ChaosReturnToMain }
};



pmenu_t mainmenu[] = {
        { "*Chaos Deathmatch",PMENU_ALIGN_CENTER, NULL, NULL },
        { "Devel Version 1.16.1",PMENU_ALIGN_RIGHT, NULL, NULL },
        { NULL,PMENU_ALIGN_CENTER, NULL, NULL },
//        { "* ! VERSION XP !",PMENU_ALIGN_CENTER, NULL, NULL },
        { " ! Reportez moi les Bugs !",PMENU_ALIGN_CENTER, NULL, NULL },
        { NULL,PMENU_ALIGN_CENTER, NULL, NULL },
        { "Afficher ce menu: menu",PMENU_ALIGN_LEFT, NULL, NULL },
        { NULL,PMENU_ALIGN_LEFT, NULL, NULL },
        { "+Bots",PMENU_ALIGN_LEFT, NULL,MenuBots },
        { NULL,PMENU_ALIGN_CENTER, NULL, NULL },
        { "+Interface",PMENU_ALIGN_LEFT, NULL, MenuInterface },
        { NULL,PMENU_ALIGN_CENTER, NULL, NULL },
        { "+Aide",PMENU_ALIGN_LEFT, NULL, MenuHelp },
        { NULL,PMENU_ALIGN_CENTER, NULL, NULL },
        { "+Aide Serveur",PMENU_ALIGN_LEFT, NULL, MenuServerHelp },
        { NULL,PMENU_ALIGN_CENTER, NULL, NULL },
        { "+Credits",PMENU_ALIGN_LEFT, NULL, ChaosCredits },
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
        { "+Ajouter un Bot (niveau 1)",PMENU_ALIGN_LEFT, NULL, AddBotsCmd },
        { "+Ajouter un Bot (niveau 2)",PMENU_ALIGN_LEFT, NULL, AddBotsCmd2 },
        { "+Ajouter un Bot (niveau 3)",PMENU_ALIGN_LEFT, NULL, AddBotsCmd3 },
        { NULL,PMENU_ALIGN_CENTER, NULL, NULL },
        { "+Retirer Tous les bots",PMENU_ALIGN_LEFT, NULL, KillBotsCmd },
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

void MenuServerHelp(edict_t *ent, pmenu_t *p)
{
        PMenu_Close(ent);
        PMenu_Open(ent, helpservermenu, -1, sizeof(helpservermenu) / sizeof(pmenu_t));
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
void AddBotsCmd3 (edict_t *ent, pmenu_t *p)
{
    stuffcmd(ent, "rcon sv addbots 1 3\n");
}

