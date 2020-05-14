#include "g_local.h"
#include "c_base.h"
#include "c_effects.h"

/*
=================
fire_hit

Used for all impact (hit/punch/slash) attacks
=================
*/
qboolean fire_hit (edict_t *self, vec3_t aim, int damage, int kick)
{
	trace_t		tr;
	vec3_t		forward, right, up;
	vec3_t		v;
	vec3_t		point;
	float		range;
	vec3_t		dir;

	//see if enemy is in range
	VectorSubtract (self->enemy->s.origin, self->s.origin, dir);
	range = VectorLength(dir);
	if (range > aim[0])
		return false;

	if (aim[1] > self->mins[0] && aim[1] < self->maxs[0])
	{
		// the hit is straight on so back the range up to the edge of their bbox
		range -= self->enemy->maxs[0];
	}
	else
	{
		// this is a side hit so adjust the "right" value out to the edge of their bbox
		if (aim[1] < 0)
			aim[1] = self->enemy->mins[0];
		else
			aim[1] = self->enemy->maxs[0];
	}

	VectorMA (self->s.origin, range, dir, point);

	tr = gi.trace (self->s.origin, NULL, NULL, point, self, MASK_SHOT);
	if (tr.fraction < 1)
	{
		if (!tr.ent->takedamage)
			return false;
		// if it will hit any client/monster then hit the one we wanted to hit
		if ((tr.ent->svflags & SVF_MONSTER) || (tr.ent->client))
			tr.ent = self->enemy;
	}

	AngleVectors(self->s.angles, forward, right, up);
	VectorMA (self->s.origin, range, forward, point);
	VectorMA (point, aim[1], right, point);
	VectorMA (point, aim[2], up, point);
	VectorSubtract (point, self->enemy->s.origin, dir);

	// do the damage
	T_Damage (tr.ent, self, self, dir, point, vec3_origin, damage, kick/2, DAMAGE_NO_KNOCKBACK, MOD_HIT);

	if (!(tr.ent->svflags & SVF_MONSTER) && (!tr.ent->client))
		return false;

	// do our special form of knockback here
	VectorMA (self->enemy->absmin, 0.5, self->enemy->size, v);
	VectorSubtract (v, point, v);
	VectorNormalize (v);
	VectorMA (self->enemy->velocity, kick, v, self->enemy->velocity);
	if (self->enemy->velocity[2] > 0)
		self->enemy->groundentity = NULL;
	return true;
}


/*
=================
fire_lead

This is an internal support routine used for bullet/pellet based weapons.
=================
*/
void fire_lead (edict_t *self, vec3_t start, vec3_t aimdir, int damage, int kick, int te_impact, int hspread, int vspread, int mod)
{
	trace_t		tr;
	vec3_t		dir;
	vec3_t		forward, right, up;
	vec3_t		end;
	float		r;
	float		u;
	vec3_t		water_start;
	qboolean	water = false;
	int			content_mask = MASK_SHOT | MASK_WATER;

	tr = gi.trace (self->s.origin, NULL, NULL, start, self, MASK_SHOT);
	if (!(tr.fraction < 1.0))
	{
		vectoangles (aimdir, dir);
		AngleVectors (dir, forward, right, up);

		r = crandom()*hspread;
		u = crandom()*vspread;
		VectorMA (start, 8192, forward, end);
		VectorMA (end, r, right, end);
		VectorMA (end, u, up, end);

		if (gi.pointcontents (start) & MASK_WATER)
		{
			water = true;
			VectorCopy (start, water_start);
			content_mask &= ~MASK_WATER;
		}

		tr = gi.trace (start, NULL, NULL, end, self, content_mask);

		// see if we hit water
		if (tr.contents & MASK_WATER)
		{
			int		color;

			water = true;
			VectorCopy (tr.endpos, water_start);

			if (!VectorCompare (start, tr.endpos))
			{
				if (tr.contents & CONTENTS_WATER)
				{
					if (strcmp(tr.surface->name, "*brwater") == 0)
						color = SPLASH_BROWN_WATER;
					else
						color = SPLASH_BLUE_WATER;
				}
				else if (tr.contents & CONTENTS_SLIME)
					color = SPLASH_SLIME;
				else if (tr.contents & CONTENTS_LAVA)
					color = SPLASH_LAVA;
				else
					color = SPLASH_UNKNOWN;

				if (color != SPLASH_UNKNOWN)
				{
					gi.WriteByte (svc_temp_entity);
					gi.WriteByte (TE_SPLASH);
					gi.WriteByte (8);
					gi.WritePosition (tr.endpos);
					gi.WriteDir (tr.plane.normal);
					gi.WriteByte (color);
					gi.multicast (tr.endpos, MULTICAST_PVS);
				}

				// change bullet's course when it enters water
				VectorSubtract (end, start, dir);
				vectoangles (dir, dir);
				AngleVectors (dir, forward, right, up);
				r = crandom()*hspread*2;
				u = crandom()*vspread*2;
				VectorMA (water_start, 8192, forward, end);
				VectorMA (end, r, right, end);
				VectorMA (end, u, up, end);
			}

			// re-trace ignoring water this time
			tr = gi.trace (water_start, NULL, NULL, end, self, MASK_SHOT);
		}
	}

	// send gun puff / flash
	if (!((tr.surface) && (tr.surface->flags & SURF_SKY)))
	{
		if (tr.fraction < 1.0)
		{
			if (tr.ent->takedamage)
			{
				T_Damage (tr.ent, self, self, aimdir, tr.endpos, tr.plane.normal, damage, kick, DAMAGE_BULLET, mod);
			}
			else
			{
				if (strncmp (tr.surface->name, "sky", 3) != 0)
				{
					gi.WriteByte (svc_temp_entity);
					gi.WriteByte (te_impact);
					gi.WritePosition (tr.endpos);
					gi.WriteDir (tr.plane.normal);
					gi.multicast (tr.endpos, MULTICAST_PVS);

					if (self->client)
						PlayerNoise(self, tr.endpos, PNOISE_IMPACT);
				}
			}
		}
	}

	// if went through water, determine where the end and make a bubble trail
	if (water)
	{
		vec3_t	pos;

		VectorSubtract (tr.endpos, water_start, dir);
		VectorNormalize (dir);
		VectorMA (tr.endpos, -2, dir, pos);
		if (gi.pointcontents (pos) & MASK_WATER)
			VectorCopy (pos, tr.endpos);
		else
			tr = gi.trace (pos, NULL, NULL, water_start, tr.ent, MASK_WATER);

		VectorAdd (water_start, tr.endpos, pos);
		VectorScale (pos, 0.5, pos);

		gi.WriteByte (svc_temp_entity);
		gi.WriteByte (TE_BUBBLETRAIL);
		gi.WritePosition (water_start);
		gi.WritePosition (tr.endpos);
		gi.multicast (pos, MULTICAST_PVS);
	}
}


