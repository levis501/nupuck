/* scoreboard.c - Copyright (C) 2001-2002 Jacob Kroon, see COPYING for details */

#include "video.h"
#include "tuxpuck.h"

#include "toolbox.h"

/* defines */
#define SCOREBOARD_POSITION	((Uint16)460)
#define SCOREBOARD_STATE_IDLE	1
#define SCOREBOARD_STATE_ADD_1	2
#define SCOREBOARD_STATE_ADD_2	3
#define LINE_WIDTH		((Uint32)3)
#define LINE_HEIGHT		((Uint32)22)
#define MOUSEBAR_TIMEOUT	((Uint32)2000)
#define MOUSEBAR_FADOUT_SPEED	((float)0.001)
#define EYEBAR_LEFT SCOREBOARD_POSITION+15
#define EYEBAR_WIDTH ((Uint16)120)
#define EYEBAR_RIGHT (EYEBAR_LEFT + EYEBAR_WIDTH)
#define EYEBAR_CENTER (EYEBAR_LEFT + (EYEBAR_WIDTH / 2))
#define EYEBAR_TOP ((Uint16)125)
#define EYE_ANGLE_MAX ((float) 0.5)
#define EYEBAR_SCALE (EYEBAR_WIDTH / (EYE_ANGLE_MAX*2))

/* externals */
extern unsigned char scoreboard_png[];
extern unsigned char mousebar_png[];
extern float _left_eye_angle;
extern float _right_eye_angle;
extern float _left_eye_width;
extern float _right_eye_width;


/* statics */
static SDL_Surface *_sdl_scoreboard, *_sdl_mousebar, *_sdl_eyebar;
static SDL_Rect _rect, _mousebar_rect, _eyebar_rect;
static Uint32 _counter = 0;
static Uint8 _score[2], _state, _mouse_speed = 0, _p1_scored, _p2_scored;
static float _mousebar_alpha = 0;

/* functions */
static void _blit_point(Uint8 who, Uint8 point) {
	SDL_Rect rect;

	point -= 1;
	rect.x = SCOREBOARD_POSITION + 18 + point * 8 + (point / 5 - 1) * 4;
	rect.y = 27 + who * 26;
	rect.w = LINE_WIDTH;
	rect.h = LINE_HEIGHT;
	video_fill(video_map_rgb(255, 255, 255), 255, &rect);
}

void scoreboard_init(void) {
	_score[0] = _score[1] = 0;
	_state = SCOREBOARD_STATE_IDLE;
	_sdl_scoreboard = video_create_png_surface(scoreboard_png, NULL);
	_sdl_mousebar = video_create_png_surface(mousebar_png, NULL);
  _sdl_eyebar = video_create_png_surface(mousebar_png, NULL);
	_rect.x = SCOREBOARD_POSITION;
	_rect.y = 0;
	_rect.w = 0;
	_rect.h = 0;
	_p1_scored = 0;
	_p2_scored = 0;
}

void scoreboard_deinit(void) {
	SDL_FreeSurface(_sdl_scoreboard);
	SDL_FreeSurface(_sdl_mousebar);
	SDL_FreeSurface(_sdl_eyebar);
}

/* Adds a point to the scoreboard. player can be FIRST_PLAYER
 * or SECOND_PLAYER.*/
void scoreboard_add_point(Uint8 player) {
	if (player == FIRST_PLAYER) {
		_state = SCOREBOARD_STATE_ADD_1;
	} else {
		_state = SCOREBOARD_STATE_ADD_2;
	}
}

void scoreboard_blit(void) {
	video_blit(_sdl_scoreboard, NULL, &_rect);
}

/*Erases the points displayed on the scoreboard.*/
void scoreboard_erase(void) {
	video_erase(&_rect);
}

void scoreboard_clean_up(void) {
	if (_mousebar_alpha > 0)
		video_erase(&_mousebar_rect);
  video_erase(&_eyebar_rect);
}

void scoreboard_set_alpha(Uint8 alpha) {
	video_set_alpha(_sdl_scoreboard, alpha);
}

void scoreboard_reblit(void) {
	SDL_Rect rect;

	if (_p1_scored) {
		_blit_point(0, _score[0]);
		_p1_scored = 0;
	} else if (_p2_scored) {
		_blit_point(1, _score[1]);
		_p2_scored = 0;
	}
	if (_mousebar_alpha > 0) {
		rect.x = SCOREBOARD_POSITION + 7;
		rect.y = 96;
		rect.w = (Uint16) (138.0 * _mouse_speed / 10.0);
		rect.h = 13;
		video_fill(video_map_rgb(0, 0, 128), (Uint8) (_mousebar_alpha * 255.0),
				&rect);
		_mousebar_rect.x = SCOREBOARD_POSITION;
		_mousebar_rect.y = 90;
		video_set_alpha(_sdl_mousebar, (Uint8) (_mousebar_alpha * 255.0));
		video_blit(_sdl_mousebar, NULL, &_mousebar_rect);
	}

  rect.y = EYEBAR_TOP;
  rect.h = 25;

/*  _left_eye_angle=-0.4;*/
  _left_eye_width=0.1;

  rect.x = EYEBAR_CENTER - EYEBAR_SCALE * (_left_eye_angle + _left_eye_width/2);
  rect.x = LIMIT(EYEBAR_LEFT, rect.x, EYEBAR_RIGHT);
  rect.w =  EYEBAR_SCALE * _left_eye_width;

  /*printf("left eye %d<=x: %d<=%d, w:%d\n", EYEBAR_LEFT,rect.x,EYEBAR_RIGHT, rect.w);*/
	video_fill(video_map_rgb(255, 0, 255), (Uint8) 255, &rect);

/*  _right_eye_angle=0.4; */
  _right_eye_width=0.1;

  rect.x = EYEBAR_CENTER - EYEBAR_SCALE * (_right_eye_angle + _right_eye_width/2);
  rect.x = LIMIT(EYEBAR_LEFT, rect.x, EYEBAR_RIGHT);
  rect.w = EYEBAR_SCALE * _right_eye_width;
	video_fill(video_map_rgb(255, 255, 0), (Uint8) 255, &rect);

  _eyebar_rect.x = SCOREBOARD_POSITION;
  _eyebar_rect.y = 125;
	video_set_alpha(_sdl_eyebar, (Uint8) 255);
	video_blit(_sdl_eyebar, NULL, &_eyebar_rect);

}

void scoreboard_update(Uint32 time) {
	switch (_state) {
	case SCOREBOARD_STATE_IDLE:
		break;
	case SCOREBOARD_STATE_ADD_1:
		_score[0]++;
		_p1_scored = 1;
		_state = SCOREBOARD_STATE_IDLE;
		break;
	case SCOREBOARD_STATE_ADD_2:
		_score[1]++;
		_p2_scored = 1;
		_state = SCOREBOARD_STATE_IDLE;
		break;
	default:
		break;
	}
	if (_mousebar_alpha > 0) {
		_counter += time;
		if ((MOUSEBAR_TIMEOUT > 0) && (_counter > MOUSEBAR_TIMEOUT)) {
			_mousebar_alpha -= time * MOUSEBAR_FADOUT_SPEED;
			if (_mousebar_alpha < 0) {
				_mousebar_alpha = 0;
				video_erase(&_mousebar_rect);
			}
		}
	}
}

void scoreboard_set_mousebar(Uint8 speed) {
	_mouse_speed = speed;
	_mousebar_alpha = 1.0;
	_counter = 0;
}
