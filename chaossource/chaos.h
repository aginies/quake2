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
void ShowAmmoMenu (edict_t *ent, pmenu_t *p);
void ShowNVMenu (edict_t *ent, pmenu_t *p);
void ShowBowMenu (edict_t *ent, pmenu_t *p);
void ShowGrenadeMenu (edict_t *ent, pmenu_t *p);
void ShowFragMenu (edict_t *ent, pmenu_t *p);


int Layout_Update(edict_t *ent);


