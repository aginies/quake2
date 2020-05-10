#include "g_local.h"
#include "stdlog.h"
#include "chaos.h"

void pvar(edict_t *ent, char *name, int *ptr, char *edit) {
    pvar_t *pvar = (pvar_t *)gi.TagMalloc(sizeof(pvar_t), TAG_LEVEL);

    pvar->name = name;
    pvar->value = ptr;
    pvar->old = *ptr;
    pvar->edit = edit;

    if(ent->pvar) {
        pvar_t *next = ent->pvar;
        while(next) {
            if(next->next) {
                next = next->next;
                continue;
            }
            next->next = pvar;
            break;
        }
    }
    else
        ent->pvar = pvar;
}


pvar_t *pvar_find(edict_t *ent, char *name) {
    pvar_t *pvar = ent->pvar;
    while(pvar) {
        if(!strcmp(name, pvar->name))
            return pvar;
        pvar = pvar->next;
    }
    return pvar;
}


void Lithium_LayoutOn(edict_t *ent, int flag) {
    ent->layout |= flag;
    ent->layout_update = true;
    ent->client->showscores = false;
    ent->client->showinventory = false;
}

void Lithium_LayoutOff(edict_t *ent, int flag) {
    if(!(ent->layout & flag)) {
//        ent->client->showscores = ent->layout ? 1 : 0;
        return;
    }
    ent->layout &= ~flag;
    ent->client->showscores = ent->layout ? 1 : 0;
    if(ent->layout)
        ent->layout_update = true;
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

void Lithium_ClientBegin(edict_t *ent) {
    char *info;
    int flags;

    ent->chaos_flags = 0;
    ent->layout = 0;
//    ent->hud = def_hud->value;
    ent->update_frame = 0;
    ent->update_size = 0;
    ent->update_other = 0;
    ent->menu = NULL;
    ent->pvar = NULL;
    ent->admin = 0;

    ent->chaos_flags |= LITHIUM_STATUSBAR;

    pvar(ent, "hud", (void *)&ent->hud, "None:Normal:Lithium:Ammo");
    pvar(ent, "admin_code", (void *)&ent->admin_code, "#####");

    info = Info_ValueForKey(ent->client->pers.userinfo, "hud");
    if(strlen(info))
        ent->hud = atoi(info);
}



void Chaos_SetStats(edict_t *self) {
    edict_t *ent = self;
    short *stats = self->client->ps.stats;

    if(!level.intermissiontime) {
    //  already used
    // 1 2 4 6 7 8 9 10 11 15 25 26 27 30
    // Don't use index > 31 this will cause error
    stats[12] = ent->client->pers.inventory[ITEM_INDEX(FindItem("shells"))];
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

char *ammobar =
"yb -156 xr -24 picn a_xgrenades xr -74 num 3 18 "
"yb -122 xr -24 picn a_lmines xr -74 num 3 17 "
"yt 84 xr -24 picn a_eshells xr -74 num 3 29 "
"yt 108 xr -24 picn a_shells xr -74 num 3 12 "
"yt 156 xr -24 picn a_slugs xr -74 num 3 12 "
"yt 180 xr -24 picn a_buzz xr -74 num 3 28 "
"yt 136 xl 0 picn a_rockets xl 24 num 3 22 "
"yt 160 xl 0 picn a_grockets xl 24 num 3 23 "
"yt 208 xl 0 picn a_cells xl 24 num 3 24 "
;

char *arraowsbar =
"yb -74 xl 0 picn a_arrows xl 24 num 3 19 "
"yb -50 xl 0 picn a_parrows xl 24 num 3 20 "
"yb -98 xl 0 picn a_earrows xl 24 num 3 21 "
;

char *grenadesbar =
"yb -98 xr -24 picn a_grenades1 xr -74 num 3 31 "
"yb -74 xr -24 picn a_fgrenades xr -74 num 3 15 "
"yb -50 xr -24 picn a_pgrenades xr -74 num 3 16 "
;

char *nukevortex =
"yt 88 xl 0 picn a_nuke xl 24 num 3 11 "
//"yt 112 xl 0 picn a_vortex xl 24 num 3 27 "
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
	    if (!ent->client->showammo)
	    {
        	strlcat(statusbar, ammobar, sizeof(statusbar));
	    }
	    if (!ent->client->showfrag)
	    {
        	strlcat(statusbar, fragsbar, sizeof(statusbar));
	    }
	    if (!ent->client->showarrow)
	    {
        	strlcat(statusbar, arraowsbar, sizeof(statusbar));
	    }
	    if (!ent->client->showgrenade)
	    {
        	strlcat(statusbar, grenadesbar, sizeof(statusbar));
	    }
	    if (ent->client->shownv)
	    {
        	strlcat(statusbar, nukevortex, sizeof(statusbar));
	    }
        strlcat(statusbar, chaos_statusbar, sizeof(statusbar));
    }
    else if(ctf->value) {
	    if (!ent->client->showammo)
	    {
        	strlcat(statusbar, ammobar, sizeof(statusbar));
	    }
	    if (!ent->client->showfrag)
	    {
        	strlcat(statusbar, fragsbar, sizeof(statusbar));
	    }
	    if (ent->client->shownv)
	    {
        	strlcat(statusbar, nukevortex, sizeof(statusbar));
	    }
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

int Layout_Updatea(edict_t *ent) {
    char statusbar[1400] = "";
    ent->chaos_flags = CHAOS_STATUSBAR;

    if (strcmp(ent->classname, "player") == 0)
    {
    if(!ctf->value) 
    {
	    if (!ent->client->showammo)
	    {
        	strlcat(statusbar, ammobar, sizeof(statusbar));
	    }
	    if (!ent->client->showfrag)
	    {
        	strlcat(statusbar, fragsbar, sizeof(statusbar));
	    }
	    if (ent->client->shownv)
	    {
        	strlcat(statusbar, nukevortex, sizeof(statusbar));
	    }
	    if (!ent->client->showarrow)
	    {
        	strlcat(statusbar, arraowsbar, sizeof(statusbar));
	    }
	    if (!ent->client->showgrenade)
	    {
        	strlcat(statusbar, grenadesbar, sizeof(statusbar));
	    }
        strlcat(statusbar, chaos_statusbar, sizeof(statusbar));
    }
    else if(ctf->value) {
	    if (!ent->client->showammo)
	    {
        	strlcat(statusbar, ammobar, sizeof(statusbar));
	    }
	    if (!ent->client->showfrag)
	    {
        	strlcat(statusbar, fragsbar, sizeof(statusbar));
	    }
	    if (ent->client->shownv)
	    {
        	strlcat(statusbar, nukevortex, sizeof(statusbar));
	    }
        strlcat(statusbar, ctf_statusbar, sizeof(statusbar));
    }

    gi.configstring (CS_STATUSBAR, statusbar);
    gi.unicast(ent, false);
    }
    return strlen(statusbar);
}

int Layout_Update(edict_t *ent) {
    char string[1024] = "";
    int size;

    ent->layout_update = false;

    if(ent->layout & LAYOUT_MENU) {
        return Menu_Update(ent);
    }
    else {
        ent->layout = LITHIUM_STATUSBAR;
        Layout_Updatea(ent);
    }

//    gi.WriteByte(svc_layout);
//    gi.WriteString(string);
//    gi.unicast(ent, true);
    return strlen(string);
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
        { "Interface",PMENU_ALIGN_CENTER, NULL, NULL },
        { NULL,PMENU_ALIGN_CENTER, NULL, NULL },
        { NULL,PMENU_ALIGN_CENTER, NULL, NULL },
        { "Status Munitions",PMENU_ALIGN_LEFT, NULL, ShowAmmoMenu },
        { "[ENTER] Pour changer",PMENU_ALIGN_RIGHT, NULL, NULL },
        { NULL,PMENU_ALIGN_CENTER, NULL, NULL },
        { "Nuke / Vortex",PMENU_ALIGN_LEFT, NULL, ShowNVMenu },
        { "[ENTER] Pour changer",PMENU_ALIGN_RIGHT, NULL, NULL },
        { NULL,PMENU_ALIGN_CENTER, NULL, NULL },
        { "Grenades",PMENU_ALIGN_LEFT, NULL, ShowGrenadeMenu },
        { "[ENTER] Pour changer",PMENU_ALIGN_RIGHT, NULL, NULL },
        { NULL,PMENU_ALIGN_CENTER, NULL, NULL },
        { "Montrer Munitions Arcs",PMENU_ALIGN_LEFT, NULL, ShowBowMenu },
        { "[ENTER] Pour changer",PMENU_ALIGN_RIGHT, NULL, NULL },
        { NULL,PMENU_ALIGN_CENTER, NULL, NULL },
        { "Montrer Nombre Frag(s)",PMENU_ALIGN_LEFT, NULL, ShowFragMenu },
        { "[ENTER] Pour changer",PMENU_ALIGN_RIGHT, NULL, NULL },
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
        { "*showammo",PMENU_ALIGN_LEFT, NULL, NULL },
        { "Status Munitions",PMENU_ALIGN_LEFT, NULL, NULL },
        { NULL,PMENU_ALIGN_CENTER, NULL, NULL },
        { "*shownv",PMENU_ALIGN_LEFT, NULL, NULL },
        { "Status Nuke et Vortex",PMENU_ALIGN_LEFT, NULL, NULL },
        { NULL,PMENU_ALIGN_CENTER, NULL, NULL },
        { "*showfrag",PMENU_ALIGN_LEFT, NULL, NULL },
        { "Nombre de Frag(s)",PMENU_ALIGN_LEFT, NULL, NULL },
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

void ShowAmmoMenu (edict_t *ent, pmenu_t *p)
{
    stuffcmd(ent, "showammo\n");
}

void ShowNVMenu (edict_t *ent, pmenu_t *p)
{
    stuffcmd(ent, "shownv\n");
}

void ShowBowMenu (edict_t *ent, pmenu_t *p)
{
    stuffcmd(ent, "showarrow\n");
}

void ShowGrenadeMenu (edict_t *ent, pmenu_t *p)
{
    stuffcmd(ent, "showgrenade\n");
}

void ShowFragMenu (edict_t *ent, pmenu_t *p)
{
    stuffcmd(ent, "showfrag\n");
}


void Lithium_Menu(edict_t *ent) {
    gi.dprintf("DEBUG HERE \n");
    Menu_Create(ent, 4, 16);
    Menu_Title(ent, "Main Menu");
    Menu_AddLine(ent, MENU_FUNC, 0, "Information", "DATA");
    Menu_AddLine(ent, MENU_TEXT, 17, "Use [ and ] keys to pick", "m");
    Menu_AddLine(ent, MENU_TEXT, 18, "Press Enter to select", "m");
    Menu_CancelFunc(ent, Lithium_Info);
}

void Lithium_Info(edict_t *ent) {
    Menu_Create(ent, 4, 15);
    Menu_Title(ent, "Information");
    Menu_AddLine(ent, MENU_PVAR, 4, "HUD layout", "hud");
    Menu_AddLine(ent, MENU_FUNC, 5, "Author Info", "DATA");
    Menu_AddLine(ent, MENU_TEXT, 6, "Use [ and ] keys to pick", "m");
    Menu_AddLine(ent, MENU_TEXT, 7, "Press Enter to select", "m");
    Menu_AddLine(ent, MENU_TEXT, 8, "Press Backspace to go back", "m");
    Menu_CancelFunc(ent, Lithium_Menu);
}

