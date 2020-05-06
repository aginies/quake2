// g_weapon.c

#include "g_local.h"
#include "m_player.h"
#include "c_base.h"
#include "c_botai.h"

void weapon_grenade_fire (edict_t *ent, qboolean held);


void P_ProjectSource (gclient_t *client, vec3_t point, vec3_t distance, vec3_t forward, vec3_t right, vec3_t result)
{
	vec3_t	_distance;

	VectorCopy (distance, _distance);
	if (client->pers.hand == LEFT_HANDED)
		_distance[1] *= -1;
	else if (client->pers.hand == CENTER_HANDED)
		_distance[1] = 0;
	G_ProjectSource (point, _distance, forward, right, result);
}

void FreeNoise1(edict_t *ent)
{
	if (ent->owner)
		ent->owner->mynoise = NULL;

	G_FreeEdict(ent);
}

void FreeNoise2(edict_t *ent)
{
	if (ent->owner)
		ent->owner->mynoise2 = NULL;

	G_FreeEdict(ent);
}

void PlayerNoise(edict_t *who, vec3_t where, int type)
{
	edict_t		*noise;

	if (type == PNOISE_WEAPON)
	{
		if (who->client->silencer_shots)
		{
			who->client->silencer_shots--;
			return;
		}
	}

	if ((type == PNOISE_SELF || type == PNOISE_WEAPON) && !who->mynoise)
	{
		noise = G_Spawn();
		noise->classname = "player_noise";
		VectorSet (noise->mins, -8, -8, -8);
		VectorSet (noise->maxs, 8, 8, 8);
		noise->owner = who;
		noise->svflags = SVF_NOCLIENT;
		noise->think = FreeNoise1;
		noise->nextthink = level.time + 0.1;
		who->mynoise = noise;

		VectorCopy (where, noise->s.origin);
		VectorSubtract (where, noise->maxs, noise->absmin);
		VectorAdd (where, noise->maxs, noise->absmax);
		gi.linkentity (noise);
	}
	else if (!who->mynoise2) // type == PNOISE_IMPACT
	{
		noise = G_Spawn();
		noise->classname = "player_noise";
		VectorSet (noise->mins, -8, -8, -8);
		VectorSet (noise->maxs, 8, 8, 8);
		noise->owner = who;
		noise->svflags = SVF_NOCLIENT;
		noise->think =FreeNoise2;
		noise->nextthink = level.time + 0.1;
		who->mynoise2 = noise;

		VectorCopy (where, noise->s.origin);
		VectorSubtract (where, noise->maxs, noise->absmin);
		VectorAdd (where, noise->maxs, noise->absmax);
		gi.linkentity (noise);
	}
}


qboolean Pickup_Weapon (edict_t *ent, edict_t *other)
{
	int			index;
	gitem_t		*ammo;
	vec3_t		dir;
	vec_t		dist;

	index = ITEM_INDEX(ent->item);

	if ( ( ((int)(dmflags->value) & DF_WEAPONS_STAY) || coop->value) 
		&& other->client->pers.inventory[index])
	{
		if (!(ent->spawnflags & (DROPPED_ITEM | DROPPED_PLAYER_ITEM) ) )
			return false;	// leave the weapon for others to pickup
	}

	other->client->pers.inventory[index]++;

	//MATTHIAS
	if (ent->item == it_supershotgun)
		other->client->pers.inventory[ITEM_INDEX(it_esupershotgun)]++;
	if (ent->item == it_rocketlauncher)
		other->client->pers.inventory[ITEM_INDEX(it_hominglauncher)]++;
	if (ent->item == it_hominglauncher)
		other->client->pers.inventory[ITEM_INDEX(it_rocketlauncher)]++;
	if (ent->item == it_grenadelauncher)
	{
		other->client->pers.inventory[ITEM_INDEX(it_flashgrenadelauncher)]++;
		other->client->pers.inventory[ITEM_INDEX(it_poisongrenadelauncher)]++;
	}
	if (ent->item == it_crossbow)
	{
		other->client->pers.inventory[ITEM_INDEX(it_poisoncrossbow)]++;
		other->client->pers.inventory[ITEM_INDEX(it_explosivecrossbow)]++;
	}

	if (!(ent->spawnflags & DROPPED_ITEM) )
	{
		// give them some ammo with it
		ammo = FindItem (ent->item->ammo);
		if ( (int)dmflags->value & DF_INFINITE_AMMO )
			Add_Ammo (other, ammo, 1000);
		else
			Add_Ammo (other, ammo, ammo->quantity);

		if (! (ent->spawnflags & DROPPED_PLAYER_ITEM) )
		{
			if (deathmatch->value)
			{
				if ((int)(dmflags->value) & DF_WEAPONS_STAY)
					ent->flags |= FL_RESPAWN;
				else
					SetRespawn (ent, 30);
			}
			if (coop->value)
				ent->flags |= FL_RESPAWN;
		}
	}

	if (other->client->pers.weapon != ent->item && 
		(other->client->pers.inventory[index] == 1) &&
		( !deathmatch->value || other->client->pers.weapon == FindItem("AK42 Assault Pistol") ) )
		other->client->newweapon = ent->item;

	if (strcmp(other->classname,"bot") == 0)
	{
		if (other->client
			&& other->client->pers.weapon != it_rturret
			&& other->client->pers.weapon != it_lturret
			&& other->client->pers.weapon != it_proxyminelauncher)
		{
			if (other->enemy)
			{
				VectorSubtract(other->enemy->s.origin, other->s.origin, dir);
				dist = VectorLength(dir);
				if (dist > IDEAL_ENEMY_DIST)
					Bot_BestFarWeapon (other);
				else if (dist >= MELEE_DIST)
					Bot_BestMidWeapon (other);
				else
					Bot_BestCloseWeapon (other);
			}
			else
				Bot_BestFarWeapon (other);
		}
	}
	return true;
}

//MATTHIAS
qboolean Pickup_NoAmmoWeapon (edict_t *ent, edict_t *other)
{
	int			index;
	vec3_t		dir;
	vec_t		dist;

	index = ITEM_INDEX(ent->item);

	if ( ( ((int)(dmflags->value) & DF_WEAPONS_STAY) || coop->value) 
		&& other->client->pers.inventory[index])
	{
		if (!(ent->spawnflags & (DROPPED_ITEM | DROPPED_PLAYER_ITEM) ) )
			return false;	// leave the weapon for others to pickup
	}

	other->client->pers.inventory[index]++;

	if (!(ent->spawnflags & DROPPED_ITEM) )
	{
		if (! (ent->spawnflags & DROPPED_PLAYER_ITEM) )
		{
			if (deathmatch->value)
			{
				if ((int)(dmflags->value) & DF_WEAPONS_STAY)
					ent->flags |= FL_RESPAWN;
				else
					SetRespawn (ent, 30);
			}
			if (coop->value)
				ent->flags |= FL_RESPAWN;
		}
	}

	if (other->client->pers.weapon != ent->item && 
		(other->client->pers.inventory[index] == 1) &&
		( !deathmatch->value || other->client->pers.weapon == FindItem("AK42 Assault Pistol") ) )
		other->client->newweapon = ent->item;

	if (strcmp(other->classname,"bot") == 0)
	{
		if (other->enemy)
		{
			VectorSubtract(other->enemy->s.origin, other->s.origin, dir);
			dist = VectorLength(dir);
			if (dist > IDEAL_ENEMY_DIST)
				Bot_BestFarWeapon (other);
			else if (dist >= MELEE_DIST)
				Bot_BestMidWeapon (other);
			else
				Bot_BestCloseWeapon (other);
		}
		else
			Bot_BestFarWeapon (other);
	}
	return true;
}


