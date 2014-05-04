/* tuxpuck.c - Copyright (C) 2001-2002 Jacob Kroon, see COPYING for details */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/uio.h>
#include <unistd.h>


#include <SDL.h>
#include "video.h"
#include "audio.h"
#include "tuxpuck.h"

#include "toolbox.h"

/* defines */
#ifdef windows
#define SETTINGSFILE "tuxpuck.ini"
#else
#define SETTINGSFILE _settings_file
#endif

Settings *_settings = NULL;
int _prediction_pipe = 0;

void startPipe() {
  struct sockaddr_un name;
  int sock;
  size_t size;

  sock = socket(PF_LOCAL, SOCK_STREAM, 0);
  if (sock < 0) {
    printf("ERROR creating socket\n");
    exit(2);
  }
  name.sun_family = AF_LOCAL;
  strncpy(name.sun_path, "/tmp/nupuck.socket", sizeof(name.sun_path));
  name.sun_path[sizeof(name.sun_path)-1]='\0';
  size = (offsetof (struct sockaddr_un, sun_path) + strlen (name.sun_path));
  if (connect (sock, (struct sockaddr *) &name, size) < 0) {
    printf("ERROR connecting to socket %s\n",name.sun_path);
    perror("perror: ");
    exit(3);
  }

  _prediction_pipe = sock;
}

/* statics */
#ifndef windows
static char _settings_file[200];
#endif

/* Returns a pointer to the selected AI opponent.
 *
 * @param opponent
 */
AIPlayer* select_opponent(Uint8 opponent) {

	AIPlayer* p2 = NULL;

	switch (opponent) {
	case 1:
		p2 = sam_create(board_get_pad(2), board_get_puck());
		break;
	case 2:
		p2 = tin_create(board_get_pad(2), board_get_puck());
		break;
	case 3:
		p2 = coler_create(board_get_pad(2), board_get_puck());
		break;
	case 4:
		p2 = lex_create(board_get_pad(2), board_get_puck());
		break;
	case 5:
		p2 = smasher_create(board_get_pad(2), board_get_puck());
		break;
	case 6:
		p2 = morth_create(board_get_pad(2), board_get_puck());
		break;
	case 7:
		p2 = arcana_create(board_get_pad(2), board_get_puck());
		break;
	case 8:
		p2 = buff_create(board_get_pad(2), board_get_puck());
		break;
	case 9:
		p2 = a1d2_create(board_get_pad(2), board_get_puck());
		break;
	case 10:
		p2 = tux_create(board_get_pad(2), board_get_puck());
		break;
	}

	return p2;
}

HumanPlayer *p1 = NULL;

