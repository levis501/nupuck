/**
 * toolbox for adapting/hacking tuxpuck to nupuck
 */

#include <time.h>

#define MAX(a,b) ((a >= b) ? (a) : (b))
#define MIN(a,b) ((a <= b) ? (a) : (b))
#define LIMIT(lo,x,hi) (MAX(lo,MIN(x,hi)))

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