/*
===============
ChangeWeapon

The old weapon has been dropped all the way, so make the new one
current
===============
*/
void ChangeWeapon (edict_t *ent)
{
	if (ent->client->grenade_time)
	{
		ent->client->grenade_time = level.time;
		ent->client->weapon_sound = 0;
		weapon_grenade_fire (ent, false);
		ent->client->grenade_time = 0;
	}

	ent->client->pers.lastweapon = ent->client->pers.weapon;
	ent->client->pers.weapon = ent->client->newweapon;
	ent->client->newweapon = NULL;
	ent->client->machinegun_shots = 0;

	if (ent->client->pers.weapon && ent->client->pers.weapon->ammo)
		ent->client->ammo_index = ITEM_INDEX(FindItem(ent->client->pers.weapon->ammo));
	else
		ent->client->ammo_index = 0;

	if (!ent->client->pers.weapon
		|| (ent->s.modelindex != 255 && ent->client->invisible != true)) // vwep
	{	// dead, or not on client, so VWep animations could do wacky things
		ent->client->ps.gunindex = 0;
		return;
	}

	ent->client->weaponstate = WEAPON_ACTIVATING;
	ent->client->ps.gunframe = 0;
	ent->client->ps.gunindex = gi.modelindex(ent->client->pers.weapon->view_model);
	
	ShowGun(ent);

	if (Q_stricmp(ent->classname, "bot") == 0)
		ent->client->b_nextwchange = level.time + 5;

    LaserSight_Check (ent);
}

/*
=================
NoAmmoWeaponChange
=================
*/
void NoAmmoWeaponChange (edict_t *ent)
{
	vec3_t	dir;
	vec_t	dist;

	if (strcmp(ent->classname,"bot") == 0)	//MATTHIAS
	{
		if (ent->enemy)
		{
			VectorSubtract(ent->enemy->s.origin, ent->s.origin, dir);
			dist = VectorLength(dir);
			if (dist > IDEAL_ENEMY_DIST)
				Bot_BestFarWeapon (ent);
			else if (dist >= MELEE_DIST)
				Bot_BestMidWeapon (ent);
			else
				Bot_BestCloseWeapon (ent);
		}
		else
			Bot_BestFarWeapon (ent);

		ent->client->b_nextshot = level.time + CHANGEWEAPON_DELAY;
	}
	else
	{
//MATTHIAS - chaos dm weapons added
		if ( ent->client->pers.inventory[ITEM_INDEX(it_buzzes)]
			&&  ent->client->pers.inventory[ITEM_INDEX(it_buzzsaw)])
		{
			ent->client->newweapon = it_buzzsaw;
			return;
		}
        if( ! it_nuke )
            it_nuke = FindItem("nuke");
		if ( ent->client->pers.inventory[ITEM_INDEX(it_nuke)])
		{
			ent->client->newweapon = it_nuke;
			return;
		}
		if (ent->client->pers.inventory[ITEM_INDEX(it_slugs)]
			&&  ent->client->pers.inventory[ITEM_INDEX(it_railgun)])
		{
			ent->client->newweapon = it_railgun;
			return;
		}
		if (ent->client->pers.inventory[ITEM_INDEX(it_cells)]
			&&  ent->client->pers.inventory[ITEM_INDEX(it_hyperblaster)])
		{
			ent->client->newweapon = it_hyperblaster;
			return;
		}
		if (ent->client->pers.inventory[ITEM_INDEX(it_cells)]
			&&  ent->client->pers.inventory[ITEM_INDEX(it_plasma)])
		{
			ent->client->newweapon = it_plasma;
			return;
		}
		if( ! it_airfist )
			it_airfist = FindItem( "airgun" );
		if ( it_airfist && ent->client->pers.inventory[ITEM_INDEX(it_airfist)])
		{
			ent->client->newweapon = it_airfist;
			return;
		}
		if (ent->client->pers.inventory[ITEM_INDEX(it_explosivearrows)]
			&&  ent->client->pers.inventory[ITEM_INDEX(it_explosivecrossbow)])
		{
			ent->client->newweapon = it_explosivecrossbow;
			return;
		}
		if (ent->client->pers.inventory[ITEM_INDEX(it_poisonarrows)]
			&&  ent->client->pers.inventory[ITEM_INDEX(it_poisoncrossbow)])
		{
			ent->client->newweapon = it_poisoncrossbow;
			return;
		}
		if (ent->client->pers.inventory[ITEM_INDEX(it_arrows)]
			&&  ent->client->pers.inventory[ITEM_INDEX(it_crossbow)])
		{
			ent->client->newweapon = it_crossbow;
			return;
		}
		if (ent->client->pers.inventory[ITEM_INDEX(it_eshells)] > 1
			&&  ent->client->pers.inventory[ITEM_INDEX(it_esupershotgun)])
		{
			ent->client->newweapon = it_esupershotgun;
			return;
		}
		if (ent->client->pers.inventory[ITEM_INDEX(it_shells)] > 1
			&&  ent->client->pers.inventory[ITEM_INDEX(it_supershotgun)])
		{
			ent->client->newweapon = it_supershotgun;
			return;
		}
		if (ent->client->pers.inventory[ITEM_INDEX(it_sword)])
		{
			ent->client->newweapon = it_sword;
			return;
		}
		ent->client->newweapon = FindItem ("AK42 Assault Pistol");
	}
}

/*
=================
Think_Weapon

Called by ClientBeginServerFrame and ClientThink
=================
*/
void Think_Weapon (edict_t *ent)
{
	// if just died, put the weapon away
	if (ent->health < 1)
	{
		ent->client->newweapon = NULL;
		ChangeWeapon (ent);
	}

	// call active weapon think routine
	if (ent->client->pers.weapon && ent->client->pers.weapon->weaponthink)
	{
		is_quad = (ent->client->quad_framenum > level.framenum);
		if (ent->client->silencer_shots)
			is_silenced = MZ_SILENCED;
		else
			is_silenced = 0;
		ent->client->pers.weapon->weaponthink (ent);
	}
}


/*
================
Use_Weapon

Make the weapon ready if there is ammo
================
*/
void Use_Weapon (edict_t *ent, gitem_t *item)
{
	int			ammo_index;
	gitem_t		*ammo_item;

	// see if we're already using it
	if (item == ent->client->pers.weapon)
		return;

	if (item->ammo && !g_select_empty->value && !(item->flags & IT_AMMO))
	{
		ammo_item = FindItem(item->ammo);
		ammo_index = ITEM_INDEX(ammo_item);

		if (!ent->client->pers.inventory[ammo_index])
		{
			cprintf2 (ent, PRINT_HIGH, "No %s for %s.\n", ammo_item->pickup_name, item->pickup_name);
			return;
		}

		if (ent->client->pers.inventory[ammo_index] < item->quantity)
		{
			cprintf2 (ent, PRINT_HIGH, "Not enough %s for %s (needs %i at least, only get %i...).\n", ammo_item->pickup_name, item->pickup_name, item->quantity, ent->client->pers.inventory[ammo_index]);
			return;
		}
	}

	// change to this weapon when down
	ent->client->newweapon = item;
}



