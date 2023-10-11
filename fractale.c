#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_thread.h> // Pour gérer les threads
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

// Fonction pour jouer de la musique
int PlayMusic(void *data) {
    if (Mix_Init(MIX_INIT_MP3) < 0) {
        printf("SDL_mixer initialization failed: %s\n", Mix_GetError());
        return 1;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer initialization failed: %s\n", Mix_GetError());
        return 1;
    }

    Mix_Music *music = Mix_LoadMUS("audio/ninho.mp3");
    if (music == NULL) {
        printf("Failed to load MP3: %s\n", Mix_GetError());
        return 1;
    }

    Mix_PlayMusic(music, -1);

    // Ne pas mettre de délai ici pour que la musique joue en boucle indéfiniment

    // Mix_HaltMusic(); // Ne pas arrêter la musique ici

    // Mix_FreeMusic(music); // Ne pas libérer la musique ici
    // Mix_CloseAudio(); // Ne pas fermer l'audio ici
    // Mix_Quit(); // Ne pas quitter SDL_mixer ici

    return 0;
}

int CalculateMandelbrotIterations(double cx, double cy, int maxIterations) {
    double zx = 0.0;
    double zy = 0.0;
    int iteration = 0;

    while (zx * zx + zy * zy < 4.0 && iteration < maxIterations) {
        double new_zx = zx * zx - zy * zy + cx;
        double new_zy = 2.0 * zx * zy + cy;
        zx = new_zx;
        zy = new_zy;
        iteration++;
    }

    return iteration;
}
// Code pour afficher la fractale (exemple de fractale Mandelbrot)
int DisplayFractale(void *data) {
   // Initialisation de SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL initialization failed: %s\n", SDL_GetError());
        return 1;
    }

    // Créer la fenêtre pour la fractale
    SDL_Window* window = SDL_CreateWindow("Fractale", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    int isRunning = 1;
    SDL_Event event;

    // Paramètres pour la fractale Mandelbrot
    double zoom = 1.0;
    double offsetX = -0.5;
    double offsetY = 0.0;
    int maxIterations = 100;

    while (isRunning) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    isRunning = 0;
                    break;
                case SDL_MOUSEWHEEL:
                    // Zoom avec la molette de la souris
                    if (event.wheel.y > 0) {
                        zoom *= 1.1;  // Zoom in
                    } else if (event.wheel.y < 0) {
                        zoom /= 1.1;  // Zoom out
                    }
                    break;
                case SDL_KEYDOWN:
                    // Déplacement avec les touches de direction
                    if (event.key.keysym.sym == SDLK_LEFT) {
                        offsetX -= 0.1 / zoom;
                    } else if (event.key.keysym.sym == SDLK_RIGHT) {
                        offsetX += 0.1 / zoom;
                    } else if (event.key.keysym.sym == SDLK_UP) {
                        offsetY -= 0.1 / zoom;
                    } else if (event.key.keysym.sym == SDLK_DOWN) {
                        offsetY += 0.1 / zoom;
                    }
                    break;
            }
        }

        // Code pour afficher la fractale Mandelbrot
        for (int x = 0; x < WINDOW_WIDTH; x++) {
            for (int y = 0; y < WINDOW_HEIGHT; y++) {
                double cx = ((double)x - WINDOW_WIDTH / 2) / (0.5 * zoom * WINDOW_WIDTH) + offsetX;
                double cy = ((double)y - WINDOW_HEIGHT / 2) / (0.5 * zoom * WINDOW_HEIGHT) + offsetY;

                int iterations = CalculateMandelbrotIterations(cx, cy, maxIterations);

                // Utilisez les itérations pour attribuer une couleur
                int r = (iterations % 256);
                int g = (iterations % 128) * 2;
                int b = (iterations % 64) * 4;

                SDL_SetRenderDrawColor(renderer, r, g, b, 255);
                SDL_RenderDrawPoint(renderer, x, y);
            }
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}


int main(int argc, char *argv[]) {
    // Initialiser les threads
    SDL_Thread *musicThread = SDL_CreateThread(PlayMusic, "Music Thread", NULL);
    SDL_Thread *fractaleThread = SDL_CreateThread(DisplayFractale, "Fractale Thread", NULL);

    // Attendre que les threads se terminent
    SDL_WaitThread(musicThread, NULL);
    SDL_WaitThread(fractaleThread, NULL);

    return 0;
}