/* functions */
static int _play_match(Uint8 opponent) {
	int next_opponent;
	SDL_Event event;
	Uint8 loop = 1, scorer = 0;
	Uint32 elapsed_time = 0;
	char buffer[50];

	/* Human player.*/
	p1 = NULL;

	/* Computer opponent. */
	AIPlayer *p2 = NULL;

	Timer *timer = NULL;
	float alpha = 0.0;

	/* Exit menu. */
	Menu *exit_menu;

	/* "Play-again" menu.*/
	Menu *again_menu;

	memset(buffer, 0, 50);
	board_init();
	scoreboard_init();
	video_save();
	p1 = human_create(board_get_pad(1), "Human");

	p2 = select_opponent(opponent);
	SDL_ShowCursor(SDL_DISABLE);

	exit_menu = menu_create(2);
	menu_add_field(exit_menu, 0, 1, "Continue");
	menu_add_field(exit_menu, 1, 1, "Surrender");
	again_menu = menu_create(2);
	menu_add_field(again_menu, 0, 1, "Play Again");
	menu_add_field(again_menu, 1, 1, "Main Menu");

	timer = timer_create();
	timer_reset(timer);
	while (loop) {
		while (SDL_PollEvent(&event))
			if (event.type == SDL_MOUSEBUTTONDOWN) {
				loop = 0;
				alpha = 1.0;
			}
		SDL_Delay(SLEEP);
		timer_update(timer);
		timer_reset(timer);
		elapsed_time = timer_elapsed(timer);
		alpha += elapsed_time * 0.001;
		if (alpha > 1.0) {
			loop = 0;
			alpha = 1.0;
		}
		board_clean_up();
		scoreboard_erase();
		aiplayer_erase(p2);
		aiplayer_set_alpha(p2, (Uint8) (alpha * 255));
		scoreboard_set_alpha((Uint8) (alpha * 255));
		entity_set_alpha((Entity *) board_get_puck(), (Uint8) (alpha * 255));
		entity_set_alpha(board_get_pad(1), (Uint8) (alpha * 255.0 / 2.0));
		entity_set_alpha(board_get_pad(2), (Uint8) (alpha * 255.0 / 2.0));
		aiplayer_blit(p2);
		board_reblit();
		scoreboard_blit();
		video_update();
	}
	loop = 1;
	board_clean_up();
	aiplayer_blit(p2);
	video_save();
	board_reblit();
	video_update();
	SDL_PumpEvents();
	SDL_GetRelativeMouseState(NULL, NULL);
#ifndef _DEBUG
	SDL_WM_GrabInput(SDL_GRAB_ON);
#endif
	human_set_speed(p1, _settings->mouse_speed);
	timer_reset(timer);
	while (loop) {
		while (SDL_PollEvent(&event))
			switch (event.type) {
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:

					SDL_ShowCursor(SDL_ENABLE);
					if (menu_get_selected(exit_menu) == 1)
						loop = 0;
					timer_reset(timer);
					break;
				case SDLK_F1:
					_settings->sound = !_settings->sound;
					audio_set_mute(!_settings->sound);
					break;
				case SDLK_F5:
					if (_settings->mouse_speed > 1)
						_settings->mouse_speed--;
					human_set_speed(p1, _settings->mouse_speed);
					scoreboard_set_mousebar(_settings->mouse_speed);
					break;
				case SDLK_F6:
					if (_settings->mouse_speed < 10)
						_settings->mouse_speed++;
					human_set_speed(p1, _settings->mouse_speed);
					scoreboard_set_mousebar(_settings->mouse_speed);
					break;
				case SDLK_f:
					_settings->fullscreen = !_settings->fullscreen;
					video_toggle_fullscreen();
					break;
				default:
					break;
				}
				break;
			case SDL_QUIT:
				loop = 0;
				break;
			}
		SDL_Delay(SLEEP);
		timer_update(timer);
		timer_reset(timer);
		elapsed_time = timer_elapsed(timer);
		human_update(p1, elapsed_time);
		aiplayer_update(p2, elapsed_time);
		scoreboard_update(elapsed_time);
		if ((scorer = board_update(elapsed_time)) != 0) {
			scoreboard_add_point(scorer);
			if (scorer == 1) {
				human_give_point(p1);
				p2->set_state(p2, PLAYER_STATE_LOOSE_POINT);
			} else {
				p2->points++;
				p2->set_state(p2, PLAYER_STATE_WIN_POINT);
			}
			if (human_get_points(p1) >= 15 || p2->points >= 15) {
				if (human_get_points(p1) == 15)
					p2->set_state(p2, PLAYER_STATE_LOOSE_GAME);
				else
					p2->set_state(p2, PLAYER_STATE_WIN_GAME);
			}
		}
		board_clean_up();
		scoreboard_clean_up();
		scoreboard_reblit();
		if (p2->reblit(p2, elapsed_time) == 0)
			loop = 0;
		board_reblit();
		video_update();
	}
#ifndef _DEBUG
	SDL_WM_GrabInput(SDL_GRAB_OFF);
#endif
	menu_free(exit_menu);
	timer_free(timer);
	human_free(p1);
	p2->free(p2);
	board_deinit();
	scoreboard_deinit();
	if (menu_get_selected(again_menu) == 0)
		next_opponent = opponent;
	else
		next_opponent = -1;
	menu_free(again_menu);
	return next_opponent;
}

static void _read_settings(void) {
	FILE *file = NULL;
	char buffer[100], buffer2[100];
	Uint32 uint32 = 0;

	if ((file = fopen(SETTINGSFILE, "r")) == NULL)
		return;
	while (fgets(buffer, 100, file) != 0) {
		if (sscanf(buffer, "%s %d\n", buffer2, &uint32) != 2) {
			fclose(file);
			return;
		} else if (strcmp(buffer2, "SOUND") == 0)
			_settings->sound = (Uint8) uint32;
		else if (strcmp(buffer2, "FULLSCREEN") == 0)
			_settings->fullscreen = (Uint8) uint32;
		else if (strcmp(buffer2, "MOUSESPEED") == 0)
			_settings->mouse_speed = (Uint8) uint32;
	}
	fclose(file);
}

static void _save_settings(void) {
	FILE *file = NULL;

	if ((file = fopen(SETTINGSFILE, "w")) == NULL)
		return;
	fprintf(file, "SOUND %d\n", _settings->sound);
	fprintf(file, "FULLSCREEN %d\n", _settings->fullscreen);
	fprintf(file, "MOUSESPEED %d\n", _settings->mouse_speed);
	fclose(file);
}

