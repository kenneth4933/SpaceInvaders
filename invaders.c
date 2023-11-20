#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <time.h>
#include <unistd.h>
#include "invaderstructs.h"

#define MAX_BOMBS 1000

void menu(struct options *settings);
void gameover(int win);

int main() {
   struct player tank;
   struct alien aliens[30];
   struct shoot shot[10];
   struct bomb bomb[MAX_BOMBS];
   struct options settings;
   unsigned int input, loops=0, i=0, j=0, currentshots=0, currentbombs=0, currentaliens=30;
   int random=0, score=0, win=-1;
   char tellscore[30];
   
   initscr();
   clear();
   noecho();
   cbreak();
   nodelay(stdscr,1);
   keypad(stdscr,1);
   srand(time(NULL)); 

   /* Set default settings */
   settings.overall = 9000;
   settings.alien = 25;
   settings.shots = 2;
   settings.bombs = 10;
   settings.bombchance = 5;

   /* Set tank settings */
    tank.r = LINES - 3;
       tank.c = COLS / 2;
       tank.ch[0] =' ';
       tank.ch[1] ='_';
       tank.ch[2] ='|';
       tank.ch[3] ='|';
       tank.ch[4] ='_';
       tank.ch[5] =' ';
       tank.a = LINES - 2;
       tank.b = COLS / 2;
       tank.sh[0] =' ';
       tank.sh[1] ='[';
       tank.sh[2] ='*';
       tank.sh[3] ='*';
       tank.sh[4] =']';
       tank.sh[5] =' ';

   /* Set aliens settings */
   for (i=0; i<10; ++i) {
      aliens[i].r = 1;
      aliens[i].c = i*3;
      aliens[i].pr = 0;
      aliens[i].pc = 0;
      aliens[i].ch = 'V';
      aliens[i].alive = 1;
      aliens[i].direction = 'r';
   }
   for (i=10; i<20; ++i) {
      aliens[i].r = 2;
      aliens[i].c = (i-10)*3;
      aliens[i].pr = 0;
      aliens[i].pc = 0;
      aliens[i].ch = 'T';
      aliens[i].alive = 1;
      aliens[i].direction = 'r';
   }
   for (i=20; i<30; ++i) {
      aliens[i].r = 3;
      aliens[i].c = (i-20)*3;
      aliens[i].pr = 0;
      aliens[i].pc = 0;
      aliens[i].ch = 'V';
      aliens[i].alive = 1;
      aliens[i].direction = 'r';
   }
   
   /* Set shot settings */
   for (i=0; i<10; ++i) {
      shot[i].active = 0;
      shot[i].ch = '*';
   }
   
   /* Set bomb settings */
   for (i=0; i<MAX_BOMBS; ++i) {
      bomb[i].active = 0;
      bomb[i].ch = '.';
      bomb[i].loop = 0;
   }
   
   /* Display game title,score header,options */
   move(0,(COLS/2)-9);
   addstr("---SPACE INVADERS---");
   move (0,1);
   addstr("SCORE: ");
   move(0,COLS-19);
   addstr("m = MENU  q = QUIT");
    
   while(1) {
      /* Show score */
      sprintf(tellscore, "%d", score);
      move(0,8);
      addstr(tellscore);
      
      /* Move tank */
       move(tank.r,tank.c);
       addch(' ');
       addch(tank.ch[0]);
       addch(tank.ch[1]);
       addch(tank.ch[2]);
       addch(tank.ch[3]);
       addch(tank.ch[4]);
       addch(tank.ch[5]);
       addch(' ');
       move(tank.a,tank.b);
       addch(' ');
       addch(tank.sh[0]);
       addch(tank.sh[1]);
       addch(tank.sh[2]);
       addch(tank.sh[3]);
       addch(tank.sh[4]);
       addch(tank.sh[5]);
       addch(' ');
      
      /* Move bombs */
      if (loops % settings.bombs == 0)
      for (i=0; i<MAX_BOMBS; ++i) {
         if (bomb[i].active == 1) {
            if (bomb[i].r < LINES) {
               if (bomb[i].loop != 0) {
                  move(bomb[i].r-1,bomb[i].c);
                  addch(' ');
               }
               else
                  ++bomb[i].loop;
               
               move(bomb[i].r,bomb[i].c);
               addch(bomb[i].ch);
               
               ++bomb[i].r;
            }
            else {
               bomb[i].active = 0;
               bomb[i].loop = 0;
               --currentbombs;
               move(bomb[i].r - 1,bomb[i].c);
               addch(' ');
            }
         }
      }
      
      /* Move shots */
      if (loops % settings.shots == 0)
      for (i=0; i<10; ++i) {
         if (shot[i].active == 1) {
            if (shot[i].r > 0) {
               if (shot[i].r < LINES - 2) {
                  move(shot[i].r + 1,shot[i].c);
                  addch(' ');
               }
               
               for (j=0; j<30; ++j) {
                  if (aliens[j].alive == 1 && aliens[j].r == shot[i].r && aliens[j].pc == shot[i].c) {
                     score += 20;
                     aliens[j].alive = 0;
                     shot[i].active = 0;
                     --currentshots;
                     --currentaliens;
                     move(aliens[j].pr,aliens[j].pc);
                     addch(' ');
                     break;
                  }
               }
               
               if (shot[i].active == 1) {
                  move(shot[i].r,shot[i].c);
                  addch(shot[i].ch);
                  
                  --shot[i].r;
               }
            }
            else {
               shot[i].active = 0;
               --currentshots;
               move(shot[i].r + 1,shot[i].c);
               addch(' ');
            }
         }
      }     
      
      /* Move aliens */
      if (loops % settings.alien == 0)
      for (i=0; i<30; ++i) {
         if (aliens[i].alive == 1) {
            move(aliens[i].pr,aliens[i].pc);
            addch(' ');
            
            move(aliens[i].r,aliens[i].c);
            addch(aliens[i].ch);

            
            aliens[i].pr = aliens[i].r;
            aliens[i].pc = aliens[i].c;
            
            /* Check if alien should drop bomb */
            random = 1+(rand()%100);
            if ((settings.bombchance - random) >= 0 && currentbombs < MAX_BOMBS) {
               for (j=0; j<MAX_BOMBS; ++j) {
                  if (bomb[j].active == 0) {
                     bomb[j].active = 1;
                     ++currentbombs;
                     bomb[j].r = aliens[i].r + 1;
                     bomb[j].c = aliens[i].c;
                     break;
                  }
               }
            }
            
            /* Set alien's next position */
            if (aliens[i].direction == 'l')
               --aliens[i].c;
            else if (aliens[i].direction == 'r')
               ++aliens[i].c;
               
            /* Check alien's next positions */
            if (aliens[i].c == COLS - 2) {
               ++aliens[i].r;
               aliens[i].direction = 'l';
            }
            else if (aliens[i].c == 0) {
               ++aliens[i].r;
               aliens[i].direction = 'r';
            }
         }
      }
      
      
      /* See if game has been won or lost*/
      if (currentaliens == 0) {
         win = 1;
         break;
      }
      for (i=0; i<30; ++i) {
         if (aliens[i].r == LINES-1) {
            win = 0;
            break;
         }
      }
      for (i=0; i<MAX_BOMBS; ++i) {
         if (bomb[i].r == tank.r && bomb[i].c == tank.c) {
            win = 0;
            break;
         }
      }    
      
      move(0,COLS-1);
      refresh();
      usleep(settings.overall);
      ++loops;
      
      input = getch();
      move(tank.r,tank.c);
      addch(' ');
      move(tank.r,tank.c);
      addch(' ');
      
      /* Check input */
       if (input == 'q'){
         win = 2;
      }else if (input == KEY_LEFT){
         --tank.c;
         --tank.b;
          }else if (input == KEY_RIGHT){
                  ++tank.c;
                  ++tank.b;
              }else if (input == ' ' && currentshots < 3) {
                      for (i=0; i<10; ++i) {
                          if (shot[i].active == 0) {
                              shot[i].active = 1;
                              ++currentshots;
                              --score;
                              shot[i].r = LINES - 2;
                              shot[i].c = tank.c;
                              break;
              }
          }
      }
       else if (input == 'm'){
         menu(&settings); 
       }
    if (win != -1)
        break;
    
      
      /* Check for valid tank position */
       if (tank.c > COLS-2){
        tank.c = COLS - 2;
        tank.b = COLS - 2;
       }else if (tank.c < 0){
       tank.c = 0;
       tank.b = 0; }
   }
   gameover(win);
   endwin();
    return 0;
}

