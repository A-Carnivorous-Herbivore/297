/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "graphics_functions.h"
#include "point.hpp"
#include "graphics.hpp"
#include "UI_functions.h"
#include "data_structures.h"
#include "helper_functions.h"

void draw_main_canvas(ezgl::renderer *g){
    
    //divide the visible world into 16 rectangles to prevent cluster
    nine_rects.clear();
    drawn_POI.clear();
    div_rects(g);
    g->set_line_cap(ezgl::line_cap::round);
    
    renderer_info.update_factors();
    draw_features(g);
    
    if(renderer_info.curr_zoom > 3)
        draw_rail(g);
    
    draw_streets(g);
    
    draw_poi(g);
    
    draw_intersections(g); // only draw a intersection as a red square if it is highlighted
    
    ezgl::rectangle world = g->get_visible_world();
        
    if(renderer_info.curr_zoom == 0){
        world_w = world.width();
        world_h = world.height();
    }

}

void draw_streets(ezgl::renderer *g){
    
    //decide on the level of detail
    int class_bound;
    
    if(renderer_info.curr_zoom < 1){
        class_bound = 0;
    }
    
    else{
        
        switch(renderer_info.curr_zoom){
            
            case 1:  class_bound = 1;  break;
            case 2:  class_bound = 4;  break;
            case 3:  class_bound = 6;  break;
            case 4:  class_bound = 8;  break;
            case 5:  class_bound = 9;  break;
            default: class_bound = 10;
            
        }
        
    }
    
    //the default color for higher class street segments
    g->set_line_dash(ezgl::line_dash::none);
    g->set_line_cap(ezgl::line_cap::round);
    
    //only loop through classes to be drawn
    for(int hwy_class = class_bound; hwy_class >= 0; hwy_class--){
        
        //the colors and width of different classes of street segments
        set_seg_colour(g, hwy_class);
        
        //loop through all the street segments of the class
        for(auto it = seg_idx_by_cls[hwy_class].begin(); 
            it < seg_idx_by_cls[hwy_class].end(); it++){
            
            //draw the street if in bound
            if(pts_in_bound(segment_info[*it].pts, g->get_visible_world()))
                draw_curve_points(g, *it);
            
        }
        
    }
    
    //draw highlighted street
    if(highlighted_street >= 0){
        
        g->set_line_width(3.5 * renderer_info.factors[0]);
        g->set_color(ezgl::LIGHT_SKY_BLUE);
        
        for(int i = 0; i < street_info[highlighted_street].segments.size(); i++){
            
            draw_curve_points(g, street_info[highlighted_street].segments[i]);
            
        }
        
    }
    
    //draw highlighted path
    if(highlighted_segments.size() != 0)
    {
        g->set_line_width(3.5 * renderer_info.factors[0]);
        g->set_color(ezgl::PURPLE);
        
        for(int i = 0; i < highlighted_segments.size(); i++)
        {
            draw_curve_points(g, highlighted_segments[i]);
        }
        
    }
    
    //set the color and font for street names
    int name_lim = 1;
    g->set_color(57, 55, 52, 255);
    g->set_font_size(renderer_info.factors[5]);
    
    if(renderer_info.curr_zoom >= 8) name_lim = 2;
    
    //follow the same manner as street segments
    for(int hwy_class = class_bound; renderer_info.curr_zoom >= 3 && hwy_class >= 0; hwy_class--){
        
        for(auto it = seg_idx_by_cls[hwy_class].begin(); 
            it < seg_idx_by_cls[hwy_class].end(); it++){
            
            if(pts_in_bound(segment_info[*it].pts, g->get_visible_world())){
                
                for(int points = 0; points < segment_info[*it].pts.size() - 1; points++){
                    
                    if(points % 3 == 2){
                        
                        int name_idx = check_rects(segment_info[*it].name[points], 1);
                    
                        if(nine_rects[name_idx].name_count > name_lim) continue;

                        (nine_rects[name_idx].valid_name)++;
                        
                        if(nine_rects[name_idx].valid_name % 4 == 0){

                            g->set_text_rotation(segment_info[*it].angles[points].name);
                            g->draw_text(segment_info[*it].name[points],
                                    name_shortener(street_info[segment_info[*it].streetID].name));

                        }
                    
                    }
                    
                }
                
            }
            
        }
        
    }
    
    if(renderer_info.curr_zoom < 5) return;
    
    int class_bound_2;
    
    if(renderer_info.curr_zoom == 5){
        class_bound_2 = 4;
    }
    
    else{
        
        switch(renderer_info.curr_zoom){
            
            case 6:  class_bound_2 = 6;   break;
            case 7:  class_bound_2 = 7;   break;
            case 8:  class_bound_2 = 8;   break;
            case 9:  class_bound_2 = 9;   break;
            default: class_bound_2 = 9;
            
        }
        
    }
    
    //set the color and line width for arrows
    g->set_line_width(renderer_info.factors[1]);
    g->set_color(87, 84, 81, 255);
    
    //follow the same manner as the street segments
    for(int hwy_class = class_bound_2; hwy_class >= 0; hwy_class--){
        
        for(auto it = seg_idx_by_cls[hwy_class].begin(); 
            it < seg_idx_by_cls[hwy_class].end(); it++){
            
            if(!pts_in_bound(segment_info[*it].pts, g->get_visible_world()))
                continue;
            
            for(int points = 0; points < segment_info[*it].pts.size() - 1; points++){
                
                if(points % 7 == 0){
                        
                    int arr_idx = check_rects(segment_info[*it].arr[points], 2);
                    
                    if(nine_rects[arr_idx].arrow_count > 1) continue;

                    (nine_rects[arr_idx].valid_arrow)++;
                        
                    if(nine_rects[arr_idx].valid_arrow % 8 == 0){

                        draw_arrows(g, segment_info[*it].arr[points], 
                                segment_info[*it].angles[points], renderer_info.factors[4]);

                    }
                    
                }
                
            }
            
        }
        
    }

}