/*
================
Drop_Weapon
================
*/
void Drop_Weapon (edict_t *ent, gitem_t *item)
{
	int		index;

	if ((int)(dmflags->value) & DF_WEAPONS_STAY)
		return;

	index = ITEM_INDEX(item);
	// see if we're already using it
	if ( ((item == ent->client->pers.weapon) || (item == ent->client->newweapon))&& (ent->client->pers.inventory[index] == 1) )
	{
		cprintf2 (ent, PRINT_HIGH, "Can't drop current weapon\n");
		return;
	}

	//MATTHIAS
	if (item == it_supershotgun
		|| item == it_esupershotgun)
	{
		ent->client->pers.inventory[ITEM_INDEX(it_supershotgun)]--;
		ent->client->pers.inventory[ITEM_INDEX(it_esupershotgun)]--;
		item = it_supershotgun;
	}
	else if (item == it_rocketlauncher
		|| item == it_hominglauncher)
	{
		ent->client->pers.inventory[ITEM_INDEX(it_rocketlauncher)]--;
		ent->client->pers.inventory[ITEM_INDEX(it_hominglauncher)]--;
		item = it_rocketlauncher;
	}
	else if (item == it_grenadelauncher
		|| item == it_flashgrenadelauncher
		|| item == it_poisongrenadelauncher)
	{
		ent->client->pers.inventory[ITEM_INDEX(it_grenadelauncher)]--;
		ent->client->pers.inventory[ITEM_INDEX(it_flashgrenadelauncher)]--;
		ent->client->pers.inventory[ITEM_INDEX(it_poisongrenadelauncher)]--;
		item = it_grenadelauncher;
	}
	else if (item == it_crossbow
		|| item == it_poisoncrossbow
		|| item == it_explosivecrossbow)
	{
		ent->client->pers.inventory[ITEM_INDEX(it_crossbow)]--;
		ent->client->pers.inventory[ITEM_INDEX(it_poisoncrossbow)]--;
		ent->client->pers.inventory[ITEM_INDEX(it_explosivecrossbow)]--;
		item = it_crossbow;
	}
	else
		ent->client->pers.inventory[index]--;

	Drop_Item (ent, item);
}


/*
================
Weapon_Generic

A generic function to handle the basics of weapon thinking
================
*/
#define FRAME_FIRE_FIRST		(FRAME_ACTIVATE_LAST + 1)
#define FRAME_IDLE_FIRST		(FRAME_FIRE_LAST + 1)
#define FRAME_DEACTIVATE_FIRST	(FRAME_IDLE_LAST + 1)

static void Weapon_Generic2 (edict_t *ent, int FRAME_ACTIVATE_LAST, int FRAME_FIRE_LAST, int FRAME_IDLE_LAST, int FRAME_DEACTIVATE_LAST, int *pause_frames, int *fire_frames, void (*fire)(edict_t *ent))
{
	int		n;

	if (ent->s.modelindex != 255 && ent->client->invisible != true) // vwep
		return; // not on client, so VWep animations could do wacky things

	if (ent->client->weaponstate == WEAPON_DROPPING)
	{
		if (ent->client->ps.gunframe == FRAME_DEACTIVATE_LAST)
		{
			ChangeWeapon (ent);
			return;
		}
		
		ent->client->ps.gunframe++;
		return;
	}

	if (ent->client->weaponstate == WEAPON_ACTIVATING)
	{
		if (ent->client->ps.gunframe == FRAME_ACTIVATE_LAST)
		{
			ent->client->weaponstate = WEAPON_READY;
			ent->client->ps.gunframe = FRAME_IDLE_FIRST;
			return;
		}

		ent->client->ps.gunframe++;
		return;
	}

	if ((ent->client->newweapon) && (ent->client->weaponstate != WEAPON_FIRING))
	{
		ent->client->weaponstate = WEAPON_DROPPING;
		ent->client->ps.gunframe = FRAME_DEACTIVATE_FIRST;
		return;
	}

	if (ent->client->weaponstate == WEAPON_READY)
	{
		if ( ((ent->client->latched_buttons|ent->client->buttons) & BUTTON_ATTACK) )
		{
			ent->client->latched_buttons &= ~BUTTON_ATTACK;
			if ((!ent->client->ammo_index) || 
				( ent->client->pers.inventory[ent->client->ammo_index] >= ent->client->pers.weapon->quantity))
			{
				ent->client->ps.gunframe = FRAME_FIRE_FIRST;
				ent->client->weaponstate = WEAPON_FIRING;

				// start the animation
				ent->client->anim_priority = ANIM_ATTACK;
				if (ent->client->ps.pmove.pm_flags & PMF_DUCKED)
				{
					ent->s.frame = FRAME_crattak1-1;
					ent->client->anim_end = FRAME_crattak9;
				}
				else
				{
					ent->s.frame = FRAME_attack1-1;
					ent->client->anim_end = FRAME_attack8;
				}
			}
			else
			{
				if (level.time >= ent->pain_debounce_time)
				{
					gi.sound(ent, CHAN_VOICE, gi.soundindex("weapons/noammo.wav"), 1, ATTN_NORM, 0);
					ent->pain_debounce_time = level.time + 1;
				}
				NoAmmoWeaponChange (ent);
			}
		}
		else
		{
			if (ent->client->ps.gunframe == FRAME_IDLE_LAST)
			{
				ent->client->ps.gunframe = FRAME_IDLE_FIRST;
				return;
			}

			if (pause_frames)
			{
				for (n = 0; pause_frames[n]; n++)
				{
					if (ent->client->ps.gunframe == pause_frames[n])
					{
						if (rand()&15)
							return;
					}
				}
			}

			ent->client->ps.gunframe++;
			return;
		}
	}

	if (ent->client->weaponstate == WEAPON_FIRING)
	{
		for (n = 0; fire_frames[n]; n++)
		{
			if (ent->client->ps.gunframe == fire_frames[n])
			{
//ZOID
				if (!CTFApplyStrengthSound(ent))
//ZOID
				if (ent->client->quad_framenum > level.framenum)
					gi.sound(ent, CHAN_ITEM, gi.soundindex("items/damage3.wav"), 1, ATTN_NORM, 0);
//ZOID
				//CTFApplyHasteSound(ent);
//ZOID

				fire (ent);
				break;
			}
		}

		if (!fire_frames[n])
			ent->client->ps.gunframe++;

		if (ent->client->ps.gunframe == FRAME_IDLE_FIRST+1)
			ent->client->weaponstate = WEAPON_READY;
	}
}