/*
=================
fire_bullet

Fires a single round.  Used for machinegun and chaingun.  Would be fine for
pistols, rifles, etc....
=================
*/
void fire_bullet (edict_t *self, vec3_t start, vec3_t aimdir, int damage, int kick, int hspread, int vspread, int mod)
{
	fire_lead (self, start, aimdir, damage, kick, TE_GUNSHOT, hspread, vspread, mod);
}


/*
=================
fire_shotgun

Shoots shotgun pellets.  Used by shotgun and super shotgun.
=================
*/
void fire_shotgun (edict_t *self, vec3_t start, vec3_t aimdir, int damage, int kick, int hspread, int vspread, int count, int mod)
{
	int		i;

	for (i = 0; i < count; i++)
		fire_lead (self, start, aimdir, damage, kick, TE_SHOTGUN, hspread, vspread, mod);
}


/*
=================
fire_blaster

Fires a single blaster bolt.  Used by the blaster and hyper blaster.
=================
*/
void blaster_touch (edict_t *self, edict_t *other, cplane_t *plane, csurface_t *surf)
{
	int		mod;

	if (other == self->owner)
		return;

	if (surf && (surf->flags & SURF_SKY))
	{
		G_FreeEdict (self);
		return;
	}

	if (self->owner->client)
		PlayerNoise(self->owner, self->s.origin, PNOISE_IMPACT);

	if (other->takedamage)
	{
		if (self->spawnflags & 1)
			mod = MOD_HYPERBLASTER;
		else
			mod = MOD_BLASTER;

		if (other->client && other->client->pers.weapon == it_sword && infront (other, self))
		{
			// 60% blocked
			if (random() < 0.6)
			{
				self->movetype = MOVETYPE_BOUNCE;
				self->velocity[0] += crandom() * 300;
				self->velocity[1] += crandom() * 300;
				self->velocity[2] += crandom() * 500;
				vectoangles (self->velocity, self->s.angles);
				gi.sound(other, CHAN_VOICE, gi.soundindex("weapons/sword/bblock.wav"), 1, ATTN_NORM, 0);
			}
			else
			{
				T_Damage (other, self, self->owner, self->velocity, self->s.origin, plane->normal, self->dmg, 1, DAMAGE_ENERGY, mod);
				G_FreeEdict (self);
			}
		}
		else
		{
			T_Damage (other, self, self->owner, self->velocity, self->s.origin, plane->normal, self->dmg, 1, DAMAGE_ENERGY, mod);
			G_FreeEdict (self);
		}
	}
	else
	{
		gi.WriteByte (svc_temp_entity);
		gi.WriteByte (TE_BLASTER);
		gi.WritePosition (self->s.origin);
		if (!plane)
			gi.WriteDir (vec3_origin);
		else
			gi.WriteDir (plane->normal);
		gi.multicast (self->s.origin, MULTICAST_PVS);

		G_FreeEdict (self);
	}
}

