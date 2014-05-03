/* lex.c - Copyright (C) 2001-2002 Jacob Kroon, see COPYING for details */

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "audio.h"
#include "video.h"
#include "tuxpuck.h"

/* defines */
#define SERVE_STATE_FLOAT_FORWARD	1
#define SERVE_STATE_SWIRL		2
#define SERVE_STATE_RELEASE		3
#define SWIRL_RADIUS			((float)10.0)
#define FLOAT_SPEED			((float)0.01)
#define COUNTER_ACC			((float)0.01)
#define RELEASE_SPEED			((float)0.5)

static Sound *_snd_boos = NULL, *_snd_apps = NULL;

/* externals */
extern unsigned char lex_png[];
extern unsigned char tux_boos_ogg[];
extern unsigned char tux_apps_ogg[];

/* statics */
static char _name[] = "Lex";
static Uint8 _serve_state = 0, _is_ready = 1;

/* functions */
static void _lex_free(AIPlayer * player) {
	SDL_FreeSurface(player->sdl_image);
	audio_free_sound(_snd_apps);
	audio_free_sound(_snd_boos);
	free(player);
}

static Uint8 _lex_reblit(AIPlayer * player, Uint32 time) {
	switch (player->state) {
	case PLAYER_STATE_WIN_POINT:
	case PLAYER_STATE_LOOSE_POINT:
		_is_ready = 1;
		break;
	case PLAYER_STATE_WIN_GAME:
	case PLAYER_STATE_LOOSE_GAME:
		return 0;
	default:
		break;
	}
	return 1;
}

static Uint8 _ready(void) {
	return _is_ready;
}

static void _lex_set_state(AIPlayer * player, Uint8 state) {
	player->state = state;

	switch (state) {
	case PLAYER_STATE_WIN_POINT:
		audio_play_sound(_snd_apps);
		if (player->speed > 1.2) {
			player->speed = player->speed - 0.25;
			printf("player drinks - speed is %f\n", player->speed);
		}
		break;
	case PLAYER_STATE_LOOSE_POINT:
		audio_play_sound(_snd_boos);
		break;
	default:
		break;
	}
}

AIPlayer *lex_create(Pad * pad, Puck * puck) {
	AIPlayer *player = NULL;

	player = malloc(sizeof(AIPlayer));
	memset(player, 0, sizeof(AIPlayer));
	player->sdl_image = video_create_png_surface(lex_png, NULL);
	SDL_SetColorKey(player->sdl_image, SDL_SRCCOLORKEY, 177);
	player->rect.x = SCREEN_W / 2 - player->sdl_image->w / 2;
	player->rect.y = BOARD_TOP - player->sdl_image->h;
	player->rect.w = player->rect.h = 0;
	_snd_apps = audio_create_sound(tux_apps_ogg, NULL);
	_snd_boos = audio_create_sound(tux_boos_ogg, NULL);
	if (_snd_apps)
		audio_set_single(_snd_apps, 0);
	if (_snd_boos)
		audio_set_single(_snd_boos, 0);
	player->name = _name;
	player->pad = pad;
	player->puck = puck;
	player->speed = 3.5;
	player->hit_power = 2.3;
	player->state = PLAYER_STATE_IDLE;
	player->reblit = _lex_reblit;
	player->set_state = _lex_set_state;
	player->ready = _ready;
	player->free = _lex_free;
	player->strategy.idle = smart_idle;
	player->strategy.serve = dumb_serve;
	player->strategy.backup = smart_backup;
	player->strategy.aim = dumb_aim;
	player->strategy.hit = dumb_hit;
	return player;
}
