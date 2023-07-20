#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define cuadro_matriz 40
#define ANCHO_PERSONAJE 30

#define MAX_FUEGOS 100

const int ANCHO_PANTALLA = 1600;
const int ALTURA_PANTALLA = 920;

const int TAMANO_PERSONAJE = 60;
const int VELOCIDAD_PERSONAJE = 3;

const int ANCHO_MAPA = 40;
const int ALTURA_MAPA = 23;

const int VELOCIDAD_FUEGO = 5;

bool key_left = false;
bool key_right = false;
bool key_up = false;
bool key_down = false;

bool puedeTeletransportar = true;
bool superSaltoHabilitado = true;

int velocidad_salto = 10;
int velocidad_actual_salto = 0;
int mapa[ALTURA_MAPA][ANCHO_MAPA];

struct personaje {
    int personaje_x = 0, personaje_y = 0;
    bool super_salto;
};

struct enemigo {
    int enemigo_x = 0, enemigo_y = 0;
    int direccion = 1; // 1: derecha, -1: izquierda
};

struct fuego {
    int x, y;
};

struct enemigo enemigo_1;
struct personaje jugador;
struct fuego lista_fuegos[MAX_FUEGOS];

ALLEGRO_BITMAP* imagen_personaje;
ALLEGRO_BITMAP* imagen_bloque;
ALLEGRO_BITMAP* imagen_fondo;
ALLEGRO_BITMAP* imagen_piso;
ALLEGRO_BITMAP* imagen_fuego;
ALLEGRO_BITMAP* imagen_enemigo;

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

void cargarGravedad() {
    if (mapa[(jugador.personaje_y + TAMANO_PERSONAJE + 1) / cuadro_matriz][jugador.personaje_x / cuadro_matriz] == 0 && mapa[(jugador.personaje_y + TAMANO_PERSONAJE) / cuadro_matriz][(jugador.personaje_x + ANCHO_PERSONAJE) / cuadro_matriz] == 0) {
        jugador.personaje_y += 1;
    }
}

ALLEGRO_SAMPLE* musica_fondo;

void cargarMusicaFondo() {
    musica_fondo = al_load_sample("musica_fondo.mp3");
    if (!musica_fondo) {
        printf("Error al cargar la música de fondo.\n");
    }
}

void reproducirMusicaFondo() {
    if (musica_fondo) {
        al_play_sample(musica_fondo, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, NULL);
    }
}

void cargarPersonaje() {
    imagen_personaje = al_load_bitmap("player.png");
}

void inicializarPersonaje() {
    jugador.personaje_x = 0;
    jugador.personaje_y = ALTURA_PANTALLA - TAMANO_PERSONAJE - 45;
    jugador.super_salto = false;
}

const int TIEMPO_ESPERA_SUPER_SALTO = 60;
time_t tiempoUltimoSuperSalto = 0;

void actualizarPosicionPersonaje() {
    int nueva_x;
    int nueva_y;

    if (key_left && mapa[(jugador.personaje_y + TAMANO_PERSONAJE) / cuadro_matriz][(jugador.personaje_x - 2) / cuadro_matriz] != 1 && mapa[(jugador.personaje_y) / cuadro_matriz][(jugador.personaje_x - 2) / cuadro_matriz] != 1 && mapa[(jugador.personaje_y + TAMANO_PERSONAJE / 2) / cuadro_matriz][(jugador.personaje_x - 2) / cuadro_matriz] != 1) {
        nueva_x = jugador.personaje_x - VELOCIDAD_PERSONAJE;
        jugador.personaje_x = nueva_x;
    }
    if (key_right && mapa[(jugador.personaje_y + TAMANO_PERSONAJE) / cuadro_matriz][(jugador.personaje_x + ANCHO_PERSONAJE + 2) / cuadro_matriz] != 1 && mapa[(jugador.personaje_y) / cuadro_matriz][(jugador.personaje_x + ANCHO_PERSONAJE + 2) / cuadro_matriz] != 1 && mapa[(jugador.personaje_y + TAMANO_PERSONAJE / 2) / cuadro_matriz][(jugador.personaje_x + ANCHO_PERSONAJE + 2) / cuadro_matriz] != 1) {
        nueva_x = jugador.personaje_x + VELOCIDAD_PERSONAJE;
        jugador.personaje_x = nueva_x;
    }
    if (key_up && (mapa[(jugador.personaje_y + TAMANO_PERSONAJE + 2) / cuadro_matriz][(jugador.personaje_x + ANCHO_PERSONAJE) / cuadro_matriz] != 0 || mapa[(jugador.personaje_y + TAMANO_PERSONAJE + 2) / cuadro_matriz][(jugador.personaje_x) / cuadro_matriz] != 0)) {
        if (velocidad_actual_salto == 0) {
            velocidad_actual_salto = velocidad_salto;
        }
    }
    else if (jugador.super_salto && (mapa[(jugador.personaje_y + TAMANO_PERSONAJE + 2) / cuadro_matriz][(jugador.personaje_x + ANCHO_PERSONAJE) / cuadro_matriz] != 0 || mapa[(jugador.personaje_y + TAMANO_PERSONAJE + 2) / cuadro_matriz][(jugador.personaje_x) / cuadro_matriz] != 0)) {
        if (velocidad_actual_salto == 0 && (time(NULL) - tiempoUltimoSuperSalto) >= TIEMPO_ESPERA_SUPER_SALTO) {
            velocidad_actual_salto = velocidad_salto * 2; // Incrementa la velocidad de salto para el super salto
            tiempoUltimoSuperSalto = time(NULL); // Guarda el tiempo actual como el tiempo del último uso del super_salto
        }
    }

    if (velocidad_actual_salto > 0) {
        nueva_y = jugador.personaje_y - velocidad_actual_salto;
        jugador.personaje_y = nueva_y;
        velocidad_actual_salto--;
    }
}

