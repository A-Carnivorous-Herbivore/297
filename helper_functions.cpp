/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <vector>
#include <iterator>
#include <algorithm>
#include "m1.h"
#include "LatLon.h"
#include "StreetsDatabaseAPI.h"
#include "data_structures.h"
#include "helper_functions.h"
#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"

void lowerToUpper(std::string& temp, int index){ //Self-Written function faster implementation
    if (temp[index]>='a'&&temp[index]<='z'){
       temp[index] = temp[index]-'a'+'A';
    }
}
void upperToLower(std::string& temp, int index){
     if (temp[index]>='A'&&temp[index]<='Z'){
       temp[index] = temp[index]-'A'+'a';
    }
}

std::vector<int> duplication_prevention(std::vector<int> data){
    
    //use algorithms to erase duplicated int within a vector
    std::sort(data.begin(), data.end());
    auto new_end = std::unique(data.begin(), data.end());
    data.erase(new_end, data.end());
    
    return data;
    
}

std::vector<std::string> duplication_prevention_s(std::vector<std::string> data){
    
    //use algorithms to erase duplicated string within a vector
    std::sort(data.begin(), data.end());
    auto new_end = std::unique(data.begin(), data.end());
    data.erase(new_end, data.end());
    
    return data;
    
}

ezgl::point2d cartConv(LatLon coord){
    float a = kEarthRadiusInMeters*coord.longitude()
                *cos(coord.latitude());
    float b = kEarthRadiusInMeters*coord.latitude();
    
    return ezgl::point2d(a,b);
}

std::string lowerCaseNoSpace(std::string str){
    str.erase(std::remove(str.begin(),str.end(),' '),str.end()); //Remove Space
    std::transform(str.begin(),str.end(),str.begin(),::tolower); 
    return str;
}

trigs calculate_angles(LatLon from, LatLon to){
    
    //calculate the angles for displaying street names and arrows
    trigs angles;
    
    double angle = atan2(yFromLat(to.latitude() - from.latitude()), 
            xFromLon(to.longitude() - from.longitude(), map_bounds.avg_lat));
    angles.deg = angle / kDegreeToRadian;
    angles.cos_1 = 1500 * cos(angle);
    angles.cos_2 = 707 * cos(angle + 45 * kDegreeToRadian);
    angles.cos_3 = 707 * cos(angle - 45 * kDegreeToRadian);
    angles.sin_1 = 1500 * sin(angle);
    angles.sin_2 = 707 * sin(angle + 45 * kDegreeToRadian);
    angles.sin_3 = 707 * sin(angle - 45 * kDegreeToRadian);
    
    //the angles of names are -90 <= angle <= 90
    if(angles.deg > 90 && angles.deg <= 180)
        angles.name = angles.deg + 180;
    else if(angles.deg >= -180 && angles.deg < -90)
        angles.name = angles.deg + 180;
    else
        angles.name = angles.deg;
    
    return angles;
    
}

bool pts_in_bound(std::vector<ezgl::point2d> pts, ezgl::rectangle bounds){
    
    bool in_bound = false;
    
    for(int i = 0; i < pts.size(); i++){
        
        if(pts[i].x > bounds.left() ||
           pts[i].x < bounds.right() ||
           pts[i].y > bounds.bottom() ||
           pts[i].y > bounds.top())
        in_bound = true;
        break;
            
    }
    
    //std::cout << in_bound << std::endl;
    return in_bound;
    
}

void draw_arrows(ezgl::renderer *g, ezgl::point2d center, trigs angles, double zoom_factor){
    
    //i can explain the mathematical model in person but basically an arrow consists of three lines
    ezgl::point2d top, bottom, L, R;
    
    top.x = center.x + angles.cos_1 * zoom_factor;
    top.y = center.y + angles.sin_1 * zoom_factor;
    
    bottom.x = center.x - angles.cos_1 * zoom_factor;
    bottom.y = center.y - angles.sin_1 * zoom_factor;
    
    L.x = center.x + angles.cos_2 * zoom_factor;
    L.y = center.y + angles.sin_2 * zoom_factor;
    
    R.x = center.x + angles.cos_3 * zoom_factor;
    R.y = center.y + angles.sin_3 * zoom_factor;
    
    g->draw_line(top, bottom);
    g->draw_line(top, L);
    g->draw_line(top, R);
    
}

