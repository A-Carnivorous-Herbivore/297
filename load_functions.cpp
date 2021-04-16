/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <utility>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <map>
#include <string>
#include <vector>
#include <unordered_map>
#include "m1.h"
#include "math.h"
#include "LatLon.h"
#include "StreetsDatabaseAPI.h"
#include "OSMDatabaseAPI.h"
#include "data_structures.h"
#include "load_functions.h"
#include "helper_functions.h"
#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"
#include "ezgl/point.hpp"
#include "graphics_functions.h"
#include "navigation_functions.h"
#include <bits/stdc++.h>

void load_OSM_info(){
    
    for(int i = 0; i < getNumberOfNodes(); i++){
        
        const OSMNode* node = getNodeByIndex(i);
        OSM_info[node->id()].node_ptr = node;
        
    }
    
    for(int idx = 0; idx < getNumberOfWays(); idx++){
        
        const OSMWay* way = getWayByIndex(idx);
        
        std::vector<OSMID> node_list = getWayMembers(way);
        OSM_info[way->id()].way_ptr = way;
        
        std::vector<ezgl::point2d> info;
        ezgl::point2d point(0, 0);
        
        //load the data for railways
        for(int i = 0; i < getTagCount(way); ++i){
            
            std::pair<std::string, std::string> tag_pair = getTagPair(way, i);
            
            if(tag_pair.first == "railway"){
                
                if(tag_pair.second == "rail"){
                    
                    if(node_list.size() == 1){
                        //push back the only point
                        point.x = xFromLon(getNodeCoords(OSM_info[node_list[0]].node_ptr).longitude(), map_bounds.avg_lat);
                        point.y = yFromLat(getNodeCoords(OSM_info[node_list[0]].node_ptr).latitude());
                        info.push_back(point);
                    }
                    
                    //loop through all nodes of the way and calculate the length
                    for (auto it = node_list.begin(); it < node_list.end() - 1; it++){

                        //push back the ith point
                        point.x = xFromLon(getNodeCoords(OSM_info[*it].node_ptr).longitude(), map_bounds.avg_lat);
                        point.y = yFromLat(getNodeCoords(OSM_info[*it].node_ptr).latitude());
                        info.push_back(point);
                        
                        //the last point
                        if(it == node_list.end() - 2){
                            point.x = xFromLon(getNodeCoords(OSM_info[*(it + 1)].node_ptr).longitude(), map_bounds.avg_lat);
                            point.y = yFromLat(getNodeCoords(OSM_info[*(it + 1)].node_ptr).latitude());
                            info.push_back(point);
                        }

                    }
                    
                    rail_info.push_back(info);
                    break;
                }
                
            }
            
        }
        
    }
    
}

