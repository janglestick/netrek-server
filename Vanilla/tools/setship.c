#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <ctype.h>
#include "defs.h"
#include "struct.h"
#include "data.h"
#include "proto.h"
#include "util.h"

static void usage(void)
{
  fprintf(stderr, "\
Usage: setship SLOT get\n\
       setship SLOT COMMAND [COMMAND ...]\n\
\n\
");
}

/* display everything known about a planet in command line format */
void get(char *us, char *name, struct player *pl)
{
  printf("%s %s", us, name);
  printf(" speed %d", pl->p_speed);
  printf(" dir %d", pl->p_dir);
  printf(" position %d %d", pl->p_x, pl->p_y);
  printf("\n");
}

struct torp *k = NULL;
int t_attribute = TOWNERSAFE | TDETTEAMSAFE;

int main(int argc, char **argv)
{
    int i, player, verbose = 0;
    struct player *me;

    openmem(0);
    if (argc == 1) { usage(); return 1; }

    i = 1;

 state_0:
    /* check for ship identifier */

  player=atoi(argv[1]);
  if ((player == 0) && (*argv[1] != '0')) {
    char c = *argv[1];
    if (c >= 'a' && c <= 'z')
      player = c - 'a' + 10;
    else {
      fprintf(stderr, "unknown slot\n");
    exit(1);
    }
  }
  if (player >= MAXPLAYER) {
    printf("MAXPLAYER is set to %d, and you want %d?\n", 
	   MAXPLAYER, player);
    exit(1);
  }
  me = &players[player];

 state_1:
    if (++i == argc) return 0;
    
    if (!strcmp(argv[i], "get")) {
      get(argv[0], argv[1], me);
      goto state_1;
    }

    if (!strcmp(argv[i], "verbose")) {
      verbose++;
      goto state_1;
    }

    if (!strcmp(argv[i], "position")) {
      int p_x, p_y;
      if (++i == argc) return 0;
      p_x = atoi(argv[i]);
      if (++i == argc) return 0;
      p_y = atoi(argv[i]);
      p_x_y_set(&players[player], p_x, p_y);
      goto state_1;
    }

    if (!strcmp(argv[i], "dir")) {
      if (++i == argc) return 0;
      players[player].p_dir = atoi(argv[i]);
      me->p_desdir = atoi(argv[i]);
      bay_release(me);
      me->p_flags &= ~(PFBOMB | PFORBIT | PFBEAMUP | PFBEAMDOWN);
      goto state_1;
    }

    if (!strcmp(argv[i], "speed")) {
      if (++i == argc) return 0;
      me->p_desspeed = atoi(argv[i]);
      me->p_flags &= ~(PFREPAIR | PFBOMB | PFORBIT | PFBEAMUP | PFBEAMDOWN);
      me->p_flags &= ~(PFPLOCK | PFPLLOCK);
      goto state_1;
    }

    if (!strcmp(argv[i], "wobble")) {
      t_attribute |= TWOBBLE;
      goto state_1;
    }

    if (!strcmp(argv[i], "no-wobble")) {
      t_attribute &= ~TWOBBLE;
      goto state_1;
    }

    if (!strcmp(argv[i], "fire-test-torpedo")) {
      if (++i == argc) return 0;
      struct ship *myship = &me->p_ship;
      for (k = firstTorpOf(me); k <= lastTorpOf(me); k++)
        if (k->t_status == TFREE)
          break;
      me->p_ntorp++;
      k->t_status = TMOVE;
      k->t_type = TPLASMA;
      k->t_attribute = t_attribute;
      k->t_owner = me->p_no;
      k->t_x = me->p_x;
      k->t_y = me->p_y;
      k->t_turns  = myship->s_torpturns;
      k->t_damage = 0;
      k->t_gspeed = myship->s_torpspeed * WARP1;
      k->t_fuse   = 500;
      k->t_dir    = atoi(argv[i]);
      k->t_war    = me->p_war;
      k->t_team   = me->p_team;
      k->t_whodet = NODET;
      goto state_1;
    }

    if (!strcmp(argv[i], "show-test-torpedo-position")) {
      if (k != NULL) {
        printf("torp %d x %d y %d\n", k->t_dir, k->t_x, k->t_y);
      }
      goto state_1;
    }

    if (!strcmp(argv[i], "destroy-test-torpedo")) {
      if (k != NULL) {
        k->t_status = TOFF;
      }
      goto state_1;
    }

    if (!strcmp(argv[i], "monitor-coordinates")) {
      for (;;) {
	printf("p_x %X p_y %X p_x_internal %X p_y_internal %X\n", me->p_x, me->p_y, me->p_x_internal, me->p_y_internal);
	usleep(20000);
      }
      goto state_1;
    }

    if (!strcmp(argv[i], "monitor-docking")) {
      for (;;) {
	printf("p_flags & PFDOCK %X p_dock_with %X p_dock_bay %X\n", me->p_flags & PFDOCK, me->p_dock_with, me->p_dock_bay);
	usleep(20000);
      }
      goto state_1;
    }

    if (!strcmp(argv[i], "sleep")) {
      if (++i == argc) return 0;
      sleep(atoi(argv[i]));
      goto state_1;
    }

    goto state_0;
}
