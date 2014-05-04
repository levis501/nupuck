/**
 * toolbox for adapting/hacking tuxpuck to nupuck
 */

#include <time.h>

#define MAX(a,b) ((a >= b) ? (a) : (b))
#define MIN(a,b) ((a <= b) ? (a) : (b))
#define LIMIT(lo,x,hi) (MAX(lo,MIN(x,hi)))

#define SQ(x) ((x)*(x))
#define DIST(x0,x1,z0,z1) (sqrt(SQ(x0-x1) + SQ(z0-z1)))

#define HUMAN_SERVE_NONE 0
#define HUMAN_SERVE_START 1
#define HUMAN_SERVE_END 2

struct _HumanPlayer {
  Uint8 points;
  char *name;
  Pad *pad;
  float speed;
  int serveState;
};


AIPlayer *sam_create(Pad * pad, Puck * puck);
AIPlayer *tin_create(Pad * pad, Puck * puck);
AIPlayer *coler_create(Pad * pad, Puck * puck);
AIPlayer *lex_create(Pad * pad, Puck * puck);
AIPlayer *smasher_create(Pad * pad, Puck * puck);
AIPlayer *morth_create(Pad * pad, Puck * puck);
AIPlayer *arcana_create(Pad * pad, Puck * puck);
AIPlayer *buff_create(Pad * pad, Puck * puck);
AIPlayer *a1d2_create(Pad * pad, Puck * puck);
AIPlayer *tux_create(Pad * pad, Puck * puck);
void run_intro(void);
