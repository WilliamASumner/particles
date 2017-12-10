#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define NUM_PARTICLES 15
#define MAX_PARTICLES 1000
#define MAX_WIDTH 500
#define MAX_HEIGHT 100
#define DELAY 60000

typedef struct particle {
    double ox;
    double oy;
    double vx;
    double vy;
    double ax;
    double ay;
    double bounce;
} Particle;

double randnum(int min, int max) {
    return (rand()%(max-min + 1) + min);
}

void draw(Particle array[MAX_PARTICLES], int num_particles) {
    clear();
    for (int j = 0; j < num_particles; j++) {
        Particle *nparticle = &array[j];
        mvaddch(nparticle->oy,nparticle->ox,'*');
    }
    refresh();
}

void interact(Particle array[MAX_PARTICLES], int height, int width, Particle *nparticle, int num_particles) {
    int didHit = 0;
    int iter = 0;
    for (int k = 0; k < num_particles; k++) {
        while ((int)array[k].ox == (int)(nparticle->ox+nparticle->vx) && (int)array[k].oy == (int)(nparticle->oy+nparticle->vy) && iter < 500) {
            iter++;
            double tempx = array[k].vx*nparticle->bounce;
            double tempy = array[k].vy*nparticle->bounce;
            array[k].vx = nparticle->vx*array[k].bounce;
            array[k].vy = nparticle->vy*array[k].bounce;
            nparticle->vx = tempx;
            nparticle->vy = tempy;
            didHit = 1;
        }
    }

    nparticle->ox = nparticle->vx + nparticle->ox;
    nparticle->oy = nparticle->vy + nparticle->oy;

    if (nparticle->ox > width - 1) {
        nparticle->ox = width - 1;
        nparticle->vx *= -nparticle->bounce;
        nparticle->vy *= nparticle->bounce;
        if (didHit) {
            nparticle->vy *= 1.05;
        }
    }
    else if (nparticle->ox < 0) {
        nparticle->ox = 0;
        nparticle->vx *= -nparticle->bounce;
        nparticle->vy *= nparticle->bounce;
        if (didHit) {
            nparticle->vy *= 1.05;
        }
    }

    if (nparticle->oy > height - 1) {
        nparticle->oy = height - 1;
        nparticle->vy *= -nparticle->bounce;
        nparticle->vx *= nparticle->bounce;
        if (didHit) {
            nparticle->vx *= 1.05;
        }
    }
    else if (nparticle->oy < 1) {
        nparticle->oy = 1;
        nparticle->vy *= -nparticle->bounce;
        nparticle->vx *= nparticle->bounce;
        if (didHit) {
            nparticle->vx *= 1.05;
        }
    }

    nparticle->vx += nparticle->ax;
    nparticle->vy += nparticle->ay;
}

int main(void) {
    srand(time(NULL));   // seeding random numbers
    initscr();
    curs_set(0);
    int height = 0;
    int width = 0;
    getmaxyx(stdscr,height,width);
    int max_speed = 5;
    int iters = 200;
    int num_particles = 10;
    Particle particle_array[MAX_PARTICLES] = {0};
    FILE *startScreenPtr;

    startScreenPtr = fopen("start-screen.txt","r");
    if (startScreenPtr != NULL) {
        char somechar = '\n';
        int count = 0;
        int x = 0;
        int y = 0;
        while (somechar != EOF) {
            addch(somechar);
            somechar = fgetc(startScreenPtr);
            if (somechar == '*' && count < MAX_PARTICLES) {
                getyx(stdscr,y,x);
                particle_array[count].ox = x;
                particle_array[count].oy = y;
                particle_array[count].vx = (float)randnum(-50,50)/10;
                particle_array[count].vy = 0;
                particle_array[count].ax = 0;
                particle_array[count].ay = 1;
                particle_array[count].bounce = (float)randnum(75,99)/100;

                count++;
            }
        }
        num_particles = count;
        getch();
        fclose(startScreenPtr);
    } else {
        num_particles = MAX_PARTICLES;
        for (int i = 0; i < num_particles; i++) {

            particle_array[i].ox = randnum(0,width-1);
            particle_array[i].oy = randnum(0,height-1);

            particle_array[i].vx = randnum(-max_speed,max_speed);
            particle_array[i].vy = randnum(-max_speed,max_speed);
            particle_array[i].ay = 1;
            particle_array[i].bounce = randnum(65,99)/100;
        }
    }

    int wait = 0;
    clear();
    while (iters > 0) {
        getmaxyx(stdscr,height,width);
        for (int k = 0; k < num_particles; k++) {
            Particle *nparticle = &particle_array[k];
            interact(particle_array,height,width,nparticle,num_particles);
        }
        printf("Particles updated...\n");
        draw(particle_array,num_particles);
        usleep(DELAY);
        iters--;
    }
    endwin();
    return 0;
}