//ZOID
void Weapon_Generic (edict_t *ent, int FRAME_ACTIVATE_LAST, int FRAME_FIRE_LAST, int FRAME_IDLE_LAST, int FRAME_DEACTIVATE_LAST, int *pause_frames, int *fire_frames, void (*fire)(edict_t *ent))
{
	int oldstate = ent->client->weaponstate;

	if (ent->s.modelindex != 255 && ent->client->invisible != true) // vwep
		return; // not on client, so VWep animations could do wacky things

	Weapon_Generic2 (ent, FRAME_ACTIVATE_LAST, FRAME_FIRE_LAST, 
		FRAME_IDLE_LAST, FRAME_DEACTIVATE_LAST, pause_frames, 
		fire_frames, fire);

	// run the weapon frame again if hasted
	if ( ( ent->client->pers.weapon == it_chainsaw
		|| ent->client->pers.weapon == it_sword
		|| ent->client->pers.weapon == it_hyperblaster )
		&& ent->client->weaponstate == WEAPON_FIRING )
		return;

	if ( CTFApplyHaste(ent) || (( ent->client->pers.weapon == it_grapple
			&& ent->client->weaponstate != WEAPON_FIRING ) &&
		    oldstate == ent->client->weaponstate ) ) {
		Weapon_Generic2 (ent, FRAME_ACTIVATE_LAST, FRAME_FIRE_LAST, 
			FRAME_IDLE_LAST, FRAME_DEACTIVATE_LAST, pause_frames, 
			fire_frames, fire);
	}
}
//ZOID

/*
======================================================================

GRENADE

======================================================================
*/

#define GRENADE_TIMER		3.0
#define GRENADE_MINSPEED	400
#define GRENADE_MAXSPEED	800

void weapon_grenade_fire (edict_t *ent, qboolean held)
{
	vec3_t	offset;
	vec3_t	forward, right;
	vec3_t	start;
	int		damage = 160;
	float	timer;
	int		speed;
	float	radius;

	radius = damage+40;
	if (is_quad)
		damage *= 4;

	VectorSet(offset, 8, 8, ent->viewheight-8);
	AngleVectors (ent->client->v_angle, forward, right, NULL);
	P_ProjectSource (ent->client, ent->s.origin, offset, forward, right, start);

	timer = ent->client->grenade_time - level.time;
	speed = GRENADE_MINSPEED + (GRENADE_TIMER - timer) * ((GRENADE_MAXSPEED - GRENADE_MINSPEED) / GRENADE_TIMER);
	fire_grenade2 (ent, start, forward, damage, speed, timer, radius, held);

	//vwep
	if (ent->client->ps.pmove.pm_flags & PMF_DUCKED && ent->health > 0)
	{
		ent->client->anim_priority = ANIM_ATTACK;
		ent->s.frame = FRAME_crattak1-1;
		ent->client->anim_end = FRAME_crattak3;
	}
	else if(ent->s.modelindex != 255 && ent->health > 0)
	{
		ent->client->anim_priority = ANIM_REVERSE;
		ent->s.frame = FRAME_wave08;
		ent->client->anim_end = FRAME_wave01;
	}

	if (! ( (int)dmflags->value & DF_INFINITE_AMMO ) )
		ent->client->pers.inventory[ent->client->ammo_index]--;

	ent->client->grenade_time = level.time + 1.0;
}

void Weapon_Grenade (edict_t *ent)
{
	if ((ent->client->newweapon) && (ent->client->weaponstate == WEAPON_READY))
	{
		ChangeWeapon (ent);
		return;
	}

	if (ent->client->weaponstate == WEAPON_ACTIVATING)
	{
		ent->client->weaponstate = WEAPON_READY;
		ent->client->ps.gunframe = 16;
		return;
	}

	if (ent->client->weaponstate == WEAPON_READY)
	{
		if ( ((ent->client->latched_buttons|ent->client->buttons) & BUTTON_ATTACK) )
		{
			ent->client->latched_buttons &= ~BUTTON_ATTACK;
			if (ent->client->pers.inventory[ent->client->ammo_index])
			{
				ent->client->ps.gunframe = 1;
				ent->client->weaponstate = WEAPON_FIRING;
				ent->client->grenade_time = 0;
			}
			else
			{
				if (level.time >= ent->pain_debounce_time)
				{
					gi.sound(ent, CHAN_VOICE, gi.soundindex("weapons/noammo.wav"), 1, ATTN_NORM, 0);
					ent->pain_debounce_time = level.time + 1;
				}
				NoAmmoWeaponChange (ent);
			}
			return;
		}

		if ((ent->client->ps.gunframe == 29) || (ent->client->ps.gunframe == 34) || (ent->client->ps.gunframe == 39) || (ent->client->ps.gunframe == 48))
		{
			if (rand()&15)
				return;
		}

		if (++ent->client->ps.gunframe > 48)
			ent->client->ps.gunframe = 16;
		return;
	}

	if (ent->client->weaponstate == WEAPON_FIRING)
	{
		if (ent->client->ps.gunframe == 5)
			gi.sound(ent, CHAN_WEAPON, gi.soundindex("weapons/hgrena1b.wav"), 1, ATTN_NORM, 0);

		if (ent->client->ps.gunframe == 11)
		{
			if (!ent->client->grenade_time)
			{
				ent->client->grenade_time = level.time + GRENADE_TIMER + 0.2;
				ent->client->weapon_sound = gi.soundindex("weapons/hgrenc1b.wav");
			}

			// they waited too long, detonate it in their hand
			if (!ent->client->grenade_blew_up && level.time >= ent->client->grenade_time)
			{
				ent->client->weapon_sound = 0;
				weapon_grenade_fire (ent, true);
				ent->client->grenade_blew_up = true;
			}

			if (ent->client->buttons & BUTTON_ATTACK)
				return;

			if (ent->client->grenade_blew_up)
			{
				if (level.time >= ent->client->grenade_time)
				{
					ent->client->ps.gunframe = 15;
					ent->client->grenade_blew_up = false;
				}
				else
				{
					return;
				}
			}
		}

		if (ent->client->ps.gunframe == 12)
		{
			ent->client->weapon_sound = 0;
			weapon_grenade_fire (ent, false);
		}

		if ((ent->client->ps.gunframe == 15) && (level.time < ent->client->grenade_time))
			return;

		ent->client->ps.gunframe++;

		if (ent->client->ps.gunframe == 16)
		{
			ent->client->grenade_time = 0;
			ent->client->weaponstate = WEAPON_READY;
		}
	}
}

/*
======================================================================

GRENADE LAUNCHER

======================================================================
*/

void weapon_grenadelauncher_fire (edict_t *ent)
{
	vec3_t	offset;
	vec3_t	forward, right;
	vec3_t	start;
	int		damage = 160;
	float	radius;

	radius = damage+40;
	if (is_quad)
		damage *= 4;

	VectorSet(offset, 8, 8, ent->viewheight-8);
	AngleVectors (ent->client->v_angle, forward, right, NULL);
	P_ProjectSource (ent->client, ent->s.origin, offset, forward, right, start);

	VectorScale (forward, -2, ent->client->kick_origin);
	ent->client->kick_angles[0] = -1;

	fire_grenade (ent, start, forward, damage, 600, 2.5, radius);

	gi.WriteByte (svc_muzzleflash);
	gi.WriteShort (ent-g_edicts);
	gi.WriteByte (MZ_GRENADE | is_silenced);
	gi.multicast (ent->s.origin, MULTICAST_PVS);

	ent->client->ps.gunframe++;

	PlayerNoise(ent, start, PNOISE_WEAPON);

	if (! ( (int)dmflags->value & DF_INFINITE_AMMO ) )
		ent->client->pers.inventory[ent->client->ammo_index]--;
}

