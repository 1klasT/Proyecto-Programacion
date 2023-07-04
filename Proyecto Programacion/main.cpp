#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <stdio.h>
#include <stdlib.h>

#define cuadro_matriz 40
#define ANCHO_PERSONAJE 42

const int ANCHO_PANTALLA = 1600;
const int ALTURA_PANTALLA = 920;

const int TAMANO_PERSONAJE = 69;
const int VELOCIDAD_PERSONAJE = 5;

const int ANCHO_MAPA = 40;
const int ALTURA_MAPA = 23;

bool key_left = false;
bool key_right = false;
bool key_up = false;
bool key_down = false;

int mapa[ALTURA_MAPA][ANCHO_MAPA];

struct personaje {
     int personaje_x=0, personaje_y=0;

};

struct personaje jugador;

ALLEGRO_BITMAP* imagen_personaje;
ALLEGRO_BITMAP* imagen_bloque;
ALLEGRO_BITMAP* imagen_fondo;
ALLEGRO_BITMAP* imagen_piso;
ALLEGRO_BITMAP* imagen_fuego;

void cargarMapa(const char* nombrearchivo) {
    int x, y;

    FILE* fdata;
    if ((fopen_s(&fdata, nombrearchivo, "r")) != 0) {
        printf("Error al abrir el mapa");

        return;
    }

    for (y = 0; y < ALTURA_MAPA; ++y) {
        for (x = 0; x < ANCHO_MAPA; ++x) {
            fscanf_s(fdata, "%d ", &mapa[y][x]);
            printf("%d ", mapa[y][x]);
        }
        printf("\n");
    }

    fclose(fdata);
}
void cargarPersonaje() {
    imagen_personaje = al_load_bitmap("player.png");
}

void cargarImagenBloque() {
    imagen_bloque = al_load_bitmap("bloque.png");
}

void cargarImagenPiso() {
    imagen_piso = al_load_bitmap("grass.png");
}

void cargarImagenFondo() {
    imagen_fondo = al_load_bitmap("wallpaper.png");
}

void cargarImagenFuego() {
    imagen_fuego = al_load_bitmap("fuego.png");
}

void cargarGravedad() {
    if (mapa[(jugador.personaje_y + TAMANO_PERSONAJE) / cuadro_matriz][jugador.personaje_x / cuadro_matriz] == 0 && mapa[(jugador.personaje_y + TAMANO_PERSONAJE) / cuadro_matriz][(jugador.personaje_x + ANCHO_PERSONAJE) / cuadro_matriz] == 0) {
        jugador.personaje_y+=2;
    }
}

void actualizarPosicionPersonaje() {
    int nueva_x; // Posicion izquierda
    int nueva_y; // Posicion arriba


    if (key_left) {
        nueva_x = jugador.personaje_x - VELOCIDAD_PERSONAJE;
        jugador.personaje_x = nueva_x;
    }
    if (key_right) {
        nueva_x = jugador.personaje_x + VELOCIDAD_PERSONAJE;
        jugador.personaje_x = nueva_x;
    }
    if (key_up) {
        nueva_y = jugador.personaje_y - VELOCIDAD_PERSONAJE;
        jugador.personaje_y = nueva_y;
    }
}

int main() {
    int x, y;
    bool corriendo;
    al_init();
    al_init_primitives_addon();
    al_init_image_addon();
    al_install_keyboard();

    ALLEGRO_DISPLAY* display = al_create_display(ANCHO_PANTALLA, ALTURA_PANTALLA);
    ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_keyboard_event_source());

    cargarMapa("mapa.txt");
    cargarGravedad();
    cargarPersonaje();
    cargarImagenBloque();
    cargarImagenPiso();
    cargarImagenFuego();
    cargarImagenFondo();

    jugador.personaje_x = 0;
    jugador.personaje_y = ALTURA_PANTALLA - TAMANO_PERSONAJE - 42;

    corriendo = true;

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60);
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_start_timer(timer);

    while (corriendo) {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);
        cargarGravedad();

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            corriendo = false;
        }
        else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            switch (event.keyboard.keycode) {
            case ALLEGRO_KEY_LEFT:
                key_left = true;
                break;
            case ALLEGRO_KEY_RIGHT:
                key_right = true;
                break;
            case ALLEGRO_KEY_UP:
                key_up = true;
                break;
            case ALLEGRO_KEY_DOWN:
                key_down = true;
                break;
            }
        }
        else if (event.type == ALLEGRO_EVENT_KEY_UP) {
            switch (event.keyboard.keycode) {
            case ALLEGRO_KEY_LEFT:
                key_left = false;
                break;
            case ALLEGRO_KEY_RIGHT:
                key_right = false;
                break;
            case ALLEGRO_KEY_UP:
                key_up = false;
                break;
            case ALLEGRO_KEY_DOWN:
                key_down = false;
                break;
            }
        }
        al_clear_to_color(al_map_rgb(0, 0, 0));
        // printf("\nCoordenadas: (%d, %d)", personaje_x/50, (personaje_y + TAMANO_PERSONAJE)/50);
        al_draw_bitmap(imagen_fondo, 0, 0, 0);
        
        for (y = 0; y < ALTURA_MAPA; ++y) {
            for (x = 0; x < ANCHO_MAPA; ++x) {
                if (mapa[y][x] == 1) {
                    al_draw_bitmap(imagen_bloque, cuadro_matriz * x, cuadro_matriz * y, 0);
                }
                else if (mapa[y][x] == 2) {
                    al_draw_bitmap(imagen_piso, cuadro_matriz * x, cuadro_matriz * y, 0);
                }
                else if (mapa[y][x] == 3) {
                    al_draw_bitmap(imagen_fuego, cuadro_matriz * x, cuadro_matriz * y, 0);
                }
            }
        }
           
        actualizarPosicionPersonaje();
        al_draw_bitmap(imagen_personaje, jugador.personaje_x, jugador.personaje_y, 0);
        al_flip_display();
    }

    al_destroy_display(display);
    al_destroy_event_queue(event_queue);
    al_destroy_bitmap(imagen_personaje);
    al_destroy_bitmap(imagen_bloque);
    al_destroy_bitmap(imagen_fondo);
    al_destroy_timer(timer);

    return 0;
}