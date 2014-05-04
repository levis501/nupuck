/* player.c - Copyright (C) 2001-2002 Jacob Kroon, see COPYING for details */

#include <stdlib.h>
#include <math.h>
#include <SDL_mouse.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/uio.h>
#include <unistd.h>


#include "video.h"
#include "tuxpuck.h"
#include "toolbox.h"

/* defines */
#define MIN_MOUSE_SPEED		((float)0.05)
#define MAX_MOUSE_SPEED		((float)0.50)

#define EYE_PUCK_WIDTH ((Uint16) 1)
/*
#define LEFT_EYE_X ((float)-10)
#define LEFT_EYE_Z ((float)-52)
#define RIGHT_EYE_X ((float)10)
#define RIGHT_EYE_Z ((float)-52)
*/
#define EYE_SPACING (5.0f)

extern Settings *_settings;
extern int _prediction_pipe;
extern Uint8 _state;
extern Uint8 _turn;

/* structs */
/* moved to toolbox.h
struct _HumanPlayer {
  Uint8 points;
  char *name;
  Pad *pad;
  float speed;
  int serveState;
};
*/

/* eye tracking for nupic */
float _left_eye_angle;
float _right_eye_angle;
float _left_eye_width;
float _right_eye_width;


/* functions */
static float _mod(float a, float b)
{
  return (a - floor(a / b) * b);
}

/* It is not absolutely exactly (+- 1 bounce difference),
 * but I think it needn't be.
 * -- Ernst Moritz Hahn
 *
 * This function is temporarly commented since it isnt used (yet),
 * and thus gives a warning when compiling.
 * -- Jacob Kroon

static float _calc_bounces(float z, float dx, float dz)
{
  float n, x;

  if(dz==0) return 0;
  n = (35 - z) / dz;
  x = n * dx;
  return fabs(x / 40);
}
*/

static float _calc_puck_x(float x, float z, float dx, float dz, float ax)
{
  float board_w;
  float raw_x, rx, g, h, n;

  board_w = 40 - PUCK_W;
  n = (ax - z) / dz;
  raw_x = n * dx + x + board_w / 2;
  rx = _mod(raw_x, board_w);
  g = raw_x / board_w;
  h = fabs(_mod(g, 2));
  if ((h > 1))
    rx = board_w - rx;
  rx = rx - board_w / 2;
  return rx;
}

static void _dumb_really_idle(AIPlayer * player, Uint32 time)
{
  static float counter = M_PI_2;
  float dx, dz, puck_x, puck_z, pad_x, pad_z;

  counter += 0.005 * time;
  entity_get_position(player->pad, &pad_x, &pad_z);
  entity_get_position((Entity *) player->puck, &puck_x, &puck_z);
  dx = (sin(counter / 2.0) * 0.03 + (puck_x - pad_x) * 0.005) * player->speed;
  dz = (sin(counter / 4.0) * 0.01 + (35 - pad_z) * 0.005) * player->speed;
  entity_set_velocity(player->pad, dx, dz);
}

static void _smart_really_idle(AIPlayer * player, Uint32 time)
{
  static float counter = M_PI_2;
  float dx, dz, puck_x, puck_z, puck_dx, puck_dz, pad_x, pad_z, x;

  counter += 0.005 * time;
  entity_get_position(player->pad, &pad_x, &pad_z);
  entity_get_position((Entity *) player->puck, &puck_x, &puck_z);
  entity_get_velocity((Entity *) player->puck, &puck_dx, &puck_dz);
  if (puck_dz != 0) {
    x = _calc_puck_x(puck_x, puck_z, puck_dx, puck_dz, 35);
    dx =
      (sin(counter / 2.0) * 0.03 + (puck_x - pad_x) * 0.005) * player->speed;
    dz = (sin(counter / 4.0) * 0.01 + (40 - pad_z) * 0.005) * player->speed;
    if (puck_dz > 0)
      dx = ((x - pad_x) * 0.005) * player->speed;
    else
      dx = (0 - pad_x) * 0.005;
    dz = ((35 - pad_z) * 0.005) * player->speed;
    entity_set_velocity(player->pad, dx, dz);
  }
}

HumanPlayer *human_create(Pad * pad, char *name)
{
  HumanPlayer *human = NULL;

  human = malloc(sizeof(HumanPlayer));
  human->name = name;
  human->pad = pad;
  human->points = 0;
  human->speed = (MAX_MOUSE_SPEED + MIN_MOUSE_SPEED) / 2.0;
  human->serveState = HUMAN_SERVE_NONE;
  return human;
}