void load_segment_info(){
    
    //initialize data structure size
    seg_idx_by_cls.resize(11);
    street_info.resize(getNumStreets());
    segment_info.resize(getNumStreetSegments());
    
    //loop through all street segments to fetch data
    for(int SegmentIdx = 0; SegmentIdx < getNumStreetSegments(); SegmentIdx++){
        
        //fetch data of current segment from database
        StreetSegmentInfo temp_info = getStreetSegmentInfo(SegmentIdx);
        double speed = temp_info.speedLimit;
        
        //find which street the segment belongs to
        street_info[temp_info.streetID].segments.push_back(SegmentIdx);
        street_info[temp_info.streetID].intersections.push_back(temp_info.from);
        street_info[temp_info.streetID].intersections.push_back(temp_info.to);
        street_info[temp_info.streetID].name = getStreetName(temp_info.streetID);
        
        //find Segment Properties
        segment_info[SegmentIdx].oneway = getStreetSegmentInfo(SegmentIdx).oneWay;
        segment_info[SegmentIdx].streetID = getStreetSegmentInfo(SegmentIdx).streetID;
        segment_info[SegmentIdx].from = temp_info.from;
        segment_info[SegmentIdx].to = temp_info.to;
      
        //get the OSM highway tag in order to draw with levels of detail
        for(int i = 0; i < getTagCount(OSM_info[temp_info.wayOSMID].way_ptr); ++i){
            
            std::pair<std::string, std::string> tag_pair = getTagPair(OSM_info[temp_info.wayOSMID].way_ptr, i);
            
            if(tag_pair.first == "highway"){
                
                std::string value = tag_pair.second;
                
                //store the street ID's by their class
                if(value == "service" || value == "road" || value == "living_street" || value == "pedestrian"){
                    seg_idx_by_cls[10].push_back(SegmentIdx);
                    break;
                }
                
                else if(value == "residential"){
                    seg_idx_by_cls[9].push_back(SegmentIdx);
                    break;
                }
                
                else if(value == "unclassified"){
                    seg_idx_by_cls[8].push_back(SegmentIdx);
                    break;
                }
                
                else if(value == "tertiary_link"){
                    seg_idx_by_cls[7].push_back(SegmentIdx);
                    break;
                }
                
                else if(value == "tertiary"){
                    seg_idx_by_cls[6].push_back(SegmentIdx);
                    break;
                }
                
                else if(value == "secondary_link"){
                    seg_idx_by_cls[5].push_back(SegmentIdx);
                    break;
                }
                
                else if(value == "secondary"){
                    seg_idx_by_cls[4].push_back(SegmentIdx);
                    break;
                }
                
                else if(value == "primary_link"){
                    seg_idx_by_cls[3].push_back(SegmentIdx);
                    break;
                }
                
                else if(value == "motorway_link" || value == "trunk_link"){
                    seg_idx_by_cls[2].push_back(SegmentIdx);
                    break;
                }
                
                else if(value == "primary"){
                    seg_idx_by_cls[1].push_back(SegmentIdx);
                    break;
                }
                
                else if(value == "motorway" || value == "trunk"){
                    seg_idx_by_cls[0].push_back(SegmentIdx);
                    break;
                }
                
                else
                    seg_idx_by_cls[10].push_back(SegmentIdx);
                
            }
            
        }
        
        //create a pair with the two endpoints of the segments
        LatLon fromPt, endPt;
        std::pair<LatLon, LatLon> pts(fromPt, endPt);
        segment_info[SegmentIdx].pts.resize(temp_info.numCurvePoints + 2);
        segment_info[SegmentIdx].arr.resize(temp_info.numCurvePoints + 1);
        segment_info[SegmentIdx].name.resize(temp_info.numCurvePoints + 1);
        segment_info[SegmentIdx].angles.resize(temp_info.numCurvePoints + 1);
        
        //discuss the simple case first
        if(temp_info.numCurvePoints == 0){
            
            pts.first = getIntersectionPosition(temp_info.from);
            pts.second = getIntersectionPosition(temp_info.to);
            street_info[temp_info.streetID].curve_pts.push_back(getIntersectionPosition(temp_info.from));
            street_info[temp_info.streetID].curve_pts.push_back(getIntersectionPosition(temp_info.to));
            segment_info[SegmentIdx].length = findDistanceBetweenTwoPoints(pts);
            segment_info[SegmentIdx].time = segment_info[SegmentIdx].length / speed;
            street_info[temp_info.streetID].length += segment_info[SegmentIdx].length;
            
            segment_info[SegmentIdx].pts[0].x = xFromLon(pts.first.longitude(), map_bounds.avg_lat);
            segment_info[SegmentIdx].pts[0].y = yFromLat(pts.first.latitude());
            segment_info[SegmentIdx].pts[1].x = xFromLon(pts.second.longitude(), map_bounds.avg_lat);
            segment_info[SegmentIdx].pts[1].y = yFromLat(pts.second.latitude());
            segment_info[SegmentIdx].angles[0] = calculate_angles(pts.first, pts.second);
            
        }
        
        //otherwise loop through all the curve point and add up results for final length
        else{
            
            double length = 0;
            for(int i = 0; i <= temp_info.numCurvePoints; i++){
                
                //first curve point
                if(i == 0){
                    
                    pts.first = getIntersectionPosition(temp_info.from);
                    pts.second = getStreetSegmentCurvePoint(SegmentIdx, i);
                    street_info[temp_info.streetID].curve_pts.push_back(pts.first);
                    street_info[temp_info.streetID].curve_pts.push_back(pts.second);
                    
                    segment_info[SegmentIdx].pts[0].x = xFromLon(pts.first.longitude(), map_bounds.avg_lat);
                    segment_info[SegmentIdx].pts[0].y = yFromLat(pts.first.latitude());
                    segment_info[SegmentIdx].pts[1].x = xFromLon(pts.second.longitude(), map_bounds.avg_lat);
                    segment_info[SegmentIdx].pts[1].y = yFromLat(pts.second.latitude());
                    segment_info[SegmentIdx].angles[0] = calculate_angles(pts.first, pts.second);
                    
                    segment_info[SegmentIdx].arr[0].x = (4 * segment_info[SegmentIdx].pts[0].x +
                                                            segment_info[SegmentIdx].pts[1].x) / 5;
                    segment_info[SegmentIdx].arr[0].y = (4 * segment_info[SegmentIdx].pts[0].y +
                                                            segment_info[SegmentIdx].pts[1].y) / 5;
                    segment_info[SegmentIdx].name[0].x = (4 * segment_info[SegmentIdx].pts[1].x +
                                                            segment_info[SegmentIdx].pts[0].x) / 5;
                    segment_info[SegmentIdx].name[0].y = (4 * segment_info[SegmentIdx].pts[1].y +
                                                            segment_info[SegmentIdx].pts[0].y) / 5;
                    
                }
                
                //last curve point
                else if(i == temp_info.numCurvePoints){
                    
                    pts.first = getIntersectionPosition(temp_info.to);
                    pts.second = getStreetSegmentCurvePoint(SegmentIdx, i - 1);
                    street_info[temp_info.streetID].curve_pts.push_back(pts.first);
                    
                    segment_info[SegmentIdx].pts[i + 1].x = xFromLon(pts.first.longitude(), map_bounds.avg_lat);
                    segment_info[SegmentIdx].pts[i + 1].y = yFromLat(pts.first.latitude());
                    segment_info[SegmentIdx].angles[i] = calculate_angles(pts.first, pts.second);
                    
                    segment_info[SegmentIdx].arr[i].x = (4 * segment_info[SegmentIdx].pts[0].x +
                                                            segment_info[SegmentIdx].pts[1].x) / 5;
                    segment_info[SegmentIdx].arr[i].y = (4 * segment_info[SegmentIdx].pts[0].y +
                                                            segment_info[SegmentIdx].pts[1].y) / 5;
                    segment_info[SegmentIdx].name[i].x = (4 * segment_info[SegmentIdx].pts[1].x +
                                                            segment_info[SegmentIdx].pts[0].x) / 5;
                    segment_info[SegmentIdx].name[i].y = (4 * segment_info[SegmentIdx].pts[1].y +
                                                            segment_info[SegmentIdx].pts[0].y) / 5;
                    
                }
                
                //any other curve points in between
                else{
                    
                    pts.first = getStreetSegmentCurvePoint(SegmentIdx, i - 1);
                    pts.second = getStreetSegmentCurvePoint(SegmentIdx, i);
                    street_info[temp_info.streetID].curve_pts.push_back(pts.second);
                    
                    segment_info[SegmentIdx].pts[i + 1].x = xFromLon(pts.second.longitude(), map_bounds.avg_lat);
                    segment_info[SegmentIdx].pts[i + 1].y = yFromLat(pts.second.latitude());
                    segment_info[SegmentIdx].angles[i] = calculate_angles(pts.first, pts.second);
                    
                    segment_info[SegmentIdx].arr[i].x = (4 * segment_info[SegmentIdx].pts[0].x +
                                                            segment_info[SegmentIdx].pts[1].x) / 5;
                    segment_info[SegmentIdx].arr[i].y = (4 * segment_info[SegmentIdx].pts[0].y +
                                                            segment_info[SegmentIdx].pts[1].y) / 5;
                    segment_info[SegmentIdx].name[i].x = (4 * segment_info[SegmentIdx].pts[1].x +
                                                            segment_info[SegmentIdx].pts[0].x) / 5;
                    segment_info[SegmentIdx].name[i].y = (4 * segment_info[SegmentIdx].pts[1].y +
                                                            segment_info[SegmentIdx].pts[0].y) / 5;
                    
                }
                
                //add up the result
                length += findDistanceBetweenTwoPoints(pts);
                
            }
            
            segment_info[SegmentIdx].length = length;
            segment_info[SegmentIdx].time = length / speed;
            street_info[temp_info.streetID].length += length;
            
        }
        
    }
    
}