void Weapon_GrenadeLauncher (edict_t *ent)
{
	static int	pause_frames[]	= {34, 51, 59, 0};
	static int	fire_frames[]	= {6, 0};

	Weapon_Generic (ent, 5, 16, 59, 64, pause_frames, fire_frames, weapon_grenadelauncher_fire);
}

/*
======================================================================

ROCKET

======================================================================
*/

void Weapon_RocketLauncher_Fire (edict_t *ent)
{
	vec3_t	offset, start;
	vec3_t	forward, right;
	int		damage;
	float	damage_radius;
	int		radius_damage;

	damage = 100 + (int)(random() * 20.0);
	radius_damage = 120;
	damage_radius = 120;
	if (is_quad)
	{
		damage *= 4;
		radius_damage *= 4;
	}

	AngleVectors (ent->client->v_angle, forward, right, NULL);

	VectorScale (forward, -2, ent->client->kick_origin);
	ent->client->kick_angles[0] = -1;

	VectorSet(offset, 8, 8, ent->viewheight-8);
	P_ProjectSource (ent->client, ent->s.origin, offset, forward, right, start);
	fire_rocket (ent, start, forward, damage, 800, damage_radius, radius_damage);

	// send muzzle flash
	gi.WriteByte (svc_muzzleflash);
	gi.WriteShort (ent-g_edicts);
	gi.WriteByte (MZ_ROCKET | is_silenced);
	gi.multicast (ent->s.origin, MULTICAST_PVS);

	ent->client->ps.gunframe++;

	PlayerNoise(ent, start, PNOISE_WEAPON);

	if (! ( (int)dmflags->value & DF_INFINITE_AMMO ) )
		ent->client->pers.inventory[ent->client->ammo_index]--;
}

void Weapon_GuidedRocket_Fire (edict_t *ent)
{
    vec3_t  offset, start;
    vec3_t  forward, right;
    int     damage;
    float   damage_radius;
    int     radius_damage;

    damage = 100 + (int)(random() * 20.0);
    radius_damage = 120;
    damage_radius = 120;
    if (is_quad)
    {
        damage *= 4;
        radius_damage *= 4;
    }

    AngleVectors (ent->client->v_angle, forward, right, NULL);

    VectorScale (forward, -2, ent->client->kick_origin);
    ent->client->kick_angles[0] = -1;

    VectorSet(offset, 8, 8, ent->viewheight-8);
    P_ProjectSource (ent->client, ent->s.origin, offset, forward, right, start);
    fire_guidedrocket (ent, start, forward, damage, 650, damage_radius, radius_damage);

    // send muzzle flash
    gi.WriteByte (svc_muzzleflash);
    gi.WriteShort (ent-g_edicts);
    gi.WriteByte (MZ_ROCKET | is_silenced);
    gi.multicast (ent->s.origin, MULTICAST_PVS);

    ent->client->ps.gunframe++;

    PlayerNoise(ent, start, PNOISE_WEAPON);
    if (! ((int)dmflags->value & DF_INFINITE_AMMO))
        ent->client->pers.inventory[ent->client->ammo_index]
            -= ent->client->pers.weapon->quantity;
}

void Weapon_RocketLauncher (edict_t *ent)
{
	static int	pause_frames[]	= {25, 33, 42, 50, 0};
	static int	fire_frames[]	= {5, 0};

    // DISABLE classical rocket launcher
    //Weapon_Generic (ent, 4, 12, 50, 54, pause_frames, fire_frames, Weapon_RocketLauncher_Fire);
    Weapon_Generic (ent, 4, 12, 50, 54, pause_frames, fire_frames, Weapon_GuidedRocket_Fire);

}


/*
======================================================================

BLASTER / HYPERBLASTER

======================================================================
*/

void Blaster_Fire (edict_t *ent, vec3_t g_offset, int damage, qboolean hyper, int effect)
{
	vec3_t	forward, right;
	vec3_t	start;
	vec3_t	offset;

	if (is_quad)
		damage *= 4;
	AngleVectors (ent->client->v_angle, forward, right, NULL);
	VectorSet(offset, 12, 6, ent->viewheight-8);
	VectorAdd (offset, g_offset, offset);
	P_ProjectSource (ent->client, ent->s.origin, offset, forward, right, start);

	VectorScale (forward, -2, ent->client->kick_origin);
	ent->client->kick_angles[0] = -1;

	if (hyper)
		fire_blaster (ent, start, forward, damage, 1000, effect, hyper);
	else
		fire_bullet (ent, start, forward, damage, 40, 0, 0, MOD_AK42);

	// send muzzle flash
	if (hyper)
	{
		gi.WriteByte (svc_muzzleflash);
		gi.WriteShort (ent-g_edicts);
		gi.WriteByte (MZ_HYPERBLASTER | is_silenced);
		gi.multicast (ent->s.origin, MULTICAST_PVS);
	}
	else
	{
		edict_t *chunk;

		gi.WriteByte (svc_muzzleflash);
		gi.WriteShort (ent-g_edicts);
		gi.WriteByte (MZ_BOOMERGUN);
		gi.multicast (ent->s.origin, MULTICAST_PVS);
		gi.sound(ent, CHAN_WEAPON, gi.soundindex("weapons/ak42.wav"), 1, ATTN_NORM, 0);

		chunk = G_Spawn();
		VectorCopy (start, chunk->s.origin);
		VectorCopy (ent->s.angles, chunk->s.angles);
		chunk->s.origin[2] +=8;
		chunk->s.angles[YAW] +=60;
		gi.setmodel (chunk, "models/objects/shell1/tris.md2");
		chunk->velocity[0] = right[0] * -80 - random() * 30;
		chunk->velocity[1] = right[1] * -80 - random() * 30;
		chunk->velocity[2] = 80;

		chunk->movetype = MOVETYPE_BOUNCE;
		chunk->solid = SOLID_NOT;
		chunk->avelocity[0] = random()*500;
		chunk->avelocity[1] = random()*500;
		chunk->avelocity[2] = random()*500;
		chunk->think = G_FreeEdict;
		chunk->nextthink = level.time + 2;
		chunk->s.frame = 0;
		chunk->flags = 0;
		chunk->classname = "shell";
		gi.linkentity (chunk);
	}
	

	PlayerNoise(ent, start, PNOISE_WEAPON);
}


void Weapon_AK42_Fire (edict_t *ent)
{
	int		damage;

	if (ent->client->fakedeath != 0)
		return;

	damage = 20;
	
	Blaster_Fire (ent, vec3_origin, damage, false, EF_BLASTER);
	ent->client->ps.gunframe++;
}

void Weapon_AK42 (edict_t *ent)
{
	static int	pause_frames[]	= {19, 32, 0};
	static int	fire_frames[]	= {5, 0};

	Weapon_Generic (ent, 4, 8, 52, 55, pause_frames, fire_frames, Weapon_AK42_Fire);
}


