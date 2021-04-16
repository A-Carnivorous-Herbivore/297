/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   graphics_functions.h
 * Author: nimingha
 *
 * Created on March 4, 2021, 8:28 AM
 */

#ifndef GRAPHICS_FUNCTIONS_H
#define GRAPHICS_FUNCTIONS_H

#include "m1.h"
#include "m2.h"
#include "StreetsDatabaseAPI.h"
#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"
#include <algorithm>

float xFromLon(float lon, float latAvg);
float yFromLat(float lat);

void draw_main_canvas(ezgl::renderer *g);

void div_rects(ezgl::renderer *g);

int check_rects(ezgl::point2d pt, int type);

void draw_features(ezgl::renderer *g);

void draw_poi(ezgl::renderer *g);

void draw_intersections(ezgl::renderer *g);

void draw_rail(ezgl::renderer *g);

void initial_setUp(ezgl::application *application,bool );

void draw_searchBox(ezgl::renderer *g);

void set_seg_colour(ezgl::renderer *g, int cls);

void draw_curve_points(ezgl::renderer *g, int streetSegmentIdx);

void draw_streets(ezgl::renderer *g);


#endif /* GRAPHICS_FUNCTIONS_H */