std::string name_shortener(std::string streetName){
    
    //i wish a had the time to make this function shorter but this basically shortens north american street names
    if(streetName == "<unknown>")
        return " ";
    
    int i = 0;
    while(1){
        //these are just back lanes
        i = streetName.find("Lane", i);
        if(i == 0) return " ";
        else break;
    }
    
    i = 0;
    while(1){
        i = streetName.find("Ramp", i);
        if(i != std::string::npos) return " ";
        else break;
    }
    
    i = 0;
    while(1){
        i = streetName.find("Collector", i);
        if(i != std::string::npos) return " ";
        else break;
    }
    
    i = 0;
    while(1){
        i = streetName.find("Transitway", i);
        if(i != std::string::npos) return " ";
        else break;
    }
    
    i = 0;
    while(1){
        i = streetName.find("Collectors", i);
        if(i != std::string::npos) return " ";
        else break;
    }
    
    i = 0;
    while(1){
        i = streetName.find("West", i);
        if(i == std::string::npos) break;
        
        streetName.replace(i, 4, "W");
        i += 1;
    }
    
    i = 0;
    while(1){
        i = streetName.find("East", i);
        if(i == std::string::npos) break;
        
        streetName.replace(i, 4, "E");
        i += 1;
    }
    
    i = 0;
    while(1){
        i = streetName.find("South", i);
        if(i == std::string::npos) break;
        
        streetName.replace(i, 5, "S");
        i += 1;
    }
    
    i = 0;
    while(1){
        i = streetName.find("North", i);
        if(i == std::string::npos) break;
        
        streetName.replace(i, 5, "S");
        i += 1;
    }
    
    i = 0;
    while(1){
        i = streetName.find("Saint", i);
        if(i == std::string::npos) break;
        
        streetName.replace(i, 5, "St.");
        i += 2;
    }
    
    i = 0;
    while(1){
        i = streetName.find("Street", i);
        if(i == std::string::npos) break;
        
        streetName.replace(i, 6, "St");
        i += 2;
        return streetName;
    }
    
    i = 0;
    while(1){
        i = streetName.find("Avenue", i);
        if(i == std::string::npos) break;
        
        streetName.replace(i, 6, "Ave");
        i += 3;
        return streetName;
    }
    
    i = 0;
    while(1){
        i = streetName.find("Highway", i);
        if(i == std::string::npos) break;
        
        streetName.replace(i, 7, "Hwy");
        i += 4;
        return streetName;
    }
    
    i = 0;
    while(1){
        i = streetName.find("Parkway", i);
        if(i == std::string::npos) break;
        
        streetName.replace(i, 7, "Pky");
        i += 4;
        return streetName;
    }
    
    i = 0;
    while(1){
        i = streetName.find("Expressway", i);
        if(i == std::string::npos) break;
        
        streetName.replace(i, 10, "Expy");
        i += 4;
        return streetName;
    }
    
    i = 0;
    while(1){
        i = streetName.find("Drive", i);
        if(i == std::string::npos) break;
        
        streetName.replace(i, 5, "Dr");
        i += 2;
        return streetName;
    }
    
    i = 0;
    while(1){
        i = streetName.find("Road", i);
        if(i == std::string::npos) break;
        
        streetName.replace(i, 4, "Rd");
        i += 2;
        return streetName;
    }
    
    i = 0;
    while(1){
        i = streetName.find("Crescent", i);
        if(i == std::string::npos) break;
        
        streetName.replace(i, 8, "Cres");
        i += 4;
        return streetName;
    }
    
    i = 0;
    while(1){
        i = streetName.find("Court", i);
        if(i == std::string::npos) break;
        
        streetName.replace(i, 5, "Ct");
        i += 2;
        return streetName;
    }
    
    i = 0;
    while(1){
        i = streetName.find("Place", i);
        if(i == std::string::npos) break;
        
        streetName.replace(i, 5, "Pl");
        i += 2;
        return streetName;
    }
    
    i = 0;
    while(1){
        i = streetName.find("Lane", i);
        if(i == std::string::npos) break;
        
        streetName.replace(i, 4, "Ln");
        i += 2;
        return streetName;
    }
    
    i = 0;
    while(1){
        i = streetName.find("Boulevard", i);
        if(i == std::string::npos) break;
        
        streetName.replace(i, 9, "Blvd");
        i += 4;
        return streetName;
    }
    
    i = 0;
    while(1){
        i = streetName.find("Circle", i);
        if(i == std::string::npos) break;
        
        streetName.replace(i, 6, "Cir");
        i += 3;
        return streetName;
    }
    
    i = 0;
    while(1){
        i = streetName.find("Terrace", i);
        if(i == std::string::npos) break;
        
        streetName.replace(i, 7, "Ter");
        i += 3;
        return streetName;
    }
    
    return streetName;
    
}