void fire_blaster (edict_t *self, vec3_t start, vec3_t dir, int damage, int speed, int effect, qboolean hyper)
{
	edict_t	*bolt;
	trace_t	tr;

	VectorNormalize (dir);

	bolt = G_Spawn();
	VectorCopy (start, bolt->s.origin);
	VectorCopy (start, bolt->s.old_origin);
	vectoangles (dir, bolt->s.angles);
	VectorScale (dir, speed, bolt->velocity);
	bolt->movetype = MOVETYPE_FLYMISSILE;
	bolt->clipmask = MASK_SHOT;
	bolt->solid = SOLID_BBOX;
	bolt->s.effects |= effect;
	VectorClear (bolt->mins);
	VectorClear (bolt->maxs);
	bolt->s.modelindex = gi.modelindex ("models/objects/laser/tris.md2");
	bolt->s.sound = gi.soundindex ("misc/lasfly.wav");
	bolt->owner = self;
	bolt->touch = blaster_touch;
	bolt->nextthink = level.time + 2;
	bolt->think = G_FreeEdict;
	bolt->dmg = damage;
	bolt->classname = "bolt";
	if (hyper)
		bolt->spawnflags = 1;
	gi.linkentity (bolt);

	tr = gi.trace (self->s.origin, NULL, NULL, bolt->s.origin, bolt, MASK_SHOT);
	if (tr.fraction < 1.0)
	{
		VectorMA (bolt->s.origin, -10, dir, bolt->s.origin);
		bolt->touch (bolt, tr.ent, NULL, NULL);
	}
}	


/*
=================
fire_grenade
=================
*/
void Grenade_Explode (edict_t *ent)
{
	vec3_t		origin;
	int			mod;

	if (ent->owner && ent->owner->client)
		PlayerNoise(ent->owner, ent->s.origin, PNOISE_IMPACT);

	//FIXME: if we are onground then raise our Z just a bit since we are a point?
	if (ent->enemy)
	{
		float	points;
		vec3_t	v;
		vec3_t	dir;

		VectorAdd (ent->enemy->mins, ent->enemy->maxs, v);
		VectorMA (ent->enemy->s.origin, 0.5, v, v);
		VectorSubtract (ent->s.origin, v, v);
		points = ent->dmg - 0.5 * VectorLength (v);
		VectorSubtract (ent->enemy->s.origin, ent->s.origin, dir);
		if (ent->spawnflags & 1)
			mod = MOD_HANDGRENADE;
		else
			mod = MOD_GRENADE;
		T_Damage (ent->enemy, ent, ent->owner, dir, ent->s.origin, vec3_origin, (int)points, (int)points, DAMAGE_RADIUS, mod);
	}

	if (ent->spawnflags & 2)
		mod = MOD_HELD_GRENADE;
	else if (ent->spawnflags & 1)
		mod = MOD_HG_SPLASH;
	else
		mod = MOD_G_SPLASH;
	T_RadiusDamage(ent, ent->owner, ent->dmg, ent->enemy, ent->dmg_radius, mod);

    // shake view
    T_ShockWave(ent, 255, 1024);
    // let blast move items
    T_ShockItems(ent);
    // do some debris
    make_debris (ent);

	VectorMA (ent->s.origin, -0.02, ent->velocity, origin);
	gi.WriteByte (svc_temp_entity);
	if (ent->waterlevel)
	{
		if (ent->groundentity)
			gi.WriteByte (TE_GRENADE_EXPLOSION_WATER);
		else
			gi.WriteByte (TE_ROCKET_EXPLOSION_WATER);
	}
	else
	{
		if (ent->groundentity)
			gi.WriteByte (TE_GRENADE_EXPLOSION);
		else
			gi.WriteByte (TE_ROCKET_EXPLOSION);
	}
	gi.WritePosition (origin);
	gi.multicast (ent->s.origin, MULTICAST_PHS);

	G_FreeEdict (ent);
}

void Grenade_Touch (edict_t *ent, edict_t *other, cplane_t *plane, csurface_t *surf)
{
	if (other == ent->owner)
		return;

	if (surf && (surf->flags & SURF_SKY))
	{
		G_FreeEdict (ent);
		return;
	}

	if (!other->takedamage)
	{
		if (ent->spawnflags & 1)
		{
			if (random() > 0.5)
				gi.sound (ent, CHAN_VOICE, gi.soundindex ("weapons/hgrenb1a.wav"), 1, ATTN_NORM, 0);
			else
				gi.sound (ent, CHAN_VOICE, gi.soundindex ("weapons/hgrenb2a.wav"), 1, ATTN_NORM, 0);
		}
		else
		{
			gi.sound (ent, CHAN_VOICE, gi.soundindex ("weapons/grenlb1b.wav"), 1, ATTN_NORM, 0);
		}
		return;
	} 

	ent->enemy = other;
	Grenade_Explode (ent);
}

void fire_grenade (edict_t *self, vec3_t start, vec3_t aimdir, int damage, int speed, float timer, float damage_radius)
{
	edict_t	*grenade;
	vec3_t	dir;
	vec3_t	forward, right, up;

	vectoangles (aimdir, dir);
	AngleVectors (dir, forward, right, up);

	grenade = G_Spawn();
	VectorCopy (start, grenade->s.origin);
	VectorScale (aimdir, speed, grenade->velocity);
	VectorMA (grenade->velocity, 200 + crandom() * 10.0, up, grenade->velocity);
	VectorMA (grenade->velocity, crandom() * 10.0, right, grenade->velocity);
	VectorSet (grenade->avelocity, 300, 300, 300);
	grenade->movetype = MOVETYPE_BOUNCE;
	grenade->clipmask = MASK_SHOT;
	grenade->solid = SOLID_BBOX;
	grenade->s.effects = 0;
	VectorClear (grenade->mins);
	VectorClear (grenade->maxs);
	grenade->s.modelindex = gi.modelindex ("models/objects/grenade/tris.md2");
	grenade->owner = self;
	grenade->touch = Grenade_Touch;
	grenade->nextthink = level.time + timer;
	grenade->think = Grenade_Explode;
	grenade->dmg = damage;
	grenade->dmg_radius = damage_radius;
	grenade->classname = "grenade";

	gi.linkentity (grenade);
}