void load_feature_info(){
    struct feature_data data;
    LatLon ll_pt;
    for(int i = 0; i< getNumFeatures(); i++)
    {
        data.area = featureAreaHelper(i);
        data.name = getFeatureName(i);
        data.type = getFeatureType(i);
        data.bound_pts.clear();
        data.rect_bound.clear();
        
        if(data.area == 0){
            data.area_class = 2;
        }
        
        else if(data.area <= 100){
            data.area_class = 5;
        }
        
        else if(data.area <= 1700){
            data.area_class = 4;
        }
        
        else if(data.area <= 7000){
            data.area_class = 3;
        }
        
        else if(data.area <= 24000){
            data.area_class = 2;
        }
        
        else if(data.area <= 74000){
            data.area_class = 1;
        }
        
        else{
            data.area_class = 0;
        }
        
        double min_x = 99999999;
        double max_x = -99999999;
        double min_y = 99999999;
        double max_y = -99999999;
        
        for(int n = 0; n<getNumFeaturePoints(i); n++)
        {
            ll_pt = getFeaturePoint(i,n);
            ezgl::point2d xy_pt(xFromLon(ll_pt.longitude(), map_bounds.avg_lat),
                                yFromLat(ll_pt.latitude()));
            data.bound_pts.push_back(xy_pt);
            
            min_x = std::min(min_x, xy_pt.x);
            max_x = std::max(max_x, xy_pt.x);
            min_y = std::min(min_y, xy_pt.y);
            max_y = std::max(max_y, xy_pt.y);
        }
        
        ezgl::point2d top_l(min_x, max_y); data.rect_bound.push_back(top_l);
        ezgl::point2d top_r(max_x, max_y); data.rect_bound.push_back(top_r);
        ezgl::point2d bot_l(min_x, min_y); data.rect_bound.push_back(bot_l);
        ezgl::point2d bot_r(max_x, min_y); data.rect_bound.push_back(bot_r);
        
        feature_info.push_back(data);
    }
    
    std::sort(feature_info.begin(), feature_info.end(), compareByArea);
    
}