void Weapon_HyperBlaster_Fire (edict_t *ent)
{
	float	rotation;
	vec3_t	offset;
	int		effect;
	int		damage;

	ent->client->weapon_sound = gi.soundindex("weapons/hyprbl1a.wav");

	if (!(ent->client->buttons & BUTTON_ATTACK))
	{
		ent->client->ps.gunframe++;
	}
	else
	{
		if (! ent->client->pers.inventory[ent->client->ammo_index] )
		{
			if (level.time >= ent->pain_debounce_time)
			{
				gi.sound(ent, CHAN_VOICE, gi.soundindex("weapons/noammo.wav"), 1, ATTN_NORM, 0);
				ent->pain_debounce_time = level.time + 1;
			}
			NoAmmoWeaponChange (ent);
		}
		else
		{
			rotation = (ent->client->ps.gunframe - 5) * 2*M_PI/6;
			offset[0] = -4 * sin(rotation);
			offset[1] = 0;
			offset[2] = 4 * cos(rotation);

			if ((ent->client->ps.gunframe == 6) || (ent->client->ps.gunframe == 10))
				effect = EF_HYPERBLASTER;
			else
				effect = 0;
			damage = 15;

			Blaster_Fire (ent, offset, damage, true, effect);
			//Blaster_Fire (ent, offset, damage, true, EF_HYPERBLASTER);

			//vwep
			ent->client->anim_priority = ANIM_ATTACK;
			if (ent->client->ps.pmove.pm_flags & PMF_DUCKED)
			{
				ent->s.frame = FRAME_crattak1 - 1;
				ent->client->anim_end = FRAME_crattak9;
			}
			else
			{
				ent->s.frame = FRAME_attack1 - 1;
				ent->client->anim_end = FRAME_attack8;
			}

			if (! ( (int)dmflags->value & DF_INFINITE_AMMO ) )
				ent->client->pers.inventory[ent->client->ammo_index]--;
		}

		ent->client->ps.gunframe++;
		if (ent->client->ps.gunframe == 12 && ent->client->pers.inventory[ent->client->ammo_index])
			ent->client->ps.gunframe = 6;
	}

	if (ent->client->ps.gunframe == 12)
	{
		gi.sound(ent, CHAN_AUTO, gi.soundindex("weapons/hyprbd1a.wav"), 1, ATTN_NORM, 0);
		ent->client->weapon_sound = 0;
	}

}

void Weapon_HyperBlaster (edict_t *ent)
{
	static int	pause_frames[]	= {0};
	static int	fire_frames[]	= {6, 7, 8, 9, 10, 11, 0};

	Weapon_Generic (ent, 5, 20, 49, 53, pause_frames, fire_frames, Weapon_HyperBlaster_Fire);
}

/*
======================================================================

MACHINEGUN / CHAINGUN

======================================================================
*/

void Machinegun_Fire (edict_t *ent)
{
	int	i;
	vec3_t		start;
	vec3_t		forward, right;
	vec3_t		angles;
	int			damage = 8;
	int			kick = 2;
	vec3_t		offset;

	if (!(ent->client->buttons & BUTTON_ATTACK))
	{
		ent->client->machinegun_shots = 0;
		ent->client->ps.gunframe++;
		return;
	}

	if (ent->client->ps.gunframe == 5)
		ent->client->ps.gunframe = 4;
	else
		ent->client->ps.gunframe = 5;

	if (ent->client->pers.inventory[ent->client->ammo_index] < 1)
	{
		ent->client->ps.gunframe = 6;
		if (level.time >= ent->pain_debounce_time)
		{
			gi.sound(ent, CHAN_VOICE, gi.soundindex("weapons/noammo.wav"), 1, ATTN_NORM, 0);
			ent->pain_debounce_time = level.time + 1;
		}
		NoAmmoWeaponChange (ent);
		return;
	}

	if (is_quad)
	{
		damage *= 4;
		kick *= 4;
	}

	for (i=1 ; i<3 ; i++)
	{
		ent->client->kick_origin[i] = crandom() * 0.35;
		ent->client->kick_angles[i] = crandom() * 0.7;
	}
	ent->client->kick_origin[0] = crandom() * 0.35;
	ent->client->kick_angles[0] = ent->client->machinegun_shots * -1.5;

	// raise the gun as it is firing
	if (!deathmatch->value)
	{
		ent->client->machinegun_shots++;
		if (ent->client->machinegun_shots > 9)
			ent->client->machinegun_shots = 9;
	}

	// get start / end positions
	VectorAdd (ent->client->v_angle, ent->client->kick_angles, angles);
	AngleVectors (angles, forward, right, NULL);
	VectorSet(offset, 0, 8, ent->viewheight-8);
	P_ProjectSource (ent->client, ent->s.origin, offset, forward, right, start);
	fire_bullet (ent, start, forward, damage, kick, DEFAULT_BULLET_HSPREAD, DEFAULT_BULLET_VSPREAD, MOD_MACHINEGUN);

	gi.WriteByte (svc_muzzleflash);
	gi.WriteShort (ent-g_edicts);
	gi.WriteByte (MZ_MACHINEGUN | is_silenced);
	gi.multicast (ent->s.origin, MULTICAST_PVS);

	PlayerNoise(ent, start, PNOISE_WEAPON);

	if (! ( (int)dmflags->value & DF_INFINITE_AMMO ) )
		ent->client->pers.inventory[ent->client->ammo_index]--;
}

void Weapon_Machinegun (edict_t *ent)
{
	static int	pause_frames[]	= {23, 45, 0};
	static int	fire_frames[]	= {4, 5, 0};

	Weapon_Generic (ent, 3, 5, 45, 49, pause_frames, fire_frames, Machinegun_Fire);
}