void fire_grenade2 (edict_t *self, vec3_t start, vec3_t aimdir, int damage, int speed, float timer, float damage_radius, qboolean held)
{
	edict_t	*grenade;
	vec3_t	dir;
	vec3_t	forward, right, up;

	vectoangles (aimdir, dir);
	AngleVectors (dir, forward, right, up);

	grenade = G_Spawn();
	VectorCopy (start, grenade->s.origin);
	VectorScale (aimdir, speed, grenade->velocity);
	VectorMA (grenade->velocity, 200 + crandom() * 10.0, up, grenade->velocity);
	VectorMA (grenade->velocity, crandom() * 10.0, right, grenade->velocity);
	VectorSet (grenade->avelocity, 300, 300, 300);
	grenade->movetype = MOVETYPE_BOUNCE;
	grenade->clipmask = MASK_SHOT;
	grenade->solid = SOLID_BBOX;
	grenade->s.effects = 0;
	VectorClear (grenade->mins);
	VectorClear (grenade->maxs);
	grenade->s.modelindex = gi.modelindex ("models/objects/grenade2/tris.md2");
	grenade->owner = self;
	grenade->touch = Grenade_Touch;
	grenade->nextthink = level.time + timer;
	grenade->think = Grenade_Explode;
	grenade->dmg = damage;
	grenade->dmg_radius = damage_radius;
	grenade->classname = "hgrenade";
	if (held)
		grenade->spawnflags = 3;
	else
		grenade->spawnflags = 1;
	grenade->s.sound = gi.soundindex("weapons/hgrenc1b.wav");

	if (timer <= 0.0)
		Grenade_Explode (grenade);
	else
	{
		gi.sound (self, CHAN_WEAPON, gi.soundindex ("weapons/hgrent1a.wav"), 1, ATTN_NORM, 0);
		gi.linkentity (grenade);
	}
}


/*
=================
rocket_touch
Grenade explosion + a few glowing trails.  Creates ~15 new entities.
=================
*/
void rocket_touch (edict_t *ent, edict_t *other, cplane_t *plane, csurface_t *surf)
{
    int     i;
    vec3_t  org;
    float       spd;
    vec3_t  origin;
    int mod;
    // can't be hit by own rocket
    if (other == ent->owner)
        return;

    // can't hit sky
    if (surf && (surf->flags & SURF_SKY))
    {
        G_FreeEdict (ent);
        return;
    }
    // noise if hits other player
    if (ent->owner->client)
        PlayerNoise(ent->owner, ent->s.origin, PNOISE_IMPACT);

    // calculate position for the explosion entity
    VectorMA (ent->s.origin, -0.02, ent->velocity, origin);
    // impact damage
    if (other->takedamage)
    {
        // Set up the means of death.
        mod = MOD_ROCKET;
        T_Damage (other, ent, ent->owner, ent->velocity, ent->s.origin,
            plane->normal, ent->dmg, 0, 0, mod);
    }

    if ((surf) && !(surf->flags & (SURF_WARP|SURF_TRANS33|SURF_TRANS66|SURF_FLOWING)))
    {
        int         n;
        n = rand() % 5;
        while(n--)
        ThrowDebris (ent, "models/objects/debris2/tris.md2", 2, ent->s.origin);
   }

   // make some glowing shrapnel
    spd = 15.0 * ent->dmg / 200;
    for (i = 0; i < 3; i++)
    {
        org[0] = ent->s.origin[0] + crandom() * ent->size[0];
        org[1] = ent->s.origin[1] + crandom() * ent->size[1];
        org[2] = ent->s.origin[2] + crandom() * ent->size[2];
        ThrowShrapnel(ent, "models/objects/debris2/tris.md2", spd, org);
    }

    make_debris (ent);
    mod = MOD_R_SPLASH;
    T_RadiusDamage(ent, ent->owner, ent->radius_dmg, other, ent->dmg_radius,
        mod);
    T_ShockItems(ent);
    T_ShockWave(ent, 255, 1024);
    BecomeNewExplosion (ent);
}

void fire_rocket (edict_t *self, vec3_t start, vec3_t dir, int damage, int speed, float damage_radius, int radius_damage)
{
	edict_t	*rocket;

	rocket = G_Spawn();
	VectorCopy (start, rocket->s.origin);
	VectorCopy (dir, rocket->movedir);
	vectoangles (dir, rocket->s.angles);
	VectorScale (dir, speed, rocket->velocity);
	rocket->movetype = MOVETYPE_FLYMISSILE;
	rocket->clipmask = MASK_SHOT;
	rocket->solid = SOLID_BBOX;
	rocket->s.effects |= EF_ROCKET;
	VectorClear (rocket->mins);
	VectorClear (rocket->maxs);
	rocket->s.modelindex = gi.modelindex ("models/objects/rocket/tris.md2");
	rocket->owner = self;
	rocket->touch = rocket_touch;
	rocket->nextthink = level.time + 8000/speed;
	rocket->think = G_FreeEdict;
	rocket->dmg = damage;
	rocket->radius_dmg = radius_damage;
	rocket->dmg_radius = damage_radius;
	rocket->s.sound = gi.soundindex ("weapons/rockfly.wav");
	rocket->classname = "rocket";

	gi.linkentity (rocket);
}