clock_t tiempoInicio;

void teletransportarPersonaje() {
    if (puedeTeletransportar) {
        int nueva_x = jugador.personaje_x + (cuadro_matriz * 6);

        int bloque_x = nueva_x / cuadro_matriz;
        int bloque_y1 = jugador.personaje_y / cuadro_matriz;
        int bloque_y2 = (jugador.personaje_y + TAMANO_PERSONAJE) / cuadro_matriz;

        if (mapa[bloque_y1][bloque_x] == 0 && mapa[bloque_y2][bloque_x] == 0) {
            jugador.personaje_x = nueva_x;
            puedeTeletransportar = false;
            tiempoInicio = clock();
        }
    }
}

void verificarTiempoEspera() {
    if (!puedeTeletransportar) {
        clock_t tiempoActual = clock();
        double tiempoPasado = (tiempoActual - tiempoInicio) / (double)CLOCKS_PER_SEC;

        if (tiempoPasado >= 10.0) {
            puedeTeletransportar = true;
        }
    }
}

ALLEGRO_FONT* fuente = NULL;

void dibujarTexto() {
    if (!fuente) {
        fuente = al_create_builtin_font();
    }

    if (puedeTeletransportar) {
        al_draw_text(fuente, al_map_rgb(255, 255, 255), 10, 10, 0, "Teletransportarse: Habilitado");
    }
    else {
        al_draw_text(fuente, al_map_rgb(255, 0, 0), 10, 10, 0, "Teletransportarse: Deshabilitado");
    }
}

void dibujarTextoSuperSalto() {
    if (!fuente) {
        fuente = al_create_builtin_font();
    }

    int tiempoRestante = TIEMPO_ESPERA_SUPER_SALTO - (time(NULL) - tiempoUltimoSuperSalto);
    char mensaje[100];
    if (tiempoRestante >= 0) {
        sprintf_s(mensaje, 100, "Super Salto: Presionar tecla E - Habilitado cada 60 segundos (te quedan %d segundos)", tiempoRestante);
        al_draw_text(fuente, al_map_rgb(255, 0, 0), 10, 30, 0, mensaje);
    }
    else
    {
        sprintf_s(mensaje, 100, "Super Salto: Presionar tecla E - Habilitado cada 60 segundos (te quedan 0 segundos)", tiempoRestante);
        al_draw_text(fuente, al_map_rgb(255, 255, 255), 10, 30, 0, mensaje);
    }
}


int verificarColision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2) {
    if (x1 < x2 + w2 &&
        x1 + w1 > x2 &&
        y1 < y2 + h2 &&
        y1 + h1 > y2) {
        return 1; // Hay colisión
    }
    return 0; // No hay colisión
}

void cargarEnemigo() {
    imagen_enemigo = al_load_bitmap("zombie.png");
}

void inicializarEnemigo() {
    enemigo_1.enemigo_x = ANCHO_PANTALLA; // Posición inicial fuera de la pantalla (a la derecha)
    enemigo_1.enemigo_y = ALTURA_PANTALLA - TAMANO_PERSONAJE - 42;
    enemigo_1.direccion = -1; // Inicialmente se moverá hacia la izquierda
}

