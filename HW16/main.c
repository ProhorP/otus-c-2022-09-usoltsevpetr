#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

int main()
{
    al_init();
    al_init_image_addon();
    ALLEGRO_DISPLAY* display = al_create_display(640, 480);
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    al_install_keyboard();
    al_install_mouse();
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_mouse_event_source());

    ALLEGRO_BITMAP* bitmap = al_load_bitmap("default.bmp");
    if(!bitmap)
        return EXIT_FAILURE;

    float x = 0, y = 0;
    bool running = true;
    int width = al_get_display_width(display);
    while(running)
    {
        al_clear_to_color(al_map_rgba_f(0, 0, 0, 1));
        al_draw_bitmap(bitmap, x, y, 0);
        al_flip_display();
        ALLEGRO_EVENT event;
        while(al_get_next_event(queue, &event))
        {
            if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
                running = false;
            if (event.type == ALLEGRO_EVENT_MOUSE_AXES)
            {
                x = event.mouse.x;
                y = event.mouse.y;
            }
            if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
            {
                x = y = 0;
                al_set_mouse_xy(display, 0, 0);
            }
        }

        // Actively poll the keyboard
        ALLEGRO_KEYBOARD_STATE keyState;
        al_get_keyboard_state(&keyState);
        if (al_key_down(&keyState, ALLEGRO_KEY_RIGHT))
            if (al_key_down(&keyState, ALLEGRO_KEY_LCTRL))
                x += 10;
            else
                x += 1;
        if (al_key_down(&keyState, ALLEGRO_KEY_LEFT))
            if (al_key_down(&keyState, ALLEGRO_KEY_LCTRL))
                x -= 10;
            else
                x -= 1;
    }
    al_destroy_display(display);
    al_uninstall_keyboard();
    al_uninstall_mouse();
    al_destroy_bitmap(bitmap);

    return 0;
}
