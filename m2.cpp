/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "m1.h"
#include "m2.h"
#include "StreetsDatabaseAPI.h"
#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"
#include "helper_functions.h"
#include "graphics_functions.h"
#include "UI_functions.h"

void drawMap() {
    
    // Create our EZGL application, using our predefined UI 
    // layout and main drawing window
    ezgl::application::settings settings;
    settings.main_ui_resource ="libstreetmap/resources/main.ui";
    // UI layout
    settings.window_identifier ="MainWindow";
    settings.canvas_identifier ="MainCanvas";
    ezgl::application application(settings);

    ezgl::rectangle initial_world{{xFromLon(map_bounds.min_lon, map_bounds.avg_lat), yFromLat(map_bounds.min_lat)}, 
                                  {xFromLon(map_bounds.max_lon, map_bounds.avg_lat), yFromLat(map_bounds.max_lat)}};

    application.add_canvas("MainCanvas", draw_main_canvas, initial_world, {240, 236, 228, 255});
    
    application.run(initial_setUp,act_on_mouse_click,NULL,act_on_key_press); // Set up callback functions
    
}






