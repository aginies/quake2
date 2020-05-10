//
//

#define CHAOS_STATUSBAR 1
#define CHAOS_MAINMENU 2
#define CHAOS_FREE 4
#define CHAOS_OBSERVER 8

#define CS_CHAOS (CS_ITEMS - 20)
#define CS_OBSERVING (CS_CHAOS + 1)


qboolean Observer(edict_t *ent, qboolean check);
qboolean IsObserver(edict_t *ent);


int StatusBar_Update(edict_t *ent);
void Chaos_SetStats(edict_t *ent);

// MENU
void ChaosOpenMenu(edict_t *ent);
void ChaosCredits(edict_t *ent, pmenu_t *p);
void MenuInterface(edict_t *ent, pmenu_t *p);
void MenuHelp(edict_t *ent, pmenu_t *p);
void MenuBots(edict_t *ent, pmenu_t *p);
void KillBotsCmd (edict_t *ent, pmenu_t *p);
void AddBotsCmd (edict_t *ent, pmenu_t *p);
void AddBotsCmd2 (edict_t *ent, pmenu_t *p);


int Layout_Update(edict_t *ent);


//
//from import L
#define MENU_TEXT 1
#define MENU_BLANK 2
#define MENU_LVAR 3
#define MENU_FUNC 4
#define MENU_CMD 5
#define MENU_SVCMD 6
#define MENU_PVAR 7

#define HUD_NONE 0
#define HUD_NORMAL 1
#define HUD_LITHIUM 2
#define HUD_AMMO 3
#define HUD_MAX 3

#define LAYOUT_MENU 4
#define LAYOUT_SCORES 1
#define LITHIUM_OBSERVER 1
#define LITHIUM_PLAYING 2
#define LITHIUM_GOCHASECAM 4
#define LITHIUM_CHASENOTICE 8
#define LITHIUM_CHASELEADER 16
#define LITHIUM_VERIFIED 32
#define LITHIUM_ATTACKWAIT 512
#define LITHIUM_STATUSBAR 2048
#define LITHIUM_MAINMENU 4096
#define LITHIUM_HIDDEN 16384
#define VAR_DEF 12


void Lithium_ClientBegin(edict_t *ent);
void Lithium_LayoutOn(edict_t *ent, int flag);
void Lithium_LayoutOff(edict_t *ent, int flag);
void Lithium_Menu(edict_t *ent);
void Lithium_Info(edict_t *ent);

pvar_t *pvar_find(edict_t *ent, char *name);
void pvar(edict_t *ent, char *name, int *ptr, char *edit);

struct lvar_s {
    cvar_t *cvar;
    char *string;
    float value;
    int admin;
    char *edit;
    int cat;
    void (*func)(void);
    lvar_t *next;
};

struct pvar_s {
    char *name;
    int *value;
    int old;
    char *edit;
    pvar_t *next;
};

lvar_t *lvar(char *name, char *string, char *edit, int cat);
typedef struct menuline_s menuline_t;
typedef struct menu_s menu_t;
typedef struct lvar_s lvar_t;
lvar_t *def_hud;
//= lvar("def_hud", "2", "None:Normal:Lithium:Ammo", VAR_DEF);


// l_menu.c
void Menu_Create(edict_t *ent, int startline, int endline);
void Menu_Offsets(edict_t *ent, int xoff, int yoff);
void Menu_Title(edict_t *ent, char *title);
void Menu_AddLine(edict_t *ent, int type, int line, char *text, void *data);
void Menu_AddText(edict_t *ent, int line, char *format, ...);
void Menu_CancelFunc(edict_t *ent, void (*func)(edict_t *ent));
void Menu_Destroy(edict_t *ent);
int Menu_Update(edict_t *ent);
void Menu_Prev(edict_t *ent);
void Menu_Next(edict_t *ent);
void Menu_PrevPage(edict_t *ent);
void Menu_NextPage(edict_t *ent);
void Menu_Use(edict_t *ent);
void Menu_Key(edict_t *ent, int key);
void Menu_EditBegin(edict_t *ent);
void Menu_EditEnd(edict_t *ent);
void Menu_LVarChanged(lvar_t *lvar);
void Menu_ClientFrame(edict_t *ent);
void Menu_Cancel(edict_t *ent);
qboolean Menu_ClientCommand(edict_t *ent);



struct menuline_s {
    menuline_t *prev, *next;
    int type;
    int line;
    char *text;
    void *data;
    qboolean selectable;
    qboolean textp;
};

struct menu_s {
    menuline_t *firstline, *lastline;
    menuline_t *sel;
    int startline, endline;
    int page;
    qboolean changed;
    qboolean editing;
    void (*cancel_func)(edict_t *ent);
    char edit[12];
    menuline_t *lastedit;
    float lastedit_time;
    int xoff;
    int yoff;
};