void human_give_point(HumanPlayer * human)
{
  human->points++;
}

Uint8 human_get_points(HumanPlayer * human)
{
  return human->points;
}

void human_free(HumanPlayer * human)
{
  free(human);
}

void human_set_speed(HumanPlayer * human, Uint8 speed)
{
  human->speed =
    MIN_MOUSE_SPEED + (MAX_MOUSE_SPEED - MIN_MOUSE_SPEED) * speed / 10.0;
}

void calc_eye_angles(HumanPlayer *player) {
  float puck_x;
  float puck_z;
  entity_get_position((Entity *)board_get_puck(), &puck_x, &puck_z);

  float human_x;
  float human_z;
  entity_get_position((Entity *)player->pad, &human_x, &human_z);

  float left_eye_x = human_x - EYE_SPACING;
  float right_eye_x = human_x + EYE_SPACING;

  _left_eye_angle = atan2(left_eye_x-puck_x, puck_z-human_z);
  _right_eye_angle = atan2(right_eye_x-puck_x, puck_z-human_z);

/*  printf("_left_eye_angle: %f\t_right_eye_angle: %f\n", _left_eye_angle, _right_eye_angle);*/

  float left_dist = DIST(left_eye_x,puck_x, puck_z,human_z);
  _left_eye_width = 0.2f - 0.002f * left_dist;
  float right_dist = DIST(right_eye_x,puck_x, puck_z,human_z);
  _right_eye_width = 0.2f - 0.002f * right_dist;
  /*
  printf("left_dist: %f\tright_dist: %f\n", left_dist, right_dist);
  printf("_left_eye_width: %f\t_right_eye_width: %f\n", _left_eye_width, _right_eye_width);
  */
}


#define SERVE_SPEED 3
void human_update(HumanPlayer * human, Uint32 time)
{
  int dx=0, dy=0;
  int n;
  char buf[256];

  calc_eye_angles(human);
  printf("EYE %f %f %f %f\n", _left_eye_angle, _left_eye_width, _right_eye_angle, _right_eye_width);
  if (_settings->predictions) {
    sprintf(buf,"%f %f %f %f\n", _left_eye_angle, _left_eye_width, _right_eye_angle, _right_eye_width);
    n = write(_prediction_pipe, buf, strlen(buf));
    if (n < 0) {
      printf("ERROR writing to socket %s\n",buf);
      perror("perror ");
      exit(4);
    }
    bzero(buf, 256);
    n = read(_prediction_pipe, buf, 255);
    sscanf(buf,"%f %f %f %f\n", &_left_eye_angle, &_left_eye_width, &_right_eye_angle, &_right_eye_width);
    printf("PRE %f %f %f %f\n", _left_eye_angle, _left_eye_width, _right_eye_angle, _right_eye_width);
  }
  if (_settings->generate || _settings->predictions) {
    if ((_left_eye_angle * _right_eye_angle) > 0) {
      if (_left_eye_angle < 0) {
        dx = 1;
      } else {
        dx = -1;
      }
    }
    float x=0,z,dz;

    entity_get_velocity((Entity *)board_get_puck(),0, &dz);
    /*printf("puck dz = %f\n", dz);*/
    entity_get_position((Entity *)human->pad, &x, &z);
    /*printf("human x,z = %f,%f\n", x,z);*/
    switch (human->serveState) {
      case HUMAN_SERVE_START:
        x=0;
        entity_set_position((Entity *)human->pad,x,z);
        if (dz > 0.001) {
          dy = SERVE_SPEED;
          human->serveState = HUMAN_SERVE_END;
        } else {
          dy = -SERVE_SPEED;
        }
        break ;
      case HUMAN_SERVE_END:
        if (z < (-35+SERVE_SPEED)) {
          dy = 0;
          human->serveState = HUMAN_SERVE_NONE;
          z = -36;
          entity_set_position((Entity *)human->pad,x,z);
        } else {
          dy = 5;
        }
        break;
      case HUMAN_SERVE_NONE:
      default:
        dy = 0;
    }
  } else {
    SDL_GetRelativeMouseState(&dx, &dy);
  }
  /*printf("dx:%d dy:%d\n",dx,dy);*/
  if (time != 0)
    entity_set_velocity(human->pad, (float) dx / time * human->speed,
			(float) -dy / time * human->speed);
  else
    entity_set_velocity(human->pad, 0, 0);
}

