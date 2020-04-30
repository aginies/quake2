#include "g_local.h"
#include "c_effects.h"

void misc_die (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, vec3_t point)
{
    G_FreeEdict (self);
}

/*
=================
make_debris
=================
*/
void make_debris (edict_t *ent)
{
	vec3_t	org;
	vec3_t	origin;
	float		spd;

	// calculate position for the explosion entity
	VectorMA (ent->s.origin, -0.02, ent->velocity, origin);

	// make a few big chunks
	spd = .5 * (float)ent->dmg / 200.0;
	org[0] = ent->s.origin[0] + crandom() * ent->size[0];
	org[1] = ent->s.origin[1] + crandom() * ent->size[1];
	org[2] = ent->s.origin[2] + crandom() * ent->size[2];
	ThrowShrapnel (ent, "models/objects/debris1/tris.md2", spd, org);
	spd = 1.5 * (float)ent->dmg / 200.0;
	VectorCopy (ent->absmin, org);
	ThrowShrapnel (ent, "models/objects/debris2/tris.md2", spd, org);
	spd = 1.5 * (float)ent->dmg / 200.0;
	VectorCopy (ent->absmin, org);
	ThrowShrapnel (ent, "models/objects/debris3/tris.md2", spd, org);
}

/*
============
T_ShockWave

Knocks view around a bit.  Based on T_RadiusDamage.
============
*/
void T_ShockWave (edict_t *inflictor, float damage, float radius)
{
	float		points;
	edict_t	*ent = NULL;
	vec3_t	v;
	vec3_t	dir = {0,0,0};

	float 	SHOCK_TIME = 0.1;

	while ((ent = findradius(ent, inflictor->s.origin, radius)) != NULL)
	{
		if (!ent->takedamage)
			continue;
		if (!ent->client)
			continue;
		VectorAdd (ent->mins, ent->maxs, v);
		VectorMA (ent->s.origin, 0.5, v, v);
		VectorSubtract (inflictor->s.origin, v, v);
		points = .5*(damage - 0.5 * VectorLength (v));
		if (points < .5)
			points = .5;
		if (points > 10)
			points = 10;
		if (points > 0)
		{
			VectorSubtract (ent->s.origin, inflictor->s.origin, dir);
				ent->client->v_dmg_pitch = -points;
				ent->client->v_dmg_roll = 0;
				ent->client->v_dmg_time = level.time + SHOCK_TIME;
				ent->client->kick_origin[2] = -points*4;
		}
	}
}

/*
============
T_ShockItems

Lets explosions move items.  Based on T_RadiusDamage.
TODO: Reorient items after coming to rest?
============
*/
void T_ShockItems (edict_t *inflictor)
{
	float		points;
	edict_t	*ent = NULL;
	vec3_t	v;
	vec3_t	dir;

	vec3_t	kvel;
	float		mass;

	float		radius=255;
	float		damage=100;

	while ((ent = findradius(ent, inflictor->s.origin, radius)) != NULL)
	{
		if (ent->item
		&& strcmp (ent->classname, "info_player_deathmatch") != 0)
		{
			VectorAdd (ent->mins, ent->maxs, v);
			VectorMA (ent->s.origin, 0.5, v, v);
			VectorSubtract (inflictor->s.origin, v, v);
			points = damage - 0.5 * VectorLength (v);
			if (ent->mass < 25)
				mass = 25;
			else
				mass = ent->mass;

			if (points > 0)
			{
				VectorSubtract (ent->s.origin, inflictor->s.origin, dir);
				ent->movetype = MOVETYPE_BOUNCE;

				// any problem w/leaving this changed?
				VectorScale (dir, 3.0 * (float)points / mass, kvel);
				VectorAdd (ent->velocity, kvel, ent->velocity);
				if (!(ent->svflags & SVF_MONSTER))
					VectorAdd (ent->avelocity, 1.5*kvel, ent->avelocity);

				//TODO: check groundentity & lower s.origin to keep objects from sticking to floor?
				ent->velocity[2]+=10;

				gi.linkentity (ent);
			}
		}
	}
}

