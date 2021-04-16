/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   UI_functions.h
 * Author: nimingha
 *
 * Created on March 4, 2021, 8:36 AM
 */

#ifndef UI_FUNCTIONS_H
#define UI_FUNCTIONS_H

#include "m1.h"
#include "m2.h"
#include "m3.h"
#include "navigation_functions.h"
#include "data_structures.h"
#include "StreetsDatabaseAPI.h"
#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"
#include "graphics_functions.h"
#include <algorithm>
#include "ezgl/callback.hpp"
#include "helper_functions.h"

float lonFromX(float x,float latAvg);

float latFromY(float y);

void act_on_mouse_click(ezgl::application* app,
                        GdkEventButton* event,
                        double x,double y);
void search_button(GtkWidget *,ezgl::application* application);

void change_map_button(GtkWidget *,ezgl::application* application);

void map_selection_box(GtkWidget *widget,ezgl::application* application);
void mode_selection_box(GtkWidget *widget,ezgl::application* application);
/*
void act_on_mouse_move(ezgl::application* app,
                        GdkEventButton* event,
                        double x,double y); */
void act_on_key_press(ezgl::application*,
                        GdkEventKey*,
                        char*);
bool validKeyPress(char* key_name);
void act_on_first_box(GtkEntry*,ezgl::application* app,char*);
void act_on_second_box(GtkEntry*,ezgl::application* app,char*);
void clear_button(GtkWidget *,ezgl::application* application);
void find_path_button(GtkWidget *,ezgl::application* application);
void help_button(GtkWidget *,ezgl::application* application);
void help_dialog_response(GtkDialog* dialog, gint response_id);
void navi_dialog_response(GtkDialog* dialog, gint response_id);
std::string prediction(std::string input);
extern double mouseScreenCoordinateX,mouseScreenCoordinateY,lastScreenClickX,lastScreenClickY;
extern std::string keyPressed1,keyPressed2;
extern IntersectionIdx last_click;
extern int map_index;
extern int help_dialog;
void zoom_to_level(int dest, ezgl::application* app);
void show_directions(ezgl::application* application, GtkWidget* label, GtkWidget* dialog);

#endif /* UI_FUNCTIONS_H */

