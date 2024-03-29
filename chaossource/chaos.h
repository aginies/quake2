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
void MenuServerHelp(edict_t *ent, pmenu_t *p);
void MenuBots(edict_t *ent, pmenu_t *p);
void KillBotsCmd (edict_t *ent, pmenu_t *p);
void AddBotsCmd (edict_t *ent, pmenu_t *p);
void AddBotsCmd2 (edict_t *ent, pmenu_t *p);
void AddBotsCmd3 (edict_t *ent, pmenu_t *p);

void ShellTouch(edict_t *self, edict_t *other, cplane_t *plane, csurface_t *surf);
void ShellDie(edict_t *self);
void EjectShell (edict_t *self, vec3_t start, int toggle );



int Layout_Update(edict_t *ent);