/*
=================
BecomeNewExplosion
=================
*/
void BecomeNewExplosion (edict_t *ent)
{
	vec3_t	origin;

	// calculate position for the explosion entity
	VectorMA (ent->s.origin, -0.02, ent->velocity, origin);

			gi.WriteByte (svc_temp_entity);
	gi.WriteByte (TE_EXPLOSION1);
	gi.WritePosition (ent->s.origin);
	gi.multicast (ent->s.origin, MULTICAST_PVS);

	// send flash & bang
	gi.WriteByte (svc_muzzleflash);
	gi.WriteShort (ent-g_edicts);
	// any way to get a mz flash without hearing the weapon?
	gi.WriteByte (MZ_CHAINGUN2);
	gi.multicast (ent->s.origin, MULTICAST_PVS);
	// any other way to make this loud enough?
	BigBang (ent);

	// destroy object
	G_FreeEdict (ent);
}

/*
=============
isvisible

This is the ai.c visible function
=============
*/
qboolean isvisible (edict_t *self, edict_t *other)
{
	vec3_t	spot1;
	vec3_t	spot2;
	trace_t	trace;

	VectorCopy (self->s.origin, spot1);
	spot1[2] += self->viewheight;
	VectorCopy (other->s.origin, spot2);
	spot2[2] += other->viewheight;
	trace = gi.trace (spot1, vec3_origin, vec3_origin, spot2, self, MASK_OPAQUE);
	if (trace.fraction == 1.0)
		return true;
	return false;
}

/*
=================
BigBang

Loud bang.
=================
*/
void BigBang (edict_t *ent)
{
	int		i;
	edict_t	*ear;
	float		radius=1024;
	vec3_t	d;

	gi.sound (ent, CHAN_ITEM, gi.soundindex ("weapons/rocklx1a.wav"), 1, ATTN_NORM, 0);

	// Unfortunately, this sounds weak, so check each client to see if
	// it is within the blast radius or in line of sight; if so,
	// send each client a loud ATTN_STATIC bang

	ear = &g_edicts[0];
	for (i=0 ; i<globals.num_edicts ; i++, ear++)
	{
		if (!ear->inuse)
			continue;
		if (!ear->client)
			continue;
		VectorSubtract (ear->s.origin, ent->s.origin, d);
		if ((VectorLength(d) < radius) | (isvisible(ent, ear)))
			gi.sound (ear, CHAN_VOICE, gi.soundindex ("weapons/rocklx1a.wav"), 1, ATTN_STATIC, 0);
	}
}

/*
=================
ThrowShrapnel

Medium persistence with glowing trail effect
=================
*/
void ThrowShrapnel (edict_t *self, char *modelname, float speed, vec3_t origin)
{
	edict_t	*chunk;
	vec3_t	v;

	chunk = G_Spawn();
	VectorCopy (origin, chunk->s.origin);
	gi.setmodel (chunk, modelname);
	v[0] = 100 * crandom();
	v[1] = 100 * crandom();
	v[2] = 100 * crandom();
	VectorMA (self->velocity, speed, v, chunk->velocity);
	chunk->movetype = MOVETYPE_BOUNCE;
	chunk->solid = SOLID_NOT;
	chunk->avelocity[0] = random()*600;
	chunk->avelocity[1] = random()*600;
	chunk->avelocity[2] = random()*600;
	chunk->think = G_FreeEdict;
	//chunk->nextthink = level.time + 5 + random()* 5;
	chunk->nextthink = level.time + random()*2;
	chunk->s.frame = 0;
	chunk->flags = 0;
	chunk->classname = "debris";
	chunk->takedamage = DAMAGE_NO;
	chunk->die = misc_die;
	//chunk->s.effects |= EF_GRENADE;
	chunk->s.effects |= EF_GRENADE | EF_ROCKET;
	gi.linkentity (chunk);
}

/*
=================
target_explosion_explode

Added ShockWave
=================
*/
void target_explosion_explode (edict_t *self)
{
	float		save;

	gi.WriteByte (svc_temp_entity);
	gi.WriteByte (TE_EXPLOSION1);
	gi.WritePosition (self->s.origin);
	gi.multicast (self->s.origin, MULTICAST_PHS);

	T_RadiusDamage (self, self->activator, self->dmg, NULL, self->dmg+40, MOD_UNKNOWN);
	T_ShockWave(self, 255, 1024);

	save = self->delay;
	self->delay = 0;
	G_UseTargets (self, self->activator);
	self->delay = save;

}