void aiplayer_blit(AIPlayer * player)
{
  video_blit(player->sdl_image, NULL, &player->rect);
}

void aiplayer_erase(AIPlayer * player)
{
  video_erase(&player->rect);
}

void aiplayer_set_alpha(AIPlayer * player, Uint8 alpha)
{
  video_set_alpha(player->sdl_image, alpha);
}

void aiplayer_update(AIPlayer * player, Uint32 time)
{
  switch (player->state) {
  case PLAYER_STATE_IDLE:
    player->strategy.idle(player, time);
    break;
  case PLAYER_STATE_SERVE:
    printf("PLAYER_STATE_SERVE\n");
    player->strategy.serve(player, time);
    break;
  case PLAYER_STATE_BACKUP:
    player->strategy.backup(player, time);
    break;
  case PLAYER_STATE_AIM:
    player->strategy.aim(player, time);
    break;
  case PLAYER_STATE_HIT:
    player->strategy.hit(player, time);
    break;
  case PLAYER_STATE_WIN_POINT:
  case PLAYER_STATE_LOOSE_POINT:
    _dumb_really_idle(player, time);
    if (player->ready() && board_get_state() == BOARD_STATE_PLAY) {
      if (board_get_turn() == 2)
	player->set_state(player, PLAYER_STATE_SERVE);
      else
	player->set_state(player, PLAYER_STATE_IDLE);
    }
    break;
  default:
    _dumb_really_idle(player, time);
    break;
  }
}

void dumb_idle(AIPlayer * player, Uint32 time)
{
  float puck_z;

  _dumb_really_idle(player, time);
  entity_get_position((Entity *) player->puck, NULL, &puck_z);
  if (puck_z > 20)
    player->set_state(player, PLAYER_STATE_BACKUP);
}

void dumb_serve(AIPlayer * player, Uint32 time)
{
  player->set_state(player, PLAYER_STATE_BACKUP);
}

void dumb_backup(AIPlayer * player, Uint32 time)
{
  float pad_x;

  entity_get_position(player->pad, &pad_x, NULL);
  if (entity_move_towards
      (player->pad, pad_x, 38, 0.03 * player->speed, time) == 0)
    player->set_state(player, PLAYER_STATE_AIM);
}

void dumb_aim(AIPlayer * player, Uint32 time)
{
  float hit_speed, dx, dz, puck_x, puck_z, pad_x, pad_z;

  entity_get_position(player->pad, &pad_x, &pad_z);
  entity_get_position((Entity *) player->puck, &puck_x, &puck_z);
  dx = (puck_x - pad_x);
  dz = (puck_z - pad_z);
  if (dz > -0.4) {
    player->set_state(player, PLAYER_STATE_BACKUP);
    return;
  }
  hit_speed = sqrt(dx * dx + dz * dz);
  dx = dx / hit_speed * player->hit_power * 0.1;
  dz = dz / hit_speed * player->hit_power * 0.1;
  player->set_state(player, PLAYER_STATE_HIT);
  entity_set_velocity(player->pad, dx, dz);
}

void dumb_hit(AIPlayer * player, Uint32 time)
{
  float pad_z;

  entity_get_position(player->pad, NULL, &pad_z);
  if (pad_z < 21)
    player->set_state(player, PLAYER_STATE_IDLE);
}

void smart_idle(AIPlayer * player, Uint32 time)
{
  float puck_z;

  _smart_really_idle(player, time);
  entity_get_position((Entity *) player->puck, NULL, &puck_z);
  if (puck_z > 20)
    player->set_state(player, PLAYER_STATE_BACKUP);
}

void smart_backup(AIPlayer * player, Uint32 time)
{
  float pad_x, puck_x, puck_z, puck_dx, puck_dz, x;

  entity_get_position(player->pad, &pad_x, NULL);
  entity_get_position((Entity *) player->puck, &puck_x, &puck_z);
  entity_get_velocity((Entity *) player->puck, &puck_dx, &puck_dz);

  if (fabs(puck_dx) > 0.15) {
    x = _calc_puck_x(puck_x, puck_z, puck_dx, puck_dz, 35);
    entity_move_towards(player->pad, x, 35, 0.03 * player->speed, time);
    player->set_state(player, PLAYER_STATE_BACKUP);
  } else {
    if (entity_move_towards
	(player->pad, pad_x, 38, 0.03 * player->speed, time) == 0) {
      player->set_state(player, PLAYER_STATE_AIM);
    }
  }
}