void guidedrocket_think (edict_t *ent)
{
    vec_t speed;
    vec3_t targetdir;

    // If our owner is dead, stop tracking & just fly straight.
    if (ent->owner->deadflag)
        return;

    // Nudge our direction toward our laser-sight.
    if (ent->owner->lasersight)
    {
        VectorSubtract (ent->owner->lasersight->s.origin, ent->s.origin,
            targetdir);
        targetdir[2] += 16;
        VectorNormalize (targetdir);
        VectorScale (targetdir, 0.35, targetdir);
        VectorAdd (targetdir, ent->movedir, targetdir);
        VectorNormalize (targetdir);
        VectorCopy (targetdir, ent->movedir);
        vectoangles (targetdir, ent->s.angles);
        speed = VectorLength(ent->velocity);
        VectorScale (targetdir, speed, ent->velocity);
    }

    ent->nextthink = level.time + FRAMETIME;

    gi.linkentity (ent);
}

void guidedrocket_explode (edict_t *ent)
{
   vec3_t      origin;

    if (ent->owner->client)
        PlayerNoise (ent->owner, ent->s.origin, PNOISE_IMPACT);

    // calculate position for the explosion entity
    VectorMA (ent->s.origin, -0.02, ent->velocity, origin);

    ent->takedamage = DAMAGE_NO;
    T_RadiusDamage (ent, ent->owner, ent->radius_dmg, ent->enemy,
        ent->dmg_radius, MOD_ROCKET);

    gi.WriteByte (svc_temp_entity);
    if (ent->waterlevel)
        gi.WriteByte (TE_ROCKET_EXPLOSION_WATER);
    else
        gi.WriteByte (TE_ROCKET_EXPLOSION);
    gi.WritePosition (origin);
    gi.multicast (ent->s.origin, MULTICAST_PHS);

    G_FreeEdict (ent);
}


void guidedrocket_die (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
    // Don't take any more damage.
    self->takedamage = DAMAGE_NO;

    // Give them credit for shooting us out of the sky, by not hurting them.
    self->enemy = attacker;

    // Blow up.
    self->think = guidedrocket_explode;
    self->nextthink = level.time + FRAMETIME;
}

void guidedrocket_touch (edict_t *ent, edict_t *other, cplane_t *plane, csurface_t *surf)
{
    if (other == ent->owner)
        return;

    if (surf && (surf->flags & SURF_SKY))
    {
        G_FreeEdict (ent);
        return;
    }

    if (other->takedamage)
    {
     T_Damage (other, ent, ent->owner, ent->velocity, ent->s.origin,
            plane->normal, ent->dmg, 120, 0, MOD_ROCKET);
    }
    else
    {
        // don't throw any debris in net games
//        if (!deathmatch->value && !coop->value)
//        {
            if ((surf) && !(surf->flags & (SURF_WARP|SURF_TRANS33|SURF_TRANS66|SURF_FLOWING)))
            {
                int         n;

                n = rand() % 5;
                while(n--)
                    ThrowDebris (ent, "models/objects/debris2/tris.md3", 2, ent->s.origin);
            }
//        }
    }

    // Now make the rocket explode.
    ent->enemy = other;
    guidedrocket_explode (ent);
}

void fire_guidedrocket (edict_t *self, vec3_t start, vec3_t dir, int damage, int speed, float damage_radius, int radius_damage)
{
    edict_t *rocket;

    rocket = G_Spawn();
    VectorCopy (start, rocket->s.origin);
    VectorCopy (dir, rocket->movedir);
    vectoangles (dir, rocket->s.angles);
    VectorScale (dir, speed, rocket->velocity);
    rocket->movetype = MOVETYPE_FLYMISSILE;
    rocket->clipmask = MASK_SHOT;
    rocket->solid = SOLID_BBOX;
    rocket->s.effects |= EF_ROCKET;
    VectorClear (rocket->mins);
    VectorClear (rocket->maxs);
    rocket->model = "models/objects/rocket/tris.md2";
    rocket->s.modelindex = gi.modelindex (rocket->model);
    rocket->owner = self;
    rocket->touch = guidedrocket_touch;
    rocket->enemy = NULL;

    /* Make guided rocket rocket shootable.
    rocket->movetype = MOVETYPE_FLYMISSILE;
    rocket->takedamage = DAMAGE_YES;
    //rocket->svflags |= SVF_MONSTER;
    //rocket->clipmask |= CONTENTS_MONSTERCLIP;
    VectorSet (rocket->mins, -8, -8, -8);
    VectorSet (rocket->maxs, 8, 8, 8);
    rocket->mass = 10;
    rocket->health = 20;
    rocket->max_health = 20;
    rocket->die = guidedrocket_die; */

    rocket->dmg = damage;
    rocket->dmg = damage;
    rocket->radius_dmg = 120;
    rocket->dmg_radius = 120;
    rocket->s.sound = gi.soundindex ("weapons/rockfly.wav");
    rocket->classname = "guided rocket";
    rocket->nextthink = level.time + FRAMETIME;
    rocket->think = guidedrocket_think;

    gi.linkentity (rocket);
}

