/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "data_structures.h"

std::vector<segment_data> segment_info;
std::vector<std::vector<int>> seg_idx_by_cls;
std::vector<street_data> street_info;
int highlighted_street = -1;
std::pair<int, int> highlighted_intersections;
std::vector<StreetSegmentIdx> highlighted_segments;
std::vector<feature_data> feature_info;
std::vector<POI_data> POI_info;
std::vector<int> drawn_POI;
std::vector<intersection_data> intersection_info;
std::vector<div_rect> nine_rects;
std::multimap<std::string,int> nameStreetID;
std::multimap<std::string,int> nameStreetID_noSpace;
bounds map_bounds;
double world_w;
double world_h;
renderer_data renderer_info;
std::vector<std::vector<ezgl::point2d>> rail_info;
std::unordered_map<OSMID, OSM_data> OSM_info;
std::vector<Node> nodes;
std::string instructions;
