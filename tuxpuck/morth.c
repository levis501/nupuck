/* morth.c
 * author: Daniel Beck*/

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "video.h"
#include "audio.h"
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
extern unsigned char sam_png[];
extern unsigned char tux_boos_ogg[];
extern unsigned char tux_apps_ogg[];

/* externals */
extern unsigned char morth_png[];

/* statics */
static char _name[] = "Morth";
/* unused? static Uint8 _serve_state = 0, _is_ready = 1;*/
static Uint8 _is_ready = 1;

/* functions */
static void _morth_free(AIPlayer * player) {
	SDL_FreeSurface(player->sdl_image);
	free(player);
}

static Uint8 _morth_reblit(AIPlayer * player, Uint32 time) {
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

/* unused?
static void _morth_serve(AIPlayer * player, Uint32 time) {
	static float counter, counter_speed, angle_limit;
	static Uint8 corner = 0;

	entity_move_towards(player->pad, 0, 35, 0.005, time);
	switch (_serve_state) {
	case SERVE_STATE_FLOAT_FORWARD:
		if (entity_move_towards((Entity *) player->puck, 0, 10 - SWIRL_RADIUS,
				FLOAT_SPEED, time) == 0) {
			_serve_state = SERVE_STATE_SWIRL;
			counter = -M_PI_2;
			counter_speed = 0.005;
			corner = (Uint8) ((float) rand() / RAND_MAX * 2);
			if (corner == 1)
				angle_limit = 2 * M_PI * 5 + M_PI + M_PI_2 / 2;
			else
				angle_limit = 2 * M_PI * 5 + M_PI;
		}
		break;
	case SERVE_STATE_SWIRL:
		counter_speed += time * COUNTER_ACC / 1000.0;
		counter += counter_speed * time;
		entity_set_position((Entity *) player->puck, SWIRL_RADIUS
				* cos(counter), 10 + SWIRL_RADIUS * sin(counter));
		if (counter > angle_limit) {
			_serve_state = SERVE_STATE_RELEASE;
			entity_set_position((Entity *) player->puck, SWIRL_RADIUS * cos(
					angle_limit), 10 + SWIRL_RADIUS * sin(angle_limit));
		}
		break;
	case SERVE_STATE_RELEASE:
		if (corner == 1)
			entity_set_velocity((Entity *) player->puck, RELEASE_SPEED * 0.477,
					RELEASE_SPEED * -0.879);
		else
			entity_set_velocity((Entity *) player->puck,
					RELEASE_SPEED * -0.140, RELEASE_SPEED * -0.990);
		player->set_state(player, PLAYER_STATE_IDLE);
		break;
	default:
		break;
	}
}*/

static void _morth_set_state(AIPlayer * player, Uint8 state) {
	player->state = state;
	player->strategy.serve = dumb_serve;
	switch (state) {
	case PLAYER_STATE_WIN_POINT:
		audio_play_sound(_snd_apps);
		break;
	case PLAYER_STATE_LOOSE_POINT:
		audio_play_sound(_snd_boos);
		break;
	default:
		break;
	}
}

AIPlayer *morth_create(Pad * pad, Puck * puck) {
	AIPlayer *player = NULL;

	player = malloc(sizeof(AIPlayer));
	memset(player, 0, sizeof(AIPlayer));

	player->sdl_image = video_create_png_surface(morth_png, NULL);
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
	player->speed = 2.5;
	player->hit_power = 2.0;
	player->state = PLAYER_STATE_IDLE;
	player->reblit = _morth_reblit;
	player->set_state = _morth_set_state;
	player->ready = _ready;
	player->free = _morth_free;
	player->strategy.idle = smart_idle;
	player->strategy.serve = dumb_serve;
	player->strategy.backup = smart_backup;
	player->strategy.aim = dumb_aim;
	player->strategy.hit = dumb_hit;
	return player;
}
