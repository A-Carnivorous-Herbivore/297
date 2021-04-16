/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   data_structures.h
 * Author: nimingha
 *
 * Created on February 4, 2021, 12:35 PM
 */

#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <map>
#include <string>
#include <vector>
#include <unordered_map>
#include "m1.h"
#include <algorithm>
#include "math.h"
#include "LatLon.h"
#include "StreetsDatabaseAPI.h"
#include "OSMDatabaseAPI.h"
#include "ezgl/point.hpp"
#include "ezgl/rectangle.hpp"

struct bounds{
    double max_lat = -91;
    double min_lat = 91;
    double max_lon = -181;
    double min_lon = 181;
    double avg_lat = 0;
};
extern bounds map_bounds;
extern double world_w;
extern double world_h;

struct trigs{
    double name;
    double deg;
    double sin_1;
    double sin_2;
    double sin_3;
    double cos_1;
    double cos_2;
    double cos_3;
};

struct segment_data{
    double    length;
    double    time;
    bool      oneway;
    StreetIdx streetID;
    IntersectionIdx from;
    IntersectionIdx to;
    std::vector<trigs> angles;
    std::vector<ezgl::point2d> pts;
    std::vector<ezgl::point2d> arr;
    std::vector<ezgl::point2d> name;
};
struct intersection_data{
    std::vector <StreetSegmentIdx> streetSegmentIdVector;
    std::vector <std::string> streetNameVector;
    std::vector <StreetIdx> streetIdVector;
    std::vector <IntersectionIdx> reachableIntersectionsVector;
    std::vector <StreetSegmentIdx> outgoingSegments;
    LatLon coord;
};
extern std::vector<segment_data> segment_info;
extern std::vector<intersection_data> intersection_info;
extern std::vector<std::vector<int>> seg_idx_by_cls;
extern std::pair<int, int> highlighted_intersections;
extern std::vector<StreetSegmentIdx> highlighted_segments;

struct OSM_data{
    const OSMNode* node_ptr;
    const OSMWay*  way_ptr;
};
extern std::vector<std::vector<ezgl::point2d>> rail_info;
extern std::unordered_map<OSMID, OSM_data> OSM_info;

struct street_data{
    double                        length = 0;
    std::string                   name;
    LatLonBounds                  bounding_box;
    std::vector<LatLon>           curve_pts;
    std::vector<IntersectionIdx>  intersections;
    std::vector<StreetSegmentIdx> segments;
};
extern std::vector<street_data> street_info;
extern int highlighted_street;

struct feature_data{
    int area_class;
    double area;
    std::vector<ezgl::point2d> bound_pts;
    std::vector<ezgl::point2d> rect_bound;
    std::string name;
    FeatureType type;
};
extern std::vector<feature_data> feature_info;

struct POI_data{
    std::string type;
    std::string name;
    LatLon loc;
    int ID;
};
extern std::vector<POI_data> POI_info;
extern std::vector<int> drawn_POI;

extern std::multimap<std::string,int> nameStreetID;
extern std::multimap<std::string,int> nameStreetID_noSpace;

#include "graphics_functions.h"

struct renderer_data{
    
    int init_zoom;
    int curr_zoom;
    std::vector<double> factors;
    
    void init_zoom_level(){
        
        double x = xFromLon(map_bounds.max_lon - map_bounds.min_lon, map_bounds.avg_lat) / 1000000 / kDegreeToRadian;
        curr_zoom = std::round(- (log(x) / log(1.66667)) + 2.52906);
        init_zoom = curr_zoom;
        std::cout << "current zoom level is " << curr_zoom << std::endl;
        
    }
    
    void update_factors(){
        
        std::cout << "current zoom level is " << curr_zoom << std::endl;
        
        //for streets, streams and rivers
        factors[0] = pow(1.19, curr_zoom);
        factors[1] = pow(1.19, curr_zoom - 1);
        factors[2] = pow(1.19, curr_zoom - 2);
        factors[3] = pow(1.19, curr_zoom - 3);
        
        //for arrows
        factors[4] = pow(1.4, 3 - curr_zoom) * kDegreeToRadian;
        
        //for street names
        factors[5] = 8 * pow(1.01, curr_zoom);
        
        //for POI names
        factors[6] = 9 * pow(1.01, curr_zoom);
        
        //for highlighted elements
        factors[7] = 10 * pow(1.28, 9 - curr_zoom);
        
        //for non-png POI's
        factors[8] = 10 * pow(1.20, 7 - curr_zoom);
        
    }
    
};
extern renderer_data renderer_info;

struct div_rect{
    ezgl::rectangle rect;
    int POI_count = 0;
    int name_count = 0;
    int arrow_count = 0;
    int valid_name = -1;
    int valid_arrow = -1;
};
extern std::vector<div_rect> nine_rects;

struct Node{
    IntersectionIdx nodeID;
    StreetSegmentIdx reachingEdge;
    double bestTime;
    std::vector<int> pickUp;
    std::vector<int> dropOff;
    int depot;
};

struct waveElem{
    IntersectionIdx nodeIdx;
    StreetSegmentIdx segID;
    double travelTime;
    
    waveElem(int n, StreetSegmentIdx id, double time)
    {
        nodeIdx = n; 
        segID = id;
        travelTime = time;
    }
};

extern std::vector<Node> nodes;
extern std::string instructions;
#endif /* DATA_STRUCTURES_H */