/*
=================
fire_rail
=================
*/
void fire_rail (edict_t *self, vec3_t start, vec3_t aimdir, int damage, int kick)
{
	vec3_t		from;
	vec3_t		end;
	trace_t		tr;
	edict_t		*ignore;
	int			mask;
	qboolean	water;

	VectorMA (start, 8192, aimdir, end);
	VectorCopy (start, from);
	ignore = self;
	water = false;
	mask = MASK_SHOT|CONTENTS_SLIME|CONTENTS_LAVA;
	while (ignore)
	{
		tr = gi.trace (from, NULL, NULL, end, ignore, mask);

		if (tr.contents & (CONTENTS_SLIME|CONTENTS_LAVA))
		{
			mask &= ~(CONTENTS_SLIME|CONTENTS_LAVA);
			water = true;
		}
		else
		{
			if ((tr.ent->svflags & SVF_MONSTER) || (tr.ent->client))
				ignore = tr.ent;
			else
				ignore = NULL;

			if ((tr.ent != self) && (tr.ent->takedamage))
				T_Damage (tr.ent, self, self, aimdir, tr.endpos, tr.plane.normal, damage, kick, 0, MOD_RAILGUN);
		}

		VectorCopy (tr.endpos, from);
	}

	// send gun puff / flash
	gi.WriteByte (svc_temp_entity);
	gi.WriteByte (TE_RAILTRAIL);
	gi.WritePosition (start);
	gi.WritePosition (tr.endpos);
	gi.multicast (self->s.origin, MULTICAST_PHS);
//	gi.multicast (start, MULTICAST_PHS);
	if (water)
	{
		gi.WriteByte (svc_temp_entity);
		gi.WriteByte (TE_RAILTRAIL);
		gi.WritePosition (start);
		gi.WritePosition (tr.endpos);
		gi.multicast (tr.endpos, MULTICAST_PHS);
	}

	if (self->client)
		PlayerNoise(self, tr.endpos, PNOISE_IMPACT);
}


/*
=================
fire_bfg
=================
*/
void bfg_explode (edict_t *self)
{
	edict_t	*ent;
	float	points;
	vec3_t	v;
	float	dist;

	if (self->s.frame == 0)
	{
		// the BFG effect
		ent = NULL;
		while ((ent = findradius(ent, self->s.origin, self->dmg_radius)) != NULL)
		{
			if (!ent->takedamage)
				continue;
			if (ent == self->owner)
				continue;
			if (!CanDamage (ent, self))
				continue;
			if (!CanDamage (ent, self->owner))
				continue;

			VectorAdd (ent->mins, ent->maxs, v);
			VectorMA (ent->s.origin, 0.5, v, v);
			VectorSubtract (self->s.origin, v, v);
			dist = VectorLength(v);
			points = self->radius_dmg * (1.0 - sqrt(dist/self->dmg_radius));
			if (ent == self->owner)
				points = points * 0.5;

			gi.WriteByte (svc_temp_entity);
			gi.WriteByte (TE_BFG_EXPLOSION);
			gi.WritePosition (ent->s.origin);
			gi.multicast (ent->s.origin, MULTICAST_PHS);
			T_Damage (ent, self, self->owner, self->velocity, ent->s.origin, vec3_origin, (int)points, 0, DAMAGE_ENERGY, MOD_BFG_EFFECT);
		}
	}

	self->nextthink = level.time + FRAMETIME;
	self->s.frame++;
	if (self->s.frame == 5)
		self->think = G_FreeEdict;
}

void bfg_touch (edict_t *self, edict_t *other, cplane_t *plane, csurface_t *surf)
{
	if (other == self->owner)
		return;

	if (surf && (surf->flags & SURF_SKY))
	{
		G_FreeEdict (self);
		return;
	}

	if (self->owner->client)
		PlayerNoise(self->owner, self->s.origin, PNOISE_IMPACT);

	// core explosion - prevents firing it into the wall/floor
	if (other->takedamage)
		T_Damage (other, self, self->owner, self->velocity, self->s.origin, plane->normal, 200, 0, 0, MOD_BFG_BLAST);
	T_RadiusDamage(self, self->owner, 200, other, 100, MOD_BFG_BLAST);

	gi.sound (self, CHAN_VOICE, gi.soundindex ("weapons/bfg__x1b.wav"), 1, ATTN_NORM, 0);
	self->solid = SOLID_NOT;
	self->touch = NULL;
	VectorMA (self->s.origin, -1 * FRAMETIME, self->velocity, self->s.origin);
	VectorClear (self->velocity);
	self->s.modelindex = gi.modelindex ("sprites/s_bfg3.sp2");
	self->s.frame = 0;
	self->s.sound = 0;
	self->s.effects &= ~EF_ANIM_ALLFAST;
	self->think = bfg_explode;
	self->nextthink = level.time + FRAMETIME;
	self->enemy = other;

	gi.WriteByte (svc_temp_entity);
	gi.WriteByte (TE_BFG_BIGEXPLOSION);
	gi.WritePosition (self->s.origin);
	gi.multicast (self->s.origin, MULTICAST_PVS);
}