int modify_zoom_level(int ID){
        
    LatLonBounds bounds = findStreetBoundingBox(ID);
    
    double lon = bounds.max.longitude() - bounds.min.longitude();
    double lat = bounds.max.latitude() - bounds.min.latitude();
    
    if(lon >= lat){
    
        double x = xFromLon(lon, map_bounds.avg_lat) / 1000000 / kDegreeToRadian;
    
        return std::floor(- (log(x) / log(1.66667)) + 2.52906);
        
    }
    
    else{
        
        double y = xFromLon(lat, map_bounds.avg_lat) / 1000000 / kDegreeToRadian;
        
        return std::floor(- (log(y) / log(1.66667)) + 2.52906);
        
    }
    
}

int modify_zoom_level_to_segs(void){
    
    LatLonBounds bounds;
    double max_lat = -91;
    double max_lon = -181;
    double min_lat = 91;
    double min_lon = 181;
    
    for(auto it = highlighted_segments.begin(); it != highlighted_segments.end(); ++it){
        
        StreetSegmentInfo info = getStreetSegmentInfo(*it);
        LatLon from = getIntersectionPosition(info.from);
        LatLon to = getIntersectionPosition(info.to);
        
        max_lat = std::max(from.latitude(), max_lat);
        min_lat = std::min(from.latitude(), min_lat);
        max_lon = std::max(from.longitude(), max_lon);
        min_lon = std::min(from.longitude(), min_lon);
        
        max_lat = std::max(to.latitude(), max_lat);
        min_lat = std::min(to.latitude(), min_lat);
        max_lon = std::max(to.longitude(), max_lon);
        min_lon = std::min(to.longitude(), min_lon);
        
    }
    
    LatLon min(min_lat, min_lon);
    LatLon max(max_lat, max_lon);
    bounds.min = min;
    bounds.max = max;
    
    double lon = bounds.max.longitude() - bounds.min.longitude();
    double lat = bounds.max.latitude() - bounds.min.latitude();
    
    if(lon >= lat){
    
        double x = xFromLon(lon, map_bounds.avg_lat) / 1000000 / kDegreeToRadian;
    
        return std::floor(- (log(x) / log(1.66667)) + 2.52906);
        
    }
    
    else{
        
        double y = xFromLon(lat, map_bounds.avg_lat) / 1000000 / kDegreeToRadian;
        
        return std::floor(- (log(y) / log(1.66667)) + 2.52906);
        
    }
    
}

void check_highlighted_intersections(int id){
    
    if(highlighted_intersections.first == -1 && highlighted_intersections.second == -1)
        highlighted_intersections.first = id;
    
    else if(highlighted_intersections.first != -1 && highlighted_intersections.second == -1){
        
        if(highlighted_intersections.first == id)
            highlighted_intersections.first = -1;
        
        else
            highlighted_intersections.second = id;
        
    }
    
    else if(highlighted_intersections.first == -1 && highlighted_intersections.second != -1){
        
        if(highlighted_intersections.second == id)
            highlighted_intersections.second = -1;
        
        else{
            
            highlighted_intersections.first = highlighted_intersections.second;
            highlighted_intersections.second = id;
            
        }
        
    }
    
    else{
        
        if(highlighted_intersections.first == id)
            highlighted_intersections.first = -1;
        
        else if(highlighted_intersections.second == id)
            highlighted_intersections.second = -1;
        
        else{
            
            highlighted_intersections.first = highlighted_intersections.second;
            highlighted_intersections.second = id;
            
        }
        
    }
    highlighted_segments.clear();
}