void Chaingun_Fire (edict_t *ent)
{
	int			i;
	int			shots;
	vec3_t		start;
	vec3_t		forward, right, up;
	float		r, u;
	vec3_t		offset;
	int			damage;
	int			kick = 2;

	if (deathmatch->value)
		damage = 6;
	else
		damage = 8;

	if (ent->client->ps.gunframe == 5)
		gi.sound(ent, CHAN_AUTO, gi.soundindex("weapons/chngnu1a.wav"), 1, ATTN_IDLE, 0);

	if ((ent->client->ps.gunframe == 14) && !(ent->client->buttons & BUTTON_ATTACK))
	{
		ent->client->ps.gunframe = 32;
		ent->client->weapon_sound = 0;
		return;
	}
	else if ((ent->client->ps.gunframe == 21) && (ent->client->buttons & BUTTON_ATTACK)
		&& ent->client->pers.inventory[ent->client->ammo_index])
	{
		ent->client->ps.gunframe = 15;
	}
	else
	{
		ent->client->ps.gunframe++;
	}

	if (ent->client->ps.gunframe == 22)
	{
		ent->client->weapon_sound = 0;
		gi.sound(ent, CHAN_AUTO, gi.soundindex("weapons/chngnd1a.wav"), 1, ATTN_IDLE, 0);
	}
	else
	{
		ent->client->weapon_sound = gi.soundindex("weapons/chngnl1a.wav");
	}

	if (ent->client->ps.gunframe <= 9)
		shots = 1;
	else if (ent->client->ps.gunframe <= 14)
	{
		if (ent->client->buttons & BUTTON_ATTACK)
			shots = 2;
		else
			shots = 1;
	}
	else
		shots = 3;

	if (ent->client->pers.inventory[ent->client->ammo_index] < shots)
		shots = ent->client->pers.inventory[ent->client->ammo_index];

	if (!shots)
	{
		if (level.time >= ent->pain_debounce_time)
		{
			gi.sound(ent, CHAN_VOICE, gi.soundindex("weapons/noammo.wav"), 1, ATTN_NORM, 0);
			ent->pain_debounce_time = level.time + 1;
		}
		NoAmmoWeaponChange (ent);
		return;
	}

	if (is_quad)
	{
		damage *= 4;
		kick *= 4;
	}

	for (i=0 ; i<3 ; i++)
	{
		ent->client->kick_origin[i] = crandom() * 0.35;
		ent->client->kick_angles[i] = crandom() * 0.7;
	}

	for (i=0 ; i<shots ; i++)
	{
		// get start / end positions
		AngleVectors (ent->client->v_angle, forward, right, up);
		r = 7 + crandom()*4;
		u = crandom()*4;
		VectorSet(offset, 0, r, u + ent->viewheight-8);
		P_ProjectSource (ent->client, ent->s.origin, offset, forward, right, start);

		fire_bullet (ent, start, forward, damage, kick, DEFAULT_BULLET_HSPREAD, DEFAULT_BULLET_VSPREAD, MOD_CHAINGUN);
	}

	// send muzzle flash
	gi.WriteByte (svc_muzzleflash);
	gi.WriteShort (ent-g_edicts);
	gi.WriteByte ((MZ_CHAINGUN1 + shots - 1) | is_silenced);
	gi.multicast (ent->s.origin, MULTICAST_PVS);

	PlayerNoise(ent, start, PNOISE_WEAPON);

	if (! ( (int)dmflags->value & DF_INFINITE_AMMO ) )
		ent->client->pers.inventory[ent->client->ammo_index] -= shots;
}


void Weapon_Chaingun (edict_t *ent)
{
	static int	pause_frames[]	= {38, 43, 51, 61, 0};
	static int	fire_frames[]	= {5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 0};

	Weapon_Generic (ent, 4, 31, 61, 64, pause_frames, fire_frames, Chaingun_Fire);
}


/*
======================================================================

SHOTGUN / SUPERSHOTGUN

======================================================================
*/

void weapon_shotgun_fire (edict_t *ent)
{
	vec3_t		start;
	vec3_t		forward, right;
	vec3_t		offset;
	int			damage = 4;
	int			kick = 8;

	if (ent->client->ps.gunframe == 9)
	{
		ent->client->ps.gunframe++;
		return;
	}

	AngleVectors (ent->client->v_angle, forward, right, NULL);

	VectorScale (forward, -2, ent->client->kick_origin);
	ent->client->kick_angles[0] = -2;

	VectorSet(offset, 0, 8,  ent->viewheight-8);
	P_ProjectSource (ent->client, ent->s.origin, offset, forward, right, start);

	if (is_quad)
	{
		damage *= 4;
		kick *= 4;
	}

	if (deathmatch->value)
		fire_shotgun (ent, start, forward, damage, kick, 500, 500, DEFAULT_DEATHMATCH_SHOTGUN_COUNT, MOD_SHOTGUN);
	else
		fire_shotgun (ent, start, forward, damage, kick, 500, 500, DEFAULT_SHOTGUN_COUNT, MOD_SHOTGUN);

	// send muzzle flash
	gi.WriteByte (svc_muzzleflash);
	gi.WriteShort (ent-g_edicts);
	gi.WriteByte (MZ_SHOTGUN | is_silenced);
	gi.multicast (ent->s.origin, MULTICAST_PVS);

	ent->client->ps.gunframe++;
	PlayerNoise(ent, start, PNOISE_WEAPON);

	if (! ( (int)dmflags->value & DF_INFINITE_AMMO ) )
		ent->client->pers.inventory[ent->client->ammo_index]--;
}

void Weapon_Shotgun (edict_t *ent)
{
	static int	pause_frames[]	= {22, 28, 34, 0};
	static int	fire_frames[]	= {8, 9, 0};

	Weapon_Generic (ent, 7, 18, 36, 39, pause_frames, fire_frames, weapon_shotgun_fire);
}


void weapon_supershotgun_fire (edict_t *ent)
{
	vec3_t		start;
	vec3_t		forward, right;
	vec3_t		offset;
	vec3_t		v;
	int			damage = 6;
	int			kick = 12;

	AngleVectors (ent->client->v_angle, forward, right, NULL);

	VectorScale (forward, -2, ent->client->kick_origin);
	ent->client->kick_angles[0] = -2;

	VectorSet(offset, 0, 8,  ent->viewheight-8);
	P_ProjectSource (ent->client, ent->s.origin, offset, forward, right, start);

	if (is_quad)
	{
		damage *= 4;
		kick *= 4;
	}

	v[PITCH] = ent->client->v_angle[PITCH];
	v[YAW]   = ent->client->v_angle[YAW] - 5;
	v[ROLL]  = ent->client->v_angle[ROLL];
	AngleVectors (v, forward, NULL, NULL);
	fire_shotgun (ent, start, forward, damage, kick, DEFAULT_SHOTGUN_HSPREAD, DEFAULT_SHOTGUN_VSPREAD, DEFAULT_SSHOTGUN_COUNT/2, MOD_SSHOTGUN);
	v[YAW]   = ent->client->v_angle[YAW] + 5;
	AngleVectors (v, forward, NULL, NULL);
	fire_shotgun (ent, start, forward, damage, kick, DEFAULT_SHOTGUN_HSPREAD, DEFAULT_SHOTGUN_VSPREAD, DEFAULT_SSHOTGUN_COUNT/2, MOD_SSHOTGUN);

	// send muzzle flash
	gi.WriteByte (svc_muzzleflash);
	gi.WriteShort (ent-g_edicts);
	gi.WriteByte (MZ_SSHOTGUN | is_silenced);
	gi.multicast (ent->s.origin, MULTICAST_PVS);

	ent->client->ps.gunframe++;
	PlayerNoise(ent, start, PNOISE_WEAPON);

	if (! ( (int)dmflags->value & DF_INFINITE_AMMO ) )
		ent->client->pers.inventory[ent->client->ammo_index] -= 2;
}

void Weapon_SuperShotgun (edict_t *ent)
{
	static int	pause_frames[]	= {29, 42, 57, 0};
	static int	fire_frames[]	= {7, 0};

	Weapon_Generic (ent, 6, 17, 57, 61, pause_frames, fire_frames, weapon_supershotgun_fire);
}



/*
======================================================================

RAILGUN

======================================================================
*/