void bfgball_touch (edict_t *self, edict_t *other, cplane_t *plane, csurface_t *surf)
{
	if (other == self->owner)
		return;

	G_FreeEdict (self);
}

void bfgball_think (edict_t *self)
{
	if (self->s.frame < 9)
		self->s.frame += 1;
	else
		self->s.frame = 0;

	self->nextthink = level.time + FRAMETIME;
}


void bfg_think (edict_t *self)
{
	edict_t	*ent;
	edict_t	*ignore;
	vec3_t	point;
	vec3_t	dir;
	vec3_t	start;
	vec3_t	end;
	int		dmg;
	trace_t	tr;

	dmg = 5;

	ent = NULL;
	while ((ent = findradius(ent, self->s.origin, 256)) != NULL)
	{
		if (ent == self)
			continue;

		if (ent == self->owner)
			continue;

		if (!ent->takedamage)
			continue;

		if (!(ent->svflags & SVF_MONSTER) && (!ent->client) && (strcmp(ent->classname, "misc_explobox") != 0))
			continue;

		VectorMA (ent->absmin, 0.5, ent->size, point);

		VectorSubtract (point, self->s.origin, dir);
		VectorNormalize (dir);

		ignore = self;
		VectorCopy (self->s.origin, start);
		VectorMA (start, 2048, dir, end);
		while(1)
		{
			tr = gi.trace (start, NULL, NULL, end, ignore, CONTENTS_SOLID|CONTENTS_MONSTER|CONTENTS_DEADMONSTER);

			if (!tr.ent)
				break;

			// hurt it if we can
			if ((tr.ent->takedamage) && !(tr.ent->flags & FL_IMMUNE_LASER) && (tr.ent != self->owner))
				T_Damage (tr.ent, self, self->owner, dir, tr.endpos, vec3_origin, dmg, 1, DAMAGE_ENERGY, MOD_BFG_LASER);

			// if we hit something that's not a monster or player we're done
			if (!(tr.ent->svflags & SVF_MONSTER) && (!tr.ent->client))
			{
				gi.WriteByte (svc_temp_entity);
				gi.WriteByte (TE_LASER_SPARKS);
				gi.WriteByte (4);
				gi.WritePosition (tr.endpos);
				gi.WriteDir (tr.plane.normal);
				gi.WriteByte (self->s.skinnum);
				gi.multicast (tr.endpos, MULTICAST_PVS);
				break;
			}

			ignore = tr.ent;
			VectorCopy (tr.endpos, start);
		}

		gi.WriteByte (svc_temp_entity);
		gi.WriteByte (TE_BFG_LASER);
		gi.WritePosition (self->s.origin);
		gi.WritePosition (tr.endpos);
		gi.multicast (self->s.origin, MULTICAST_PHS);
	}
	self->nextthink = level.time + FRAMETIME;
}


void fire_bfg (edict_t *self, vec3_t start, vec3_t dir, int damage, int speed, float damage_radius)
{
	edict_t	*bfg;
	edict_t	*ball;

	bfg = G_Spawn();
	VectorCopy (start, bfg->s.origin);
	VectorCopy (dir, bfg->movedir);
	vectoangles (dir, bfg->s.angles);
	VectorScale (dir, speed, bfg->velocity);
	bfg->movetype = MOVETYPE_FLYMISSILE;
	bfg->clipmask = MASK_SHOT;
	bfg->solid = SOLID_BBOX;
	bfg->s.effects |= EF_BFG | EF_ANIM_ALLFAST;
	VectorClear (bfg->mins);
	VectorClear (bfg->maxs);
	bfg->s.modelindex = gi.modelindex ("sprites/s_bfg1.sp2");
	bfg->owner = self;
	bfg->touch = bfg_touch;
	bfg->nextthink = level.time + 8000/speed;
	bfg->think = G_FreeEdict;
	bfg->radius_dmg = damage;
	bfg->dmg_radius = damage_radius;
	bfg->classname = "bfg blast";
	bfg->s.sound = gi.soundindex ("weapons/bfg__l1a.wav");

	bfg->think = bfg_think;
	bfg->nextthink = level.time + FRAMETIME;
	bfg->teammaster = bfg;
	bfg->teamchain = NULL;

	gi.linkentity (bfg);


	ball = G_Spawn();
	VectorCopy (start, ball->s.origin);
	VectorCopy (dir, ball->movedir);
	vectoangles (dir, ball->s.angles);
	VectorScale (dir, speed, ball->velocity);
	ball->movetype = MOVETYPE_FLYMISSILE;
	ball->clipmask = MASK_SHOT;
	ball->solid = SOLID_BBOX;
	ball->s.effects = 0;
	VectorClear (ball->mins);
	VectorClear (ball->maxs);
	ball->s.modelindex = gi.modelindex ("models/objects/bfgball/tris.md2");
	ball->owner = self;
	ball->touch = bfgball_touch;
	ball->nextthink = level.time + 8000/speed;
	ball->think = G_FreeEdict;
	ball->radius_dmg = damage;
	ball->dmg_radius = damage_radius;
	ball->classname = "bfg ball";

	ball->s.frame = 0;
	ball->s.renderfx = RF_TRANSLUCENT;
	ball->think = bfgball_think;
	ball->nextthink = level.time + FRAMETIME;
	ball->teammaster = ball;
	ball->teamchain = NULL;

	gi.linkentity (ball);
}

