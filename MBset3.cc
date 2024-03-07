#include <SDL2/SDL.h>
#include <iostream>
#include <cmath>

const int WIDTH = 800;
const int HEIGHT = 600;

struct Complex {
    double real;
    double imag;
};

int calculateMandelbrot(const Complex& c, int maxIter) {
    Complex z = {0.0, 0.0};

    for (int iterations = 0; iterations < maxIter; ++iterations) {
        double r2 = z.real * z.real;
        double i2 = z.imag * z.imag;

        if (r2 + i2 > 4.0) {
            return iterations;
        }

        double newReal = r2 - i2 + c.real;
        double newImag = 2.0 * z.real * z.imag + c.imag;

        z.real = newReal;
        z.imag = newImag;
    }

    return maxIter;
}

void zoom(Complex &topLeft, Complex &bottomRight, double factor)
{
    double width = bottomRight.real - topLeft.real;
    double height = topLeft.imag - bottomRight.imag;

    topLeft.real += width * (1.0 - factor) / 2.0;
    topLeft.imag -= height * (1.0 - factor) / 2.0;

    bottomRight.real = topLeft.real + width * factor;
    bottomRight.imag = topLeft.imag - height * factor;
}

void drawMandelbrot(SDL_Renderer* renderer, const Complex& topLeft, const Complex& bottomRight, int maxIter) {
    for (int row = 0; row < HEIGHT; ++row) {
        for (int col = 0; col < WIDTH; ++col) {
            double x = topLeft.real + (bottomRight.real - topLeft.real) * col / (WIDTH - 1);
            double y = topLeft.imag - (topLeft.imag - bottomRight.imag) * row / (HEIGHT - 1);

            Complex c;
            c.real = x;
            c.imag = y;
            int iterations = calculateMandelbrot(c, maxIter);

            /// % 200, 25, 25 makes cool read thing
            Uint8 r = static_cast<Uint8>((iterations % 256)); 
            Uint8 g = static_cast<Uint8>((iterations % 256));
            Uint8 b = static_cast<Uint8>((iterations % 256));

            SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE);
            SDL_RenderDrawPoint(renderer, col, row);
        }
    }
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window;
    SDL_Renderer* renderer;

    SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer);
    SDL_SetWindowTitle(window, "Mandelbrot Set");

    Complex topLeft;
    topLeft.real = -2.0;
    topLeft.imag = 1.0;

    Complex bottomRight;
    bottomRight.real = 1.0;
    bottomRight.imag = -1.0;


    SDL_Event event;
    bool quit = false;

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_w:
                        topLeft.imag += 0.1 * (topLeft.imag - bottomRight.imag);
                        bottomRight.imag += 0.1 * (topLeft.imag - bottomRight.imag);
                        break;
                    case SDLK_s:
                        topLeft.imag -= 0.1 * (topLeft.imag - bottomRight.imag);
                        bottomRight.imag -= 0.1 * (topLeft.imag - bottomRight.imag);
                        break;
                    case SDLK_a:
                        topLeft.real -= 0.1 * (bottomRight.real - topLeft.real);
                        bottomRight.real -= 0.1 * (bottomRight.real - topLeft.real);
                        break;
                    case SDLK_d:
                        topLeft.real += 0.1 * (bottomRight.real - topLeft.real);
                        bottomRight.real += 0.1 * (bottomRight.real - topLeft.real);
                        break;
                    case SDLK_UP:
                        zoom(topLeft, bottomRight, 0.1);
                    case SDLK_DOWN:
                        zoom(topLeft, bottomRight, 1.7);
                    default:
                        break;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        // Increase maxIter for higher accuracy, e.g., when zooming in
        int maxIter = 400;
        drawMandelbrot(renderer, topLeft, bottomRight, maxIter);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
