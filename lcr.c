// lcr.c
// https://www.wikihow.com/Play-LCR
// $ gcc -Wall -O3 lcr.c -lm -o lcr

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

FILE * ur = NULL;

int grand(void)
{
  unsigned bits;

  if(ur==NULL) ur=fopen("/dev/urandom","rb");
  assert(1 == fread(&bits, sizeof(bits), 1, ur));
  bits &= 0x7FFFFFFF;
  return bits;
}

int grand_0_m(int m) // result <= m
{
  unsigned mask = 0x7FFFFFFF;
  int res;

  while((mask/2) >= m) mask /= 2;
  do {
    res = (unsigned)grand() & mask;
  } while(res > m);
  return res;
}

#define GAMES 10000000

int *chips;
int PLAYERS;

int game() { // player 0 goes first
  for(int i=0; i<PLAYERS; i++) chips[i] = 3;
  int player = 0;
  while(1) {
    int dice = chips[player];
    if(dice > 3) dice = 3;
    for(int roll=0; roll<dice; roll++) {
      int die = grand_0_m(5);
      switch(die){
        case 3 : // L
          chips[player]--;
          if(player) chips[player-1]++;
          else       chips[PLAYERS-1]++;
          break;
        case 4 : // R
          chips[player]--;
          if(player < PLAYERS-1) chips[player+1]++;
          else                   chips[0]++;
          break;
        case 5 : // C
          chips[player]--;
          break;
      }
    }
    int pwc, j, k=0;
    for(pwc=0, j=0; j<PLAYERS; j++) if(chips[j]) {pwc++; k=j;}
    if(pwc==1) return k;
    if(++player >= PLAYERS) player=0;
  }
}

int main(int argc, char *argv[]) {
  assert(argc == 3);
  PLAYERS = atoi(argv[1]);
  int TABS = atoi(argv[2]);
  assert(PLAYERS >= 3);
  chips = malloc(PLAYERS * sizeof(int));
  assert(chips != NULL);
  unsigned *wins = calloc(PLAYERS, sizeof(unsigned));
  assert(wins != NULL);
  for(int g=0; g<GAMES; g++) {
    int winner = game();
    wins[winner]++;
  }
  for(int p=0; p<PLAYERS; p++){
    char * tabs = malloc(TABS + 1);
    assert(tabs);
    memset(tabs,'\t',TABS);
    tabs[TABS] = 0;
    double pos = (double)p / (double)PLAYERS;
    double ei = (double)wins[p] / (double)GAMES * (double)PLAYERS;
    printf("%d\t%f%s%f\n", p, pos, tabs, ei);
  }
  return 0;
}