/* This function handles the menu options available to the user */
void menu(struct options *settings) {
   char option, buf[30];
   int new;

   clear();
   echo();
   nocbreak();
   nodelay(stdscr,0);

    move(0,0);
       addstr("1. MODIFIER LA VITESSE GLOBALE DU JEU");
       move(1,0);
       addstr("2. CHANGER LA VITESSE DE DEPLACEMENT DES ALIENS");
       move(2,0);
       addstr("3. CHNGER LA VITESSE DE TIR DU TANK");
       move(3,0);
       addstr("4. CHANGER LA VITESSE DE TIR DES ALIENS");
       move(4,0);
       addstr("5. CHANGER LA FREQUENCE DES BOMBE");
       move(5,0);
       addstr("6. RETOURNER AU JEU");
       move(6,0);
       addstr("7. QUITTER LE JEU");
       move(8,0);
       addstr("8. ENTREZ VOTRE OPTION: ");
       refresh();
       
       while(1) {
          move(8,23);
          option = getch();
          move(9,0);
          deleteln();
          move(10,0);
          deleteln();
          move(11,0);
          deleteln();
          
          if (option == '1') {
             sprintf(buf,"VALEUR ACTUELLE: %d\n", settings->overall);
             
             move(9,0);
             addstr(buf);
             move(10,0);
             addstr("ENTREZ UNE NOUVELLE VALEUR: ");
             move(10,27);
             refresh();
             getch();
             getstr(buf);
             
             new = atoi(buf);
             
             /* Check for valid new value */
             if (new < 0) {
                move(11,0);
                addstr("ERREUR: VALEUR INVALIDE");
             }
             else {
                settings->overall = new;
             }
          }
          else if (option == '2') {
             sprintf(buf,"VALEUR ACTUELLE: %d\n", settings->alien);
             
             move(9,0);
             addstr(buf);
             move(10,0);
             addstr("ENTREZ UNE NOUVELLE VALEUR: ");
             move(10,27);
             refresh();
             getch();
             getstr(buf);
             
             new = atoi(buf);
             
             /* Check for valid new value */
             if (new <= 0) {
                move(11,0);
                addstr("ERREUR: VALEUR INVALIDE");
             }
             else {
                settings->alien = new;
             }
          }
          else if (option == '3') {
             sprintf(buf,"VALEUR ACTUELLE: %d\n", settings->shots);
             
             move(9,0);
             addstr(buf);
             move(10,0);
             addstr("ENTREZ UNE NOUVELLE VALEUR: ");
             move(10,27);
             refresh();
             getch();
             getstr(buf);
             
             new = atoi(buf);
             
             /* Check for valid new value */
             if (new <= 0) {
                move(11,0);
                addstr("ERREUR: VALEUR INVALIDE");
             }
             else {
                settings->shots = new;
             }
          }
          else if (option == '4') {
             sprintf(buf,"VALEUR ACTUELLE: %d\n", settings->bombs);
             
             move(9,0);
             addstr(buf);
             move(10,0);
             addstr("ENTREZ UNE NOUVELLE VALEUR: ");
             move(10,27);
             refresh();
             getch();
             getstr(buf);
             
             new = atoi(buf);
             
             /* Check for valid value new */
             if (new <= 0) {
                move(11,0);
                addstr("ERREUR: VALEUR INVALIDE");
             }
             else {
                settings->bombs = new;
             }
          }
          else if (option == '5') {
             sprintf(buf,"VALEUR ACTUELLE: %d\n", settings->bombchance);
             
             move(9,0);
             addstr(buf);
             move(10,0);
             addstr("ENTREZ NOUVELLE VALEUR: ");
             move(10,27);
             refresh();
             getch();
             getstr(buf);
             
             new = atoi(buf);
             
             /* Check for valid new value */
             if (new > 100 || new < 0) {
                move(11,0);
                addstr("ERREUR: SELECTION INVALIDE");
             }
             else {
                settings->bombchance = new;
             }
          }
          else if (option == '6') {
             break;
          }
          else if (option == '7') {
             endwin();
             exit(0);
          }
          else {
             move(9,0);
             addstr("ERREUR: SELECTION INVALIDE");
             move(8,19);
             addstr(" ");
             refresh();
          }
       }
       
       clear();
       noecho();
       cbreak();
       nodelay(stdscr,1);
       
       move(0,(COLS/2)-9);
       addstr("---SPACE INVADERS---");
       move (0,1);
       addstr("SCORE: ");
       move(0,COLS-19);
       addstr("m = MENU  q = QUITTER LE JEU");
    }

    /* This function handles displaying the win/lose screen */
    void gameover(int win) {

       nodelay(stdscr, 0);
       
       if (win == 0) {
          clear();
          move((LINES/2)-1,(COLS/2)-5);
          addstr("VOUS AVEZ PERDU");
          move((LINES/2),(COLS/2)-11);
          addstr("APPUYEZ UN BOUTON POUR SORTIR");
          move(0,COLS-1);
          refresh();
          getch();
       }
       
       else if (win == 1) {
          clear();
          move((LINES/2)-1,(COLS/2)-5);
          addstr("VOUS AVEZ TERMINER LA PARTIE");
          move((LINES/2),(COLS/2)-11);
          addstr("APPUYEZ UN BOUTON POUR SORTIR");
          move(0,COLS-1);
          refresh();
          getch();
       }
    }
