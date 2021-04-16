/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   HelperFucntion.h
 * Author: jinrudy
 *
 * Created on February 6, 2021, 4:05 PM
 */

#ifndef HELPERFUCNTION_H
#define HELPERFUCNTION_H

#include <map>
#include <string>
#include <vector>
#include <unordered_map>
#include "data_structures.h"
#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"

void lowerToUpper(std::string& temp, int index);
void upperToLower(std::string& temp, int index);
std::vector<int> duplication_prevention(std::vector<int> data);
std::vector<std::string> duplication_prevention_s(std::vector<std::string> data);
ezgl::point2d cartConv(LatLon coord);
std::string lowerCaseNoSpace(std::string str);
bool pts_in_bound(std::vector<ezgl::point2d> pts, ezgl::rectangle bounds);
trigs calculate_angles(LatLon from, LatLon to);
std::string name_shortener(std::string streetName);
void draw_arrows(ezgl::renderer *g, ezgl::point2d center, trigs angles, double zoom_factor);
int modify_zoom_level(int ID);
int modify_zoom_level_to_segs(void);
void check_highlighted_intersections(int id);

#endif /* HELPERFUCNTION_H */