//======================================================================
//======================================================================
// Marsilainen's Plasma Rifle mod


void plasma_explode(edict_t *self)
{
   //just the exposion animation sprite. damage is made in hit

   self->nextthink = level.time + FRAMETIME;
   self->s.frame++;
   //free the entity after last frame
   if (self->s.frame == 4) {
       self->think = G_FreeEdict;
   }
}

void plasma_touch(edict_t *self, edict_t *other, cplane_t *plane, csurface_t *surf)
{
   if (other == self->owner)
       return;

   if (surf && (surf->flags & SURF_SKY))
   {
       G_FreeEdict(self);
       return;
   }

   if (self->owner->client)
       PlayerNoise(self->owner, self->s.origin, PNOISE_IMPACT);


   if (other->takedamage)
   {
       T_Damage(other, self, self->owner, self->velocity, self->s.origin, plane->normal, self->dmg, 1, DAMAGE_ENERGY+5, MOD_PLASMA_RIFLE);
   }
 
   //explosion sound
   gi.sound(self, CHAN_VOICE, gi.soundindex("weapons/plsmexpl.wav"), 1, ATTN_STATIC, 0);
   self->solid = SOLID_NOT;
   self->touch = NULL;
   // calculate position for the explosion entity
   VectorMA(self->s.origin, -0.02, self->velocity, self->s.origin);
   VectorClear(self->velocity);
   self->s.modelindex = 0;
   //change the sprite to explosion
   self->s.modelindex = gi.modelindex("sprites/s_pls2.sp2");
   self->s.frame = 0;
   self->s.sound = 0;
   //disable blue light effect for the explosion sprite
//   self->s.effects &= ~EF_BLUEHYPERBLASTER;
   self->s.effects |= EF_BFG;
   //disable translucency for the explosion
   if (plasma_alpha->value == 2) {
       self->s.renderfx &= ~RF_TRANSLUCENT;
   }
   self->think = plasma_explode;
   self->nextthink = level.time + FRAMETIME;
   self->enemy = other;

   gi.WriteByte(svc_temp_entity);
   gi.WriteByte(TE_BFG_EXPLOSION);
   gi.WritePosition(self->s.origin);
   gi.multicast(self->s.origin, MULTICAST_PVS);

   /*
   //effect direction
   if (!plane)
   {
       gi.WriteDir(vec3_origin);
   }
   else
   {
       gi.WriteDir(plane->normal);
       gi.multicast(self->s.origin, MULTICAST_PVS);
   }
   */
}


void plasma_think(edict_t *self)
{
   self->nextthink = level.time + FRAMETIME;
}


void fire_plasma(edict_t *self, vec3_t start, vec3_t dir, int damage, int speed)
{

   edict_t *plasma;
   trace_t tr;
   vec3_t  end;

   vec3_t scaledv;
   vec3_t spawnpos = {0,0,0};

   plasma = G_Spawn();

   //check if wall too close
   VectorMA(start, 8, dir, end);
   tr = gi.trace(self->s.origin, NULL, NULL, end, self, MASK_SOLID);

   //move spawn position back a little
   if (tr.fraction < 1) {
       if (tr.surface) {
           VectorScale(dir, 42, scaledv);
           VectorInverse(scaledv);
           VectorAdd(scaledv, start, spawnpos);
       }
   }
   else {
       VectorCopy(start, spawnpos);
   }

   VectorCopy(spawnpos, plasma->s.origin);
   VectorCopy(dir, plasma->movedir);

   vectoangles(dir, plasma->s.angles);
   VectorScale(dir, speed, plasma->velocity);

   plasma->movetype = MOVETYPE_FLYMISSILE;
   plasma->clipmask = MASK_SHOT | MASK_WATER;
   plasma->solid = SOLID_BBOX;

   // blue light, 10hz sprite animation loop
   plasma->s.effects |= EF_ANIM_ALLFAST | EF_BLUEHYPERBLASTER;

   //set alpha effect
   if (plasma_alpha->value == 1 || plasma_alpha->value == 2) {
       plasma->s.renderfx = RF_TRANSLUCENT;
   }

   VectorClear(plasma->mins);
   VectorClear(plasma->maxs);

   plasma->s.modelindex = gi.modelindex("sprites/s_pls1.sp2");

   plasma->owner = self;
   plasma->touch = plasma_touch;

   plasma->nextthink = level.time + 2;
   plasma->think = G_FreeEdict;
   plasma->dmg = damage;
   plasma->classname = "plasma";

   plasma->think = plasma_think;
   plasma->nextthink = level.time + FRAMETIME;

   gi.linkentity(plasma);
}

