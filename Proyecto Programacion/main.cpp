#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <stdio.h>
#include <stdlib.h>

const int ANCHO_PANTALLA = 800;
const int ALTURA_PANTALLA = 600;

const int TAMANO_PERSONAJE = 50;
const int VELOCIDAD_PERSONAJE = 5;

const int ANCHO_MAPA = 16;
const int ALTURA_MAPA = 12;

int personaje_x;
int personaje_y;

bool key_left = false;
bool key_right = false;
bool key_up = false;
bool key_down = false;

int mapa[ALTURA_MAPA][ANCHO_MAPA];

ALLEGRO_BITMAP* imagen_bloque;
ALLEGRO_BITMAP* imagen_fondo;

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

void cargarImagenBloque() {
    imagen_bloque = al_load_bitmap("bloque.png");
}

void cargarImagenFondo() {
    imagen_fondo = al_load_bitmap("wallpaper.png");
}

void actualizarPosicionPersonaje() {
    int nueva_x; // Posicion izquierda
    int nueva_y; // Posicion arriba


    if (key_left) {
        nueva_x = personaje_x - VELOCIDAD_PERSONAJE;
        personaje_x = nueva_x;
    }
    if (key_right) {
        nueva_x = personaje_x + VELOCIDAD_PERSONAJE;
        personaje_x = nueva_x;
    }
    if (key_up) {
        nueva_y = personaje_y - VELOCIDAD_PERSONAJE;
        personaje_y = nueva_y;
    }
    if (key_down) {
        nueva_y = personaje_y + VELOCIDAD_PERSONAJE;
        personaje_y = nueva_y;
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
    cargarImagenBloque();
    cargarImagenFondo();

    personaje_x = 0;
    personaje_y = ALTURA_PANTALLA - TAMANO_PERSONAJE;

    corriendo = true;

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60);
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_start_timer(timer);

    while (corriendo) {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);

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
        al_draw_scaled_bitmap(imagen_fondo, 0, 0, al_get_bitmap_width(imagen_fondo), al_get_bitmap_height(imagen_fondo), 0, 0, ANCHO_PANTALLA, ALTURA_PANTALLA, 0);
        
        for (y = 0; y < ALTURA_MAPA; ++y) {
            for (x = 0; x < ANCHO_MAPA; ++x) {
                if (mapa[y][x] == 1) {
                    al_draw_scaled_bitmap(imagen_bloque, 0, 0, al_get_bitmap_width(imagen_bloque), al_get_bitmap_height(imagen_bloque),
                        x * TAMANO_PERSONAJE, y * TAMANO_PERSONAJE, TAMANO_PERSONAJE, TAMANO_PERSONAJE, 0);
                }
            }
        }

        actualizarPosicionPersonaje();
        al_draw_filled_rectangle(personaje_x, personaje_y, personaje_x + TAMANO_PERSONAJE, personaje_y + TAMANO_PERSONAJE, al_map_rgb(255, 255, 255));
        al_flip_display();
    }

    al_destroy_display(display);
    al_destroy_event_queue(event_queue);
    al_destroy_bitmap(imagen_bloque);
    al_destroy_bitmap(imagen_fondo);
    al_destroy_timer(timer);

    printf("Mensaje nuevo");

    return 0;
}