void draw_curve_points(ezgl::renderer *g, int streetSegmentIdx){
    
    //loop through the points of the street segment
    for(int points = 0; points < segment_info[streetSegmentIdx].pts.size() - 1; points++){
            
        g->draw_line({segment_info[streetSegmentIdx].pts[points].x, 
                segment_info[streetSegmentIdx].pts[points].y}, 
                {segment_info[streetSegmentIdx].pts[points + 1].x, 
                        segment_info[streetSegmentIdx].pts[points + 1].y});
                                   
    }
    
}

void set_seg_colour(ezgl::renderer *g, int cls){
    
    switch(cls){
            case 10: g->set_line_width(1 * renderer_info.factors[0]);
                     g->set_color(245, 243, 240, 255);                break;
            case 9:  g->set_line_width(2 * renderer_info.factors[0]);
                     g->set_color(245, 243, 240, 255);                break;
            
            case 8:  g->set_line_width(3 * renderer_info.factors[3]);
                     g->set_color(245, 243, 240, 255);                break;
            case 7:  g->set_line_width(2 * renderer_info.factors[0]);
                     g->set_color(245, 243, 240, 255);                break;
            
            case 6:  g->set_line_width(3 * renderer_info.factors[2]);
                     g->set_color(245, 243, 240, 255);                break;
            case 5:  g->set_line_width(2 * renderer_info.factors[0]);
                     g->set_color(253, 215, 161, 255);                break;
            
            case 4:  g->set_line_width(3 * renderer_info.factors[1]);
                     g->set_color(253, 215, 161, 255);                break;
            case 3:  g->set_line_width(2 * renderer_info.factors[0]);
                     g->set_color(251, 178, 154, 255);                break;
            case 2:  g->set_line_width(2 * renderer_info.factors[0]);
                     g->set_color(233, 144, 160, 255);                break;
            case 1:  g->set_line_width(3 * renderer_info.factors[1]);
                     g->set_color(251, 178, 154, 255);                break;
            default: g->set_line_width(3 * renderer_info.factors[0]);
                     g->set_color(233, 144, 160, 255);
        }
    
}

void div_rects(ezgl::renderer *g){
    
    //use the top left corner to get a 4 * 4 world
    ezgl::point2d top_l = g->get_visible_world().top_left();
    double width_inc = (g->get_visible_world().width()) / 4;
    double height_dec = (g->get_visible_world().height()) / 4;
    
    for(int i = 0; i <= 3; i++){
        
        for(int j = 1; j <= 4; j++){
            
            div_rect curr;
            ezgl::point2d bot_l(top_l.x + i * width_inc, top_l.y - j * height_dec);
            curr.rect = ezgl::rectangle(bot_l, width_inc, height_dec);
            nine_rects.push_back(curr);
            
        }
        
    }
    
}

int check_rects(ezgl::point2d pt, int type){
    
    for(int i = 0; i < 16; i++){
        
        if(nine_rects[i].rect.contains(pt)){
            
            switch(type){
                
                case 0:  (nine_rects[i].POI_count)++;   break;
                case 1:  (nine_rects[i].name_count)++;  break;
                case 2:  (nine_rects[i].arrow_count)++; break;
                default: (nine_rects[i].POI_count)++;
                
            }
            return i;
            
        }
        
    }
    
    return 0;
    
}

