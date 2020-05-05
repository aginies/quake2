//
//

#define CHAOS_STATUSBAR 1
#define CHAOS_MAINMENU 2
#define CHAOS_FREE 4

qboolean Observer(edict_t *ent, qboolean check);


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
void ShowFragsMenu (edict_t *ent, pmenu_t *p);