bool compareByArea(const feature_data &a, const feature_data &b){
    
    return a.area > b.area;
    
}

double featureAreaHelper(FeatureIdx feature_id){
    //rule out all the exceptions
    if(!(getFeaturePoint(feature_id,0) == getFeaturePoint(feature_id,getNumFeaturePoints(feature_id)-1)))
        return 0;
    
    //loop through all boundary points and use trapezoid method to find out the total area.
    double result = 0;
    double x1,x2,y1,y2;
    double cosLatAvg = cos(getFeaturePoint(feature_id,0).latitude()*kDegreeToRadian);
    double lowerBound = 0;
    
    //find a proper lower bound for the map to resolve corner cases
    for(int i = 0; i <getNumFeaturePoints(feature_id); i++)
    {
        if(lowerBound < getFeaturePoint(feature_id,i).latitude())
            lowerBound = getFeaturePoint(feature_id,i).latitude();
    }
    
    for(int index = 0; index < getNumFeaturePoints(feature_id)-1;)
    {
        x1 = getFeaturePoint(feature_id,index).longitude();
        x1 = x1*kDegreeToRadian*cosLatAvg*kEarthRadiusInMeters;
        y1 = getFeaturePoint(feature_id,index).latitude()-lowerBound;
        y1 = y1*kDegreeToRadian*kEarthRadiusInMeters;
        index++;
        x2 = getFeaturePoint(feature_id,index).longitude();
        x2 = x2*kDegreeToRadian*cosLatAvg*kEarthRadiusInMeters;
        y2 = getFeaturePoint(feature_id,index).latitude()-lowerBound;
        y2 = y2*kDegreeToRadian*kEarthRadiusInMeters;
        
        result += (x2-x1)*(y2+y1)/2;
    }
    
    return fabs(result);
}

