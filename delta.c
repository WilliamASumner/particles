#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

#define NUM_PARTICLES 15
#define MAX_PARTICLES 1000
#define MAX_WIDTH 500
#define MAX_HEIGHT 100
#define DELAY 1000

typedef struct particle {
    double ox;
    double oy;
    double vx;
    double vy;
    double ax;
    double ay;
    double bounce;
    double mass;
} Particle;

double randnum(int min, int max) {
    return (rand()%(max-min + 1) + min);
}

void draw(Particle array[MAX_PARTICLES], int num_particles) {
    clear();
    for (int j = 0; j < num_particles; j++) {
        Particle *nparticle = &array[j];
        mvaddch((int)nparticle->oy,(int)nparticle->ox,'*');
    }
    refresh();
}

void interact(Particle array[MAX_PARTICLES], int height, int width, Particle *nparticle, int num_particles, double delta_t ) {
    for (int k = 0; k < num_particles; k++) {
        if (fabs(array[k].ox - (nparticle->ox+nparticle->vx  *  delta_t)) < delta_t*fabs(nparticle->vx) &&
                fabs(array[k].oy - (nparticle->oy+nparticle->vy * delta_t)) < delta_t*fabs(nparticle->vy))
        {
            double tempx = array[k].vx * array[k].mass / nparticle->mass;
            double tempy = array[k].vy * array[k].mass / nparticle->mass;
            array[k].vx = nparticle->vx * nparticle->mass / array[k].mass;
            array[k].vy = nparticle->vy * nparticle->mass / array[k].mass;
            nparticle->vx = tempx;
            nparticle->vy = tempy;
        }
    }

    nparticle->ox = nparticle->vx * delta_t + nparticle->ox;
    nparticle->oy = nparticle->vy * delta_t + nparticle->oy;

    if (nparticle->ox > width - 1) {
        nparticle->ox = width - 1;
        nparticle->vx *= -nparticle->bounce;
        nparticle->vy *= nparticle->bounce;
    }
    else if (nparticle->ox < 0) {
        nparticle->ox = 0;
        nparticle->vx *= -nparticle->bounce;
        nparticle->vy *= nparticle->bounce;
    }

    if (nparticle->oy > height - 1) {
        nparticle->oy = height - 1;
        nparticle->vy *= -nparticle->bounce;
        nparticle->vx *= nparticle->bounce;
    }
    else if (nparticle->oy < 0) {
        nparticle->oy = 1;
        nparticle->vy *= -nparticle->bounce;
        nparticle->vx *= nparticle->bounce;
    }

    nparticle->vx += nparticle->ax * delta_t;
    nparticle->vy += nparticle->ay * delta_t;
}

int main(void) {
    srand(time(NULL));   // seeding random numbers
    initscr();
    curs_set(0);
    int height = 0;
    int width = 0;
    getmaxyx(stdscr,height,width);
    int max_speed = 5;
    int length_in_secs = 30;
    double delta_t = 0.02; //10/DELAY;
    int iters = length_in_secs*1000000/DELAY;
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
                particle_array[count].ay = 0.5;
                particle_array[count].bounce = (float)randnum(75,99)/100;
                particle_array[count].mass = (float)randnum(100,200)/100;
                count++;
            }
        }
        num_particles = count;
        getch();
        fclose(startScreenPtr);
    } else {
        num_particles = 50;
        for (int i = 0; i < num_particles; i++) {

            particle_array[i].ox = randnum(0,width-1);
            particle_array[i].oy = randnum(0,height-1);

            particle_array[i].vx = randnum(-max_speed,max_speed);
            particle_array[i].vy = randnum(-max_speed,max_speed);
            particle_array[i].ay = 0.5;
            particle_array[i].ax = 0;
            particle_array[i].bounce = randnum(75,99)/100;
            particle_array[i].mass = (float)randnum(100,200)/100;
        }
    }

    int wait = 0;
    clear();
    while (iters > 0) {
        getmaxyx(stdscr,height,width);
        for (int k = 0; k < num_particles; k++) {
            interact(particle_array,height,width,&particle_array[k],num_particles,delta_t);
        }
        draw(particle_array,num_particles);
        usleep(DELAY);
        iters--;
    }
    endwin();
    return 0;
}