void weapon_railgun_fire (edict_t *ent)
{
	vec3_t		start;
	vec3_t		forward, right;
	vec3_t		offset;
	int			damage;
	int			kick;

	if (deathmatch->value)
	{	// normal damage is too extreme in dm
		damage = 100;
		kick = 200;
	}
	else
	{
		damage = 150;
		kick = 250;
	}

	if (is_quad)
	{
		damage *= 4;
		kick *= 4;
	}

	AngleVectors (ent->client->v_angle, forward, right, NULL);

	VectorScale (forward, -3, ent->client->kick_origin);
	ent->client->kick_angles[0] = -3;

	VectorSet(offset, 0, 7,  ent->viewheight-8);
	P_ProjectSource (ent->client, ent->s.origin, offset, forward, right, start);
	fire_rail (ent, start, forward, damage, kick);

	// send muzzle flash
	gi.WriteByte (svc_muzzleflash);
	gi.WriteShort (ent-g_edicts);
	gi.WriteByte (MZ_RAILGUN | is_silenced);
	gi.multicast (ent->s.origin, MULTICAST_PVS);

	ent->client->ps.gunframe++;
	PlayerNoise(ent, start, PNOISE_WEAPON);

	if (! ( (int)dmflags->value & DF_INFINITE_AMMO ) )
		ent->client->pers.inventory[ent->client->ammo_index]--;
}


void Weapon_Railgun (edict_t *ent)
{
	static int	pause_frames[]	= {56, 0};
	static int	fire_frames[]	= {4, 0};

	Weapon_Generic (ent, 3, 18, 56, 61, pause_frames, fire_frames, weapon_railgun_fire);
}


/*
======================================================================

BFG10K

======================================================================
*/

void weapon_bfg_fire (edict_t *ent)
{
	vec3_t	offset, start;
	vec3_t	forward, right;
	int		damage;
	float	damage_radius = 1000;

	if (deathmatch->value)
		damage = 200;
	else
		damage = 500;

	if (ent->client->ps.gunframe == 9)
	{
		// send muzzle flash
		gi.WriteByte (svc_muzzleflash);
		gi.WriteShort (ent-g_edicts);
		gi.WriteByte (MZ_BFG | is_silenced);
		gi.multicast (ent->s.origin, MULTICAST_PVS);

		ent->client->ps.gunframe++;

		PlayerNoise(ent, start, PNOISE_WEAPON);
		return;
	}

	// cells can go down during windup (from power armor hits), so
	// check again and abort firing if we don't have enough now
	if (ent->client->pers.inventory[ent->client->ammo_index] < 50)
	{
		ent->client->ps.gunframe++;
		return;
	}

	if (is_quad)
		damage *= 4;

	AngleVectors (ent->client->v_angle, forward, right, NULL);

	VectorScale (forward, -2, ent->client->kick_origin);

	// make a big pitch kick with an inverse fall
	ent->client->v_dmg_pitch = -40;
	ent->client->v_dmg_roll = crandom()*8;
	ent->client->v_dmg_time = level.time + DAMAGE_TIME;

	VectorSet(offset, 8, 8, ent->viewheight-8);
	P_ProjectSource (ent->client, ent->s.origin, offset, forward, right, start);
	fire_bfg (ent, start, forward, damage, 400, damage_radius);

	ent->client->ps.gunframe++;

	PlayerNoise(ent, start, PNOISE_WEAPON);

	if (! ( (int)dmflags->value & DF_INFINITE_AMMO ) )
		ent->client->pers.inventory[ent->client->ammo_index] -= 50;
}

void Weapon_BFG (edict_t *ent)
{
	static int	pause_frames[]	= {39, 45, 50, 55, 0};
	static int	fire_frames[]	= {9, 17, 0};

	Weapon_Generic (ent, 8, 32, 55, 58, pause_frames, fire_frames, weapon_bfg_fire);
}


//======================================================================
// Marsilainen's Plasma Rifle mod


void Plasma_Fire(edict_t *ent, vec3_t g_offset, int damage)
{
   vec3_t  forward, right;
   vec3_t  start;
   vec3_t  offset;
   int consume = 1;
   int sr;

   if (is_quad)
       damage *= 4;

   AngleVectors(ent->client->v_angle, forward, right, NULL);

   //small randomization
   sr = (int)(random() * 10.0);

   //Z, X, Y //
   VectorSet(offset, 50, 12, ent->viewheight - (9 + sr));
   P_ProjectSource(ent->client, ent->s.origin, offset, forward, right, start);

   VectorScale(forward, -2, ent->client->kick_origin);

   ent->client->kick_angles[0] = -1;

   //launch plasmaball
   fire_plasma(ent, start, forward, damage, 1500);

   //eats cells
   if (!((int)dmflags->value & DF_INFINITE_AMMO))
       ent->client->pers.inventory[ent->client->ammo_index] -= consume;

   //play firing sound
   gi.sound(ent, CHAN_WEAPON, gi.soundindex("weapons/plsmfire.wav"), 1, ATTN_NORM, 0);

   // send muzzle flash
   gi.WriteByte(svc_muzzleflash);
   gi.WriteShort(ent - g_edicts);
   gi.WriteByte(MZ_BLUEHYPERBLASTER | is_silenced);
   gi.multicast(ent->s.origin, MULTICAST_PVS);

   PlayerNoise(ent, start, PNOISE_WEAPON);

}

void Weapon_PlasmaRifle_Fire(edict_t *ent)
{
   int     damage;

   if (!(ent->client->buttons & BUTTON_ATTACK))
   {
       ent->client->ps.gunframe++;
   }
   else
   {
       if (!ent->client->pers.inventory[ent->client->ammo_index])
       {
           if (level.time >= ent->pain_debounce_time)
           {
               gi.sound(ent, CHAN_VOICE, gi.soundindex("weapons/noammo.wav"), 1, ATTN_NORM, 0);
               ent->pain_debounce_time = level.time + 1;
           }
           NoAmmoWeaponChange(ent);
       }
       else
       {

           if (deathmatch->value)
               damage = 20;
           else
               damage = 30;

           Plasma_Fire(ent, vec3_origin, damage);

           ent->client->anim_priority = ANIM_ATTACK;

           if (ent->client->ps.pmove.pm_flags & PMF_DUCKED)
           {
               ent->s.frame = FRAME_crattak1 - 1;
               ent->client->anim_end = FRAME_crattak9;
           }
           else
           {
               ent->s.frame = FRAME_attack1 - 1;
               ent->client->anim_end = FRAME_attack8;
           }

       }

       ent->client->ps.gunframe++;

       //loop
       if (ent->client->ps.gunframe == 11 && ent->client->pers.inventory[ent->client->ammo_index]) {
           ent->client->ps.gunframe = 9;
       }
   }

   /*
   //shooting ends
   if (ent->client->ps.gunframe == 11)
       gi.sound(ent, CHAN_AUTO, gi.soundindex("weapons/plsmend.wav"), 1, ATTN_NORM, 0);
       //ent->client->weapon_sound = 0;
   }
   */
}


void Weapon_PlasmaRifle(edict_t *ent)
{

   //pause idle animations
   static int  pause_frames[] = { 25, 29, 39 };

   //weapon fire frames
   static int  fire_frames[] = {9, 10 };

   Weapon_Generic(ent, 8, 18, 43, 50,  pause_frames, fire_frames, Weapon_PlasmaRifle_Fire);

}


//
//======================================================================