//Unmodified Data Name : ID
void street_name_ID_Map(){ 
    for(int i = 0 ; i < getNumStreets() ;  i++ ){
       nameStreetID.insert(std::make_pair(getStreetName(i),i));
    }
}

//Without space and all lowercase
void street_name_ID_Map_No_Space(){
    for(int i = 0 ; i < getNumStreets() ;  i++ ){
        
        std::string name=getStreetName(i);
        name.erase(std::remove(name.begin(),name.end(),' '),name.end());
        std::transform(name.begin(),name.end(),name.begin(),::tolower);
        nameStreetID_noSpace.insert(std::make_pair(name,i));
    }
}

void load_intersection_info(){
    intersection_info.resize(getNumIntersections());
    nodes.resize(getNumIntersections());
    
    int segmentNumbers;
    for (int i = 0 ; i < getNumIntersections() ; i++){
        
        //initialize nodes
        nodes[i].bestTime = 36000;
        nodes[i].reachingEdge = -1;
        nodes[i].nodeID = i;
        
        // Find several attributes related each intersection
        segmentNumbers=getNumIntersectionStreetSegment(i);
        for(int j = 0 ; j < segmentNumbers ; j++){
            intersection_info[i].streetSegmentIdVector.push_back(getIntersectionStreetSegment(i,j));
            intersection_info[i].streetIdVector.push_back(getStreetSegmentInfo(intersection_info[i].streetSegmentIdVector[j]).streetID);
            intersection_info[i].streetNameVector.push_back(getStreetName(intersection_info[i].streetIdVector[j]));
        }
        
        //Find the adjacent reachable intersections
        //New for loop to look cleaner
        for(int k = 0 ; k < segmentNumbers ;k++){
            IntersectionIdx from,to;
            from=getStreetSegmentInfo(intersection_info[i].streetSegmentIdVector[k]).from;
            to=getStreetSegmentInfo(intersection_info[i].streetSegmentIdVector[k]).to;
            if(from==i){
                intersection_info[i].reachableIntersectionsVector.push_back(to);
                intersection_info[i].outgoingSegments.push_back(intersection_info[i].streetSegmentIdVector[k]);
            }
            if(to==i&&!getStreetSegmentInfo(intersection_info[i].streetSegmentIdVector[k]).oneWay)
            {
                intersection_info[i].reachableIntersectionsVector.push_back(from);
                intersection_info[i].outgoingSegments.push_back(intersection_info[i].streetSegmentIdVector[k]);
            }
            
        }
          
       intersection_info[i].reachableIntersectionsVector=duplication_prevention(intersection_info[i].reachableIntersectionsVector);
       intersection_info[i].outgoingSegments=duplication_prevention(intersection_info[i].outgoingSegments);
       
       //for m2, find the map bounds with intersections
       LatLon pos = getIntersectionPosition(i);
       intersection_info[i].coord = pos;
       
       map_bounds.max_lat = std::max(map_bounds.max_lat, pos.latitude());
       map_bounds.min_lat = std::min(map_bounds.min_lat, pos.latitude());
       map_bounds.max_lon = std::max(map_bounds.max_lon, pos.longitude());
       map_bounds.min_lon = std::min(map_bounds.min_lon, pos.longitude());
       map_bounds.avg_lat = (map_bounds.max_lat + map_bounds.min_lat) / 2;
        
    }
    
    renderer_info.init_zoom_level();
    renderer_info.factors.resize(9);
    highlighted_intersections.first = -1;
    highlighted_intersections.second = -1;
    //std::cout << map_bounds.max_lat << " " << map_bounds.min_lat << std::endl;
    
}

void load_POI_info(){
    struct POI_data data;
    
    for(int i = 0; i<getNumPointsOfInterest(); i++)
    {
        data.ID = i;
        data.type = getPOIType(i);
        data.name = getPOIName(i);
        data.loc = getPOIPosition(i);
        //data.osm = getPOIOSMNodeID(i);
        POI_info.push_back(data);
    }
    
}