/* 
 * Copyright 2021 University of Toronto
 *
 * Permission is hereby granted, to use this software and associated 
 * documentation files (the "Software") in course work at the University 
 * of Toronto, or for personal use. Other uses are prohibited, in 
 * particular the distribution of the Software either publicly or to third 
 * parties.
 *
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <iostream>
#include <algorithm>
#include "m1.h"
#include "data_structures.h"
#include "load_functions.h"
#include "helper_functions.h"
#include "StreetsDatabaseAPI.h"
#include "OSMDatabaseAPI.h"


// loadMap will be called with the name of the file that stores the "layer-2"
// map data accessed through StreetsDatabaseAPI: the street and intersection 
// data that is higher-level than the raw OSM data). 
// This file name will always end in ".streets.bin" and you 
// can call loadStreetsDatabaseBIN with this filename to initialize the
// layer 2 (StreetsDatabase) API.
// If you need data from the lower level, layer 1, API that provides raw OSM
// data (nodes, ways, etc.) you will also need to initialize the layer 1 
// OSMDatabaseAPI by calling loadOSMDatabaseBIN. That function needs the 
// name of the ".osm.bin" file that matches your map -- just change 
// ".streets" to ".osm" in the map_streets_database_filename to get the proper
// name.
bool loadMap(std::string map_streets_database_filename) {
    
    //std::cout << "loadMap: " << map_streets_database_filename << std::endl;
    
    std::string OSM_path = map_streets_database_filename;
    while(OSM_path.find("streets") != std::string::npos)
        OSM_path.replace(OSM_path.find("streets"), 7, "osm");
    
    // Load your map related data structures here.
    if(!loadStreetsDatabaseBIN(map_streets_database_filename) ||
            !loadOSMDatabaseBIN(OSM_path)) 
        return false;
    
    load_intersection_info();
    load_OSM_info();
    load_segment_info();
    load_feature_info();
    
    street_name_ID_Map();
    street_name_ID_Map_No_Space();
    load_POI_info();
    
    return true;
    
}

void closeMap() {
    
    //Clean-up your map related data structures here
    segment_info.clear();
    street_info.clear();
    nameStreetID.clear();
    nameStreetID_noSpace.clear();
    feature_info.clear();
    intersection_info.clear();
    POI_info.clear();
    
    seg_idx_by_cls.clear();
    rail_info.clear();
    OSM_info.clear();
    nodes.clear();
    
    highlighted_street = -1;
    highlighted_intersections.first = -1;
    highlighted_intersections.second = -1;
    highlighted_segments.clear();
    
    closeStreetDatabase();
    closeOSMDatabase();
    
}

//Minghao's Work
double findDistanceBetweenTwoPoints(std::pair<LatLon, LatLon> points){
    
    //compute cosine of average latitude first
    double cosLatAvg = cos((points.first.latitude() + points.second.latitude()) / 2 * kDegreeToRadian);
    
    //compute distance b/n two pts within one statement
    return kEarthRadiusInMeters * sqrt((points.first.latitude() - points.second.latitude()) * 
            (points.first.latitude()- points.second.latitude()) +
            (points.first.longitude() - points.second.longitude()) * 
            (points.first.longitude() - points.second.longitude()) * 
            cosLatAvg * cosLatAvg) * 
            kDegreeToRadian;
    
}

double findStreetSegmentLength(StreetSegmentIdx street_segment_id){
    
    return segment_info[street_segment_id].length;
    
}

double findStreetSegmentTravelTime(StreetSegmentIdx street_segment_id){
    
    return segment_info[street_segment_id].time;
    
}

std::vector<IntersectionIdx> findIntersectionsOfStreet(StreetIdx street_id){
    
    return duplication_prevention(street_info[street_id].intersections);
    
}

std::vector<IntersectionIdx> findIntersectionsOfTwoStreets(std::pair<StreetIdx, StreetIdx> street_ids){
    
    //initialize result and erase duplicate data
    std::vector<int> result;
    
    //loop through all intersections of the two streets and see if there's an intersection
    for(std::vector<int>::iterator it_1 = street_info[street_ids.first].intersections.begin();
            it_1 != street_info[street_ids.first].intersections.end(); ++it_1){
        
        for(std::vector<int>::iterator it_2 = street_info[street_ids.second].intersections.begin();
                it_2 != street_info[street_ids.second].intersections.end(); ++it_2){
            
            if(*it_1 == *it_2)
                result.push_back(*it_1);
            
        }
        
    }
    
    return duplication_prevention(result);
    
}

double findStreetLength(StreetIdx street_id){
    
    return street_info[street_id].length;
    
}

LatLonBounds findStreetBoundingBox(StreetIdx street_id){
    
    //initialize results
    LatLonBounds result;
    double max_lat = -91;
    double max_lon = -181;
    double min_lat = 91;
    double min_lon = 181;
    
    //loop through all pts of the street and expand the bounding box
    for(std::vector<LatLon>::iterator it = street_info[street_id].curve_pts.begin();
            it != street_info[street_id].curve_pts.end(); ++it){
        
        max_lat = std::max(max_lat, (*it).latitude());
        min_lat = std::min(min_lat, (*it).latitude());
        max_lon = std::max(max_lon, (*it).longitude());
        min_lon = std::min(min_lon, (*it).longitude());
        
    }
    
    LatLon min(min_lat, min_lon);
    LatLon max(max_lat, max_lon);
    result.min = min;
    result.max = max;
    return result;
    
}
//End of Minghao's work

//Yunfan's work
double findFeatureArea(FeatureIdx feature_id)
{
    return feature_info[feature_id].area;
}

POIIdx findClosestPOI(LatLon my_position, std::string POIname)
{
    // Find all the POI of matching name
    std::map<POIIdx,LatLon> target;
    std::pair<POIIdx,LatLon> data;
    for(int i = 0; i<getNumPointsOfInterest();i++)
    {
        if(POIname.compare(getPOIName(i)) == 0)
        {
            data.first = i;
            data.second = getPOIPosition(i);
            target.insert(data);
        }
    }
    
    // sort out the least distance one
    POIIdx result = target.begin()->first;
    std::pair<LatLon,LatLon> pointsPair(target.begin()->second,my_position);
    double leastDistance = findDistanceBetweenTwoPoints(pointsPair);
    double tempDis;
    
    for(auto it = target.begin();it != target.end();++it)
    {
        pointsPair.first = it->second;
        tempDis = findDistanceBetweenTwoPoints(pointsPair);
        if(leastDistance > tempDis)
        {
            result = it->first;
            leastDistance = tempDis;
        }
    }
    
    return result;
}

IntersectionIdx findClosestIntersection(LatLon my_position)
{
    // lots of intersections to be loaded, so i use a map instead of vector to include all the position info
    std::map<IntersectionIdx,LatLon> target;
    std::pair<IntersectionIdx,LatLon> IntersectionPos;
    for(int i = 0; i<getNumIntersections();i++)
    {
            IntersectionPos.first = i;
            IntersectionPos.second = getIntersectionPosition(i);
            target.insert(IntersectionPos);
    }
    
    // sort out the least distance one using a loop
    IntersectionIdx result = target.begin()->first;
    std::pair<LatLon,LatLon> pointsPair;
    pointsPair.first = target.begin()->second;
    pointsPair.second = my_position;
    double leastDistance = findDistanceBetweenTwoPoints(pointsPair);
    double tempDis;
    
    for(auto it = ++target.begin();it != target.end(); ++it)
    {
        pointsPair.first = it->second;
        tempDis = findDistanceBetweenTwoPoints(pointsPair);
        if(leastDistance > tempDis)
        {
            result = it->first;
            leastDistance = tempDis;
        }
    }
    return result;
}
//End of Yunfan's work

//Rudy's Work

std::vector<StreetIdx> findStreetIdsFromPartialStreetName(std::string street_prefix){
    std::vector<StreetIdx> result;
    std::map<std::string,int>::iterator it;
    if(street_prefix.empty()){
        return result;
    }else{
        street_prefix.erase(std::remove(street_prefix.begin(),street_prefix.end(),' '),street_prefix.end()); //Remove Space
        std::transform(street_prefix.begin(),street_prefix.end(),street_prefix.begin(),::tolower);         //All upper case
        auto itlower=nameStreetID_noSpace.lower_bound(street_prefix);                                   //Find lower bound case
        street_prefix[street_prefix.length()-1]+=1;                 //Add last char one
        auto itlower2=nameStreetID_noSpace.lower_bound(street_prefix);                                   //Find upper bound case


        for(it=itlower;it!=itlower2;++it){
            result.push_back(it->second);
        }

        return result;
    }
}

std::vector <StreetSegmentIdx> findStreetSegmentsOfIntersection(IntersectionIdx intersection_id){

    return intersection_info[intersection_id].streetSegmentIdVector;
    
}

 std::vector<std::string> findStreetNamesOfIntersection(IntersectionIdx intersection_id){
     
     return intersection_info[intersection_id].streetNameVector;
     
}
 
 std::vector<IntersectionIdx> findAdjacentIntersections(IntersectionIdx intersection_id){
    
     return intersection_info[intersection_id].reachableIntersectionsVector;
     
 }
 
 //End of Rudy's Work