void draw_rail(ezgl::renderer *g){
    
    g->set_line_cap(ezgl::line_cap::butt);
    
    //draw beautiful dashed railways
    for(int i = 0; i < rail_info.size(); i++){
        
        g->set_color(91, 75, 63, 255); 
        g->set_line_width(3);
        g->set_line_dash(ezgl::line_dash::none);
        for(int points = 0; points < rail_info[i].size() - 1; points++){

            g->draw_line({rail_info[i][points].x, 
                                    rail_info[i][points].y}, 
                                   {rail_info[i][points + 1].x, 
                                    rail_info[i][points + 1].y});

        }
        
        g->set_color(245, 243, 240, 255); 
        g->set_line_width(2);
        g->set_line_dash(ezgl::line_dash::symmetric_1_1);
        for(int points = 0; points < rail_info[i].size() - 1; points++){

            g->draw_line({rail_info[i][points].x, 
                                    rail_info[i][points].y}, 
                                   {rail_info[i][points + 1].x, 
                                    rail_info[i][points + 1].y});

        }
        
    }
    
}

void draw_features(ezgl::renderer *g){
    
    int class_bound = 5;
    g->set_line_width(renderer_info.factors[0]);
    g->set_line_dash(ezgl::line_dash::none);
    
    if(renderer_info.curr_zoom <= 1) class_bound = 0;
    
    else if(renderer_info.curr_zoom <= 3) class_bound = 1;
    
    else if(renderer_info.curr_zoom <= 5) class_bound = 2;
    
    else if(renderer_info.curr_zoom <= 7) class_bound = 3;
    
    for(auto it = feature_info.begin();it != feature_info.end(); it++) {
        
        //prevent unnecessary loop iterations
        if((*it).area_class > class_bound || !pts_in_bound((*it).rect_bound, g->get_visible_world())) continue;
        
        switch(it->type)
        {
            case PARK:       g->set_color(192,232,174); 
                        break;
            case BEACH:      g->set_color(249,245,237); 
                        break;
            case LAKE:       g->set_color(174,225,245); 
                        break;
            case RIVER:      g->set_color(174,225,245);
                        break;
            case ISLAND:     g->set_color(235,235,210); 
                        break;
            case BUILDING:   g->set_color(222,219,213); 
                        break;
            case GREENSPACE: g->set_color(215,237,204); 
                        break;
            case GOLFCOURSE: g->set_color(210,237,187); 
                        break;
            case STREAM:     g->set_color(174,225,245);
                        break;
            default:         g->set_color(175,175,175);       
        }   // color scheme
        if(it->area != 0)
            g->fill_poly(it->bound_pts);    // draw the feature if it is large enough on the screen
        else
        {
            for(auto p_it = it->bound_pts.begin();
                    p_it != --(it->bound_pts.end());)
            {
                g->draw_line(*p_it,*(++p_it));
            }
        }
    }
}

void draw_poi(ezgl::renderer *g){
    
    g->set_color(249, 152, 68);
    g->set_text_rotation(0);
    g->set_font_size(renderer_info.factors[6]);
    g->set_vert_text_just(ezgl::text_just::bottom);
    
    for(auto it = POI_info.begin(); it != POI_info.end(); it++)
    {
        if(renderer_info.curr_zoom < 6) // draw icon only if zoomed in enough
        break;
        
        ezgl::point2d location(xFromLon(it->loc.longitude(), map_bounds.avg_lat),
                                yFromLat(it->loc.latitude()));
        
        if(!g->get_visible_world().contains(location)) // only draw icon in visible world
            continue;
        
        int rect_idx = check_rects(location, 0);
        if(nine_rects[rect_idx].POI_count > 1)
            continue;
        
        ezgl::surface* point;
        
        if(it->type.compare("cinema") == 0)
        {
            point = g->load_png("libstreetmap/resources/movie-2-line.png");
            g->draw_surface(point,location);
        }
        else if(it->type.compare("hospital") == 0)
        {
            point = g->load_png("libstreetmap/resources/hospital-fill.png");
            g->draw_surface(point,location);
        }
        else if(it->type.find("fuel") != std::string::npos)
        {   
            
                point = g->load_png("libstreetmap/resources/gas-station-fill.png");
                g->draw_surface(point,location);
                
        }
        else if(it->type.compare("food_court") == 0)
        {
            point = g->load_png("libstreetmap/resources/shopping-bag-fill.png");
            g->draw_surface(point,location);
        }else if(it->type.compare("restaurant") == 0)
        {
            
            point = g->load_png("libstreetmap/resources/restaurant-fill.png");
            g->draw_surface(point,location);
            
        }else if(it->type.compare("bank") == 0)
        {
            point = g->load_png("libstreetmap/resources/bank-line.png");
            g->draw_surface(point,location);
            
        }else if( it->type.compare("library") == 0){
            point = g->load_png("libstreetmap/resources/book-2-fill.png");
            g->draw_surface(point,location);
        }else{
            g->fill_arc(location, renderer_info.factors[8], 0, 360);
        }
        
        drawn_POI.push_back(it->ID);
        
    }
    
    g->set_color(87, 84, 81);
    
    for(auto it = drawn_POI.begin(); it != drawn_POI.end(); it++)
    {
        
        if(renderer_info.curr_zoom < 7) break;
        
        ezgl::point2d location(xFromLon(POI_info[*it].loc.longitude(), map_bounds.avg_lat),
                                yFromLat(POI_info[*it].loc.latitude()));
        g->draw_text(location, POI_info[*it].name);
        
    }
    
}

