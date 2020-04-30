// laser sight patch, by Geza Beladi
#include "g_local.h"
#include "c_base.h"

void LaserSightThink (edict_t *self);
void LaserSight_On (edict_t *self);


/*----------------------------------------
SP_LaserSight

Create/remove the laser sight entity
-----------------------------------------*/

#define lss self->lasersight

void SP_LaserSight (edict_t *self)
{
	// No lasersight when you're dead.
	if (self->deadflag)
		return;

	// Toggle its status
	self->lasersightOn = !(self->lasersightOn);

	// Print its status.
	gi.cprintf (self, PRINT_HIGH, "Lasersight %s.\n",
		((self->lasersightOn) ? "on" : "off"));

	// Check to see if we should display the lasersight.
	LaserSight_Check (self);
}


void LaserSight_Check (edict_t *self)
{
	// If there's no lasersight, and any reason there should be one, make one.
	if (!self->lasersight
	&& (self->client->pers.weapon == it_rocketlauncher
		|| self->lasersightOn))
	{
		LaserSight_On (self);
	}

	// If there's a lasersight, and any reason there shouldn't be one, get rid
	// of it.
	if (self->lasersight
	&& self->client->pers.weapon != it_rocketlauncher
	&& !self->lasersightOn)
	{
		LaserSight_Off (self);
	}
}


void LaserSight_Off (edict_t *self)
{
	if ( lss )
	{
		G_FreeEdict(lss);
		lss = NULL;
	}
}


void LaserSight_On (edict_t *self)
{
	vec3_t  start,forward,right,end;

	AngleVectors (self->client->v_angle, forward, right, NULL);

	VectorSet(end,100 , 0, 0);
	G_ProjectSource (self->s.origin, end, forward, right, start);

	lss = G_Spawn ();
	lss->owner = self;
	lss->movetype = MOVETYPE_NOCLIP;
	lss->solid = SOLID_NOT;
	lss->classname = "lasersight";
    lss->s.modelindex = gi.modelindex ("sprites/lsight.sp2");
	lss->s.skinnum = 0;

	lss->s.renderfx |= RF_FULLBRIGHT;

	lss->think = LaserSightThink;
	lss->nextthink = level.time + FRAMETIME;
}


/*---------------------------------------------
LaserSightThink

Updates the sights position, angle, and shape
of the lasersight entity
---------------------------------------------*/

void LaserSightThink (edict_t *self)
{
	vec3_t start,end,endp,offset;
	vec3_t forward,right,up;
	trace_t tr;

	AngleVectors (self->owner->client->v_angle, forward, right, up);

	VectorSet(offset,24 , 6, self->owner->viewheight-7);
	G_ProjectSource (self->owner->s.origin, offset, forward, right, start);
	VectorMA(start,8192,forward,end);

	tr = gi.trace (start,NULL,NULL, end,self->owner,
		CONTENTS_SOLID|CONTENTS_MONSTER|CONTENTS_DEADMONSTER);

	if (tr.fraction != 1)
	{
		VectorMA(tr.endpos,-4,forward,endp);
		VectorCopy(endp,tr.endpos);
	}

	/* What the hell does this do?
	if ((tr.ent->svflags & SVF_MONSTER) || (tr.ent->client))
	{
		if ((tr.ent->takedamage) && (tr.ent != self->owner))
		{
			self->s.skinnum = 0;
		}
	}
	else
		self->s.skinnum = 0; */

	// If the laser sight is on someone, glow.
	if ((tr.ent->svflags & SVF_MONSTER) || (tr.ent->client))
		self->s.renderfx |= RF_SHELL_RED;
	else
		self->s.renderfx &= ~RF_SHELL_RED;

	vectoangles(tr.plane.normal,self->s.angles);
	VectorCopy(tr.endpos,self->s.origin);

	gi.linkentity (self);
	self->nextthink = level.time + FRAMETIME;

#if 0
	// Hehe, draw a line in the air too!
	gi.WriteByte (svc_temp_entity);
	gi.WriteByte (TE_BFG_LASER);
	gi.WritePosition (self->owner->s.origin);
	gi.WritePosition (tr.endpos);
	gi.unicast (self->owner, true);
#endif
}
/* eof
*/