static void _tuxpuck_init(void) {
#ifndef windows
	char *homeDir = NULL;
#endif
	srand(time(NULL));
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	audio_init();
	video_init();

	video_save();
	_settings = (Settings *) malloc(sizeof(Settings));
	memset(_settings, 0, sizeof(Settings));
	_settings->sound = 1;
	_settings->fullscreen = 0;
	_settings->mouse_speed = 5;
#ifndef windows
	homeDir = getenv("HOME");
	sprintf(_settings_file, "%s/.tuxpuckrc", homeDir);
#endif
	_read_settings();
	audio_set_mute(!_settings->sound);
	if (_settings->fullscreen)
		video_toggle_fullscreen();

	run_intro();
	video_save();
}

static void _tuxpuck_deinit(void) {
	audio_deinit();
	video_deinit();
	SDL_Quit();
	_save_settings();
	free(_settings);
}

Menu* create_game_menu() {
	Menu* main_menu = menu_create(5);
	menu_add_field(main_menu, 0, 1, "Play");
	menu_add_field(main_menu, 1, 1, "Game Options");
	menu_add_field(main_menu, 2, 1, "Generate Data");
	menu_add_field(main_menu, 3, 1, "Predictive Play");
	menu_add_field(main_menu, 4, 1, "Exit");
	return main_menu;
}

Menu* create_option_menu() {
	Menu* option_menu = menu_create(3);
	menu_add_field(option_menu, 0, 1, "Fullscreen");
	menu_add_field(option_menu, 1, 1, "Windowed");
	menu_add_field(option_menu, 2, 1, "Back");
	return option_menu;
}

Menu* create_opponent_menu() {
	Menu* op_menu = menu_create(12);
	menu_add_field(op_menu, 0, 0, "Opponent");
	menu_add_field(op_menu, 1, 1, "Sam");
	menu_add_field(op_menu, 2, 1, "Tin");
	menu_add_field(op_menu, 3, 1, "Coler");
	menu_add_field(op_menu, 4, 1, "Lex");
	menu_add_field(op_menu, 5, 1, "Smasher");
	menu_add_field(op_menu, 6, 1, "Morth");
	menu_add_field(op_menu, 7, 1, "Arcana");
	menu_add_field(op_menu, 8, 1, "Buff");
	menu_add_field(op_menu, 9, 1, "A1d2");
	menu_add_field(op_menu, 10, 1, "Tux");
	menu_add_field(op_menu, 11, 1, "Back");

	return op_menu;

}

/* main method*/
int main(int argc, char *argv[]) {

	printf("Here we go!\n");

	int next_opponent, option;
	Menu *main_menu, *opponent_menu, *option_menu;

	_tuxpuck_init();
	main_menu = create_game_menu();
	opponent_menu = create_opponent_menu();
	option_menu = create_option_menu();
	int loop = 1;
	while (loop) {
		int selection = menu_get_selected(main_menu);

		printf("selection is %i\n", selection);
		switch (selection) {

		/* Escape was pressed.*/
		case -1:

			loop = 0;
			break;
		case 0:
			next_opponent = menu_get_selected(opponent_menu);
			while (next_opponent != -1)
				next_opponent = _play_match(next_opponent);
			break;
		case 1:
			printf("To be implemented\n");
			int option_loop = 1;
			while (option_loop) {
				option = menu_get_selected(option_menu);
				printf("option %i\n", option);

				if (option == 0) {
					Uint32 uint32 = 1;
					_settings->fullscreen = (Uint8) uint32;
				}
				if (option == 1) {
					Uint32 uint32 = 0;
					_settings->fullscreen = (Uint8) uint32;
				}
				if (option == 2) {
					option_loop = 0;

				}
			}

			break;
    case 2: /* Generate data */
      printf("Generate data\n");
      _settings->generate = 1;
			next_opponent = menu_get_selected(opponent_menu);
			while (next_opponent != -1)
				next_opponent = _play_match(next_opponent);
			break;
    case 3: /* Play with predictions */
      printf("Play with predictions\n");
      _settings->predictions = 1;
      if (_prediction_pipe == 0) {
        startPipe();
      }
      if (_prediction_pipe == 0) {
        printf("ERROR opening pipe\n");
        exit(1);
      }
			next_opponent = menu_get_selected(opponent_menu);
			while (next_opponent != -1)
				next_opponent = _play_match(next_opponent);
			break;
		case 4:
			loop = 0;
			break;
		default:
			printf("This case should not exist\n");
			break;
		}
	}
	menu_free(opponent_menu);
	menu_free(option_menu);
	menu_free(main_menu);
	_tuxpuck_deinit();
	return 0;
}