void draw_searchBox(ezgl::renderer *g){
    
    g->set_coordinate_system(ezgl::SCREEN);
    g->fill_rectangle({g->get_visible_screen().right()-40,g->get_visible_screen().top()}, 
                      {g->get_visible_screen().right(),g->get_visible_screen().top()-40});
   
}

void draw_intersections(ezgl::renderer *g){                             //Draw the intersections

    g->set_color(ezgl::LIGHT_SKY_BLUE);
    
    if(highlighted_intersections.first >= 0){
        
        LatLon coord = getIntersectionPosition(highlighted_intersections.first);
        ezgl::point2d location(xFromLon(coord.longitude(), map_bounds.avg_lat), yFromLat(coord.latitude()));
        g->fill_arc(location, renderer_info.factors[7], 0, 360);
        
    }
    
    if(highlighted_intersections.second >= 0){
        
        LatLon coord = getIntersectionPosition(highlighted_intersections.second);
        ezgl::point2d location(xFromLon(coord.longitude(), map_bounds.avg_lat), yFromLat(coord.latitude()));
        g->fill_arc(location, renderer_info.factors[7], 0, 360);
        
    }
    
    if(highlighted_intersections.first != -1 && highlighted_intersections.second != -1)
    {
        ezgl::surface* point;
        LatLon coord = getIntersectionPosition(highlighted_intersections.first);

        ezgl::rectangle screenCoord= g->get_visible_screen();
        double screenX = screenCoord.m_second.x - screenCoord.m_first.x;
        //std::cout<<screenX<<std::endl;
        double screenY = screenCoord.m_second.y - screenCoord.m_first.y;
        ezgl::rectangle worldCoord= g->get_visible_world();
        double worldX = worldCoord.m_second.x - worldCoord.m_first.x;
        double worldY = worldCoord.m_second.y - worldCoord.m_first.y;
        //std::cout<<worldX<<std::endl;
        ezgl::point2d locationOne(xFromLon(coord.longitude(), map_bounds.avg_lat)-(15/screenX)*worldX, yFromLat(coord.latitude())+(11/screenY)*worldY);
        point = g->load_png("libstreetmap/resources/send-plane-line.png");
        g->draw_surface(point,locationOne);
        coord = getIntersectionPosition(highlighted_intersections.second);
        ezgl::point2d locationTwo(xFromLon(coord.longitude(), map_bounds.avg_lat)-(12/screenX)*worldX, yFromLat(coord.latitude())+(24/screenY)*worldY);
        point = g->load_png("libstreetmap/resources/map-pin-fill.png");
        g->draw_surface(point,locationTwo);
    }
    
}

float xFromLon(float lon, float latAvg){
    return lon * kDegreeToRadian * kEarthRadiusInMeters 
            * cos(latAvg * kDegreeToRadian);
}

float yFromLat(float lat){
    return kEarthRadiusInMeters * lat * kDegreeToRadian;
}

void initial_setUp(ezgl::application *application,bool ){               //Set up all callbacks
    
    application->create_button("Search",6,search_button);

    GObject* map_select = application->get_object("map_select");
    g_signal_connect(map_select,
            "changed",
            G_CALLBACK(map_selection_box),
            application
    );
    
    GObject* mode_select = application->get_object("search_mode");
    g_signal_connect(mode_select,
            "changed",
            G_CALLBACK(mode_selection_box),
            application
    );
    
    application->create_button("Find path", 7, find_path_button);
    application->create_button("Clear", 8, clear_button);

    GObject* help = application->get_object("HelpButton");
    g_signal_connect(help,"clicked",G_CALLBACK(help_button),application);
    
    GObject* firstBox = application->get_object("TextInput");
    g_signal_connect(firstBox,"changed",G_CALLBACK(act_on_first_box),application);
    GObject* secondBox = application->get_object("TextInput2");
    g_signal_connect(secondBox,"changed",G_CALLBACK(act_on_second_box),application);
//    GObject* thirdBox = application->get_object("TextInput3");
//    g_signal_connect(thirdBox,"changed",G_CALLBACK(act_on_third_box),application);
//    GObject* fourthBox = application->get_object("TextInput4");
//    g_signal_connect(fourthBox,"changed",G_CALLBACK(act_on_fourth_box),application);
    
}