void moverEnemigo() {
    int bloque_x, bloque_y1, bloque_y2;

    // Verifica colisiones con los bloques en la dirección actual del enemigo
    if (enemigo_1.direccion == 1) {
        bloque_x = (enemigo_1.enemigo_x + ANCHO_PERSONAJE + 2) / cuadro_matriz;
        bloque_y1 = enemigo_1.enemigo_y / cuadro_matriz;
        bloque_y2 = (enemigo_1.enemigo_y + TAMANO_PERSONAJE) / cuadro_matriz;

        if (mapa[bloque_y1][bloque_x] == 1 || mapa[bloque_y2][bloque_x] == 1) {
            enemigo_1.direccion = -1; // Cambia la dirección a la izquierda si hay colisión con un bloque a la derecha
        }
    }
    else if (enemigo_1.direccion == -1) {
        bloque_x = (enemigo_1.enemigo_x - 2) / cuadro_matriz;
        bloque_y1 = enemigo_1.enemigo_y / cuadro_matriz;
        bloque_y2 = (enemigo_1.enemigo_y + TAMANO_PERSONAJE) / cuadro_matriz;

        if (mapa[bloque_y1][bloque_x] == 1 || mapa[bloque_y2][bloque_x] == 1) {
            enemigo_1.direccion = 1; // Cambia la dirección a la derecha si hay colisión con un bloque a la izquierda
        }
    }

    // Mover el enemigo en la dirección actual
    enemigo_1.enemigo_x += VELOCIDAD_PERSONAJE * enemigo_1.direccion;

    // Verifica si el enemigo toca el borde de la ventana y cambiar dirección nuevamente
    if (enemigo_1.enemigo_x >= ANCHO_PANTALLA - ANCHO_PERSONAJE) {
        enemigo_1.direccion = -1; // Cambia la dirección hacia la izquierda cuando toca el borde derecho
    }
    else if (enemigo_1.enemigo_x <= 0) {
        enemigo_1.direccion = 1; // Cambia la dirección hacia la derecha cuando toca el borde izquierdo
    }
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

const int FUERA_PANTALLA_X = -100;
const int FUERA_PANTALLA_Y = -100;

void inicializarFuego() {
    for (int i = 0; i < MAX_FUEGOS; i++) {
        lista_fuegos[i].x = FUERA_PANTALLA_X;
        lista_fuegos[i].y = FUERA_PANTALLA_Y;
    }
}

double tiempoEsperaFuego = 2.0;
double tiempoAcumulado = 0.0;
int fuegosActivos = 0;

void actualizarFuego() {
    tiempoAcumulado += 1.0 / 60.0;

    if (tiempoAcumulado >= tiempoEsperaFuego && fuegosActivos < MAX_FUEGOS) {
        lista_fuegos[fuegosActivos].x = rand() % (ANCHO_PANTALLA - cuadro_matriz);
        lista_fuegos[fuegosActivos].y = -(rand() % (ALTURA_PANTALLA / 2));
        fuegosActivos++;
        tiempoAcumulado = 0.0;
    }

    for (int i = 0; i < fuegosActivos; i++) {
        lista_fuegos[i].y += VELOCIDAD_FUEGO;
    }
}

int verificarColisionFuego(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2) {
    if (x1 < x2 + w2 &&
        x1 + w1 > x2 &&
        y1 < y2 + h2 &&
        y1 + h1 > y2) {
        return 1; // Hay colisión
    }
    return 0; // No hay colisión
}

int main() {
    int x, y;
    bool corriendo;
    al_init();
    al_init_primitives_addon();
    al_init_image_addon();
    al_install_keyboard();
    al_init_font_addon();

    al_install_audio();
    al_init_acodec_addon();
    al_reserve_samples(1);
    al_set_default_mixer(al_get_default_mixer());

    ALLEGRO_DISPLAY* display = al_create_display(ANCHO_PANTALLA, ALTURA_PANTALLA);
    ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_keyboard_event_source());

    cargarMapa("mapa.txt");
    cargarGravedad();
    cargarPersonaje();
    cargarEnemigo();
    cargarImagenBloque();
    cargarImagenPiso();
    cargarImagenFuego();
    cargarImagenFondo();
    cargarImagenFuego();
    cargarMusicaFondo();
    reproducirMusicaFondo();

    inicializarEnemigo();
    inicializarPersonaje();
    inicializarFuego();

    corriendo = true;

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60);
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_start_timer(timer);

    while (corriendo) {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);
        reproducirMusicaFondo();
        cargarGravedad();
        actualizarFuego();
        moverEnemigo();
        verificarTiempoEspera();

        for (int i = 0; i < MAX_FUEGOS; i++) {
            if (verificarColisionFuego(lista_fuegos[i].x, lista_fuegos[i].y, cuadro_matriz, cuadro_matriz, jugador.personaje_x, jugador.personaje_y, TAMANO_PERSONAJE, TAMANO_PERSONAJE)) {
                corriendo = false;
                break;
            }
        }


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
            case ALLEGRO_KEY_R:
                teletransportarPersonaje();
                break;
            case ALLEGRO_KEY_E:
                jugador.super_salto = true;
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
            case ALLEGRO_KEY_E:
                jugador.super_salto = false;
                break;
            }
        }

        al_clear_to_color(al_map_rgb(0, 0, 0));
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
        al_draw_bitmap(imagen_enemigo, enemigo_1.enemigo_x, enemigo_1.enemigo_y, 0);
        for (int i = 0; i < MAX_FUEGOS; i++) {
            al_draw_bitmap(imagen_fuego, lista_fuegos[i].x, lista_fuegos[i].y, 0);
        }
        dibujarTexto();
        dibujarTextoSuperSalto();
        al_flip_display();
    }

    al_stop_samples();
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);
    al_destroy_bitmap(imagen_personaje);
    al_destroy_bitmap(imagen_bloque);
    al_destroy_bitmap(imagen_fondo);
    al_destroy_sample(musica_fondo);
    al_destroy_timer(timer);

    return 0;
}

// cambiar matriz a char