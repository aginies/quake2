// new effects
void T_ShockWave (edict_t *inflictor, float damage, float radius);
void T_ShockItems (edict_t *inflictor);
void ThrowShrapnel (edict_t *self, char *modelname, float speed, vec3_t origin);
void make_debris (edict_t *ent);
void BecomeNewExplosion (edict_t *ent);
void BigBang (edict_t *ent);
void target_explosion_explode (edict_t *self);
qboolean isvisible (edict_t *self, edict_t *other);

