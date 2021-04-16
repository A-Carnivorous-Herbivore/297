/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "UI_functions.h"
#include "data_structures.h"
#include "helper_functions.h"
#include "ezgl/control.hpp"

double mouseScreenCoordinateX,mouseScreenCoordinateY,lastScreenClickX,lastScreenClickY;
std::string keyPressed1;
std::string keyPressed2;
//IntersectionIdx last_click = -1;
int map_index = 0;
int help_dialog = 0;
std::string indicator1;
int indicator=1;
std::string s1;

float lonFromX(float x,float latAvg){                   //Convert Coordinate Value
    return x / ( kDegreeToRadian * kEarthRadiusInMeters 
            * cos( latAvg*kDegreeToRadian ) );
}

float latFromY(float y){
    return y/ ( kEarthRadiusInMeters * kDegreeToRadian );
}

void act_on_mouse_click(ezgl::application* app,         //Mouse click gives coordinates
                        GdkEventButton* event,
                        double x,double y)
{
    std::cout << "Mouse clicked at (" << x << ", " << y << ")\n";
//    lastScreenClickX = event -> x;
//    lastScreenClickY = event -> y;
//    std::cout << lastScreenClickX << std::endl << lastScreenClickY; // get mouse click Coordinate
    
    LatLon coord = LatLon(latFromY(y),lonFromX(x,map_bounds.avg_lat));
    int id = findClosestIntersection(coord);
    std::cout << "Closest Intersection: " << getIntersectionName(id) << std::endl;
    app->update_message("Closest Intersection: " + getIntersectionName(id));   //find closest intersection for highlight
   
    check_highlighted_intersections(id);
    
//    last_click = id;    // use a global var to toggle the highlight
    
    app->refresh_drawing();
}
/*
void act_on_mouse_move(ezgl::application* app,
                        GdkEventButton* event,
                        double x,double y){
    
    //app->update_message("Mouse Move");
}
  */
void act_on_first_box(GtkEntry*,ezgl::application* app,char*){      //Call back function on first search entry
    GtkEntry* text1 = (GtkEntry*) app -> get_object("TextInput");
    std::string inputText1 = gtk_entry_get_text(text1);
    app->update_message(prediction(inputText1));
}

void act_on_second_box(GtkEntry*,ezgl::application* app,char*){     //Same but on second
    GtkEntry* text2 = (GtkEntry*) app->get_object("TextInput2");
    std::string inputText2 = gtk_entry_get_text(text2);
    app->update_message(prediction(inputText2));
}

void act_on_key_press(ezgl::application*,
                        GdkEventKey*,
                        char*){
    /*GtkEntry* text1=(GtkEntry*) app->get_object("TextInput");
    std::string inputText1=gtk_entry_get_text(text1);
    std::string temp(key_name);*/
    //std::cout<<key_name<<indicator<<std::endl;
    //GObject* firstBox=app->get_object("TextInput");
    //std::cout<<key_name<<std::endl;
    //std::cout<<inputText1<<std::endl;
    //app->update_message(inputText1);
    //g_signal_connect(firstBox,"activate",G_CALLBACK(act_on_first_box),app);
    //if(inputText1!=indicator1){
      /*  if(validKeyPress(key_name)){
            if (temp=="BackSpace"&&!keyPressed1.empty()){
                keyPressed1.erase(keyPressed1.size()-1);
                indicator1.erase(indicator1.size()-1);
            }else if(temp=="BackSpace"&&keyPressed1.empty()){
                //Do nothing no change
            }else if(temp=="space"){
                keyPressed1+=" ";
                indicator1+=" ";
            }else if(temp=="apostrophe"){
                keyPressed1+="'";
                indicator1+="'";
            }else{
                keyPressed1+=temp;
                indicator1+=temp;
            }
        
            temp=prediction(keyPressed1);
            app->update_message(temp);
        }else{
            if(validKeyPress(key_name)){
                if (temp=="BackSpace"&&!keyPressed2.empty()){
                    keyPressed2.erase(keyPressed2.size()-1);
                    indicator2.erase(indicator2.size()-1);
                }else if(temp=="BackSpace"&&keyPressed2.empty()){
                    //Do nothing no change
                }else if(temp=="space"){
                    keyPressed2+=" ";
                    indicator2+=" ";
                }else if(temp=="apostrophe"){
                    keyPressed2+="'";
                    indicator2+="'";
                }else{
                    keyPressed2+=temp;
                    indicator2+=temp;
                }
            }
        temp=prediction(keyPressed2);
        app->update_message(temp);
        }*/
    
    
    
}
 
bool validKeyPress(char* key_name){                                 // Check keypress valid 
    std::string temp(key_name);
    if(temp == "a"||temp == "b"||temp == "c"||temp == "d"||temp == "e"||
            temp == "f"||temp == "g"||temp == "h"||temp == "i"||
            temp == "j"||temp == "k"||temp == "l"||temp == "m"||temp == "n"||
            temp == "o"||temp == "p"||temp == "q"||temp == "r"||temp == "s"||
            temp == "t"||temp == "u"||temp == "v"||temp == "w"||temp == "x"||
            temp == "y"||temp == "z"||temp == "A"||temp == "B"||temp == "C"||temp == "D"||temp == "E"||
            temp == "F"||temp == "G"||temp == "H"||temp == "I"||
            temp == "J"||temp == "K"||temp == "L"||temp == "M"||temp == "N"||
            temp == "O"||temp == "P"||temp == "Q"||temp == "R"||temp == "S"||
            temp == "T"||temp == "U"||temp == "V"||temp == "W"||temp == "X"||
            temp == "Y"||temp == "Z"||temp == "1"||temp == "2"||temp == "3"||temp == "4"||temp == "5"||
            temp == "6"||temp == "7"||temp == "8"||temp == "9"||
            temp == "0"||temp == "apostrophe"||temp == "semicolon"||temp == "space"||temp == "BackSpace"||temp == "'"){
        return true;
    }
    return false;
}

void search_button(GtkWidget *,ezgl::application* application){             //Two Search buttons algorithm
    std::string initialText;
    std::string outputText;
    std::vector<int> id1;
    std::vector<int> id2;
    std::vector<int> intersections;
    //std::pair<StreetIdx, StreetIdx> street_ids;
    //int streetNum,streetID1 = -1,streetID2 = -1;
    GtkEntry* text1 = (GtkEntry*) application -> get_object("TextInput");
    std::string inputText1 = gtk_entry_get_text(text1);
    GtkEntry* text2 = (GtkEntry*) application -> get_object("TextInput2");
    std::string inputText2 = gtk_entry_get_text(text2);
    std::string output;
    if(inputText1.empty()&&inputText2.empty()){                  //CASE 1 suggest user to input
            application->update_message("Enter a searching point.");
            std::cout<<"Enter a searching point."<<std::endl;
        }else if( inputText2.empty() && !inputText1.empty() ){      //CASE 2 only first search box has entry
            //int index;
            //bool onlyOne = false;
            initialText = "Searching for ";
            outputText = initialText+inputText1+" ...";
            application -> update_message(outputText);
            id1 = findStreetIdsFromPartialStreetName(inputText1);
            /*
            inputText1.erase(std::remove(inputText1.begin(),inputText1.end(),' '),inputText1.end());
            std::transform(inputText1.begin(),inputText1.end(),inputText1.begin(),::tolower);
            for(int i = 0 ; i < id1.size(); i++){
                std::string temp=getStreetName(id1[i]);
                temp.erase(std::remove(temp.begin(),temp.end(),' '),temp.end());
                std::transform(temp.begin(),temp.end(),temp.begin(),::tolower);
                if(temp == inputText1){
                    onlyOne = true;
                    index = i;
                    break;
                }
            }*/
            if (id1.size() == 0){                                                                     //DNE
                application -> update_message("No such place is suggested.");
                std::cout<<"No such place is suggested."<<std::endl;
            }else{                                                                      //Navigate
                
                highlighted_street = id1[0];
                
                gtk_entry_set_text(text1,street_info[id1[0]].name.c_str());
                
                application-> update_message(getStreetName(id1[0]) + " is being navigated.");
                
                LatLonBounds bounds = findStreetBoundingBox(id1[0]);
                LatLon bottom_l = bounds.min;
                LatLon top_r = bounds.max;
                double center_lat = (bottom_l.latitude() + top_r.latitude()) / 2;
                double center_lon = (bottom_l.longitude() + top_r.longitude()) / 2;
                double center_y = yFromLat(center_lat);
                double center_x = xFromLon(center_lon, map_bounds.avg_lat);
                double w = world_w / 2;
                double h = world_h / 2;
                renderer_info.curr_zoom = renderer_info.init_zoom;
                
                ezgl::point2d min(center_x - w, center_y - h);
                ezgl::point2d max(center_x + w, center_y + h);
                
                application -> get_renderer() -> set_visible_world({min, max});
                
                zoom_to_level(modify_zoom_level(id1[0]), application);
                
            }/*else{                                                                        //Give street name suggestions
                output="Do you mean ";  
                int size=id1.size();
                int length=std::min(8,size);
                for( int i=0 ; i < length ; i++){
                    std::string temp = getStreetName(id1[i]);
                    std::cout << temp << std::endl;
                    output += temp;
                    output += " / ";
                }
                output+="...";
                application->update_message(output);
                LatLon coord = getIntersectionPosition(street_info[id1[0]].intersections[0]);
                    ezgl::point2d pos(xFromLon(coord.longitude(),map_bounds.avg_lat)
                    ,yFromLat(coord.latitude()));
                    application->get_renderer()->set_visible_world(
                    {{pos.x-2000,pos.y-2000},{pos.x+2000,pos.y+2000}});*/
            //}
        }else if(!inputText2.empty()&&inputText1.empty()){      //CASE 3 if only second search box has entry
            //bool onlyOne = false;
            //int index;
            initialText = "Searching for ";
            outputText = initialText + inputText2 + " ...";
            application -> update_message(outputText);
            id2 = findStreetIdsFromPartialStreetName(inputText2);
            /*
            inputText2.erase( std::remove(inputText2.begin(),inputText2.end(),' '),inputText2.end() );
            std::transform( inputText2.begin(),inputText2.end(),inputText2.begin(),::tolower );
            for(int i = 0 ; i < id2.size() ; i++){
                std::string temp = getStreetName(id2[i]);
                temp.erase( std::remove(temp.begin(),temp.end(),' '),temp.end() );
                std::transform( temp.begin(),temp.end(),temp.begin(),::tolower );
                if(temp == inputText2){
                    onlyOne = true;
                    index = i;
                    break;
                }
            }
             */ 
            if (id2.size() == 0){                                                                       //DNE
                application -> update_message("No such place is suggested.");
                std::cout<<"No such place is suggested."<<std::endl;
            }else{                                                                      
                
                highlighted_street = id2[0];
                
                gtk_entry_set_text(text2,street_info[id2[0]].name.c_str());
                
                application -> update_message(getStreetName(id2[0]) + " is being navigated.");            //Navigate
                LatLonBounds bounds = findStreetBoundingBox(id2[0]);
                LatLon bottom_l = bounds.min;
                LatLon top_r = bounds.max;
                double center_lat = (bottom_l.latitude() + top_r.latitude()) / 2;
                double center_lon = (bottom_l.longitude() + top_r.longitude()) / 2;
                double center_y = yFromLat(center_lat);
                double center_x = xFromLon(center_lon, map_bounds.avg_lat);
                double w = world_w / 2;
                double h = world_h / 2;
                renderer_info.curr_zoom = renderer_info.init_zoom;
                
                ezgl::point2d min(center_x - w, center_y - h);
                ezgl::point2d max(center_x + w, center_y + h);
                
                application -> get_renderer() -> set_visible_world({min, max});
                
                zoom_to_level(modify_zoom_level(id2[0]), application);
                
            }/*else{                                                                                    //Give Street Names Suggestions
                output = "Do you mean ";
                int size = id2.size();
                int length = std::min(8,size);
                for( int i = 0 ; i < length ; i++ ){
                    std::string temp = getStreetName(id2[i]);
                    std::cout << temp << std::endl;
                    output += temp;
                    output += " / ";
                }
                 output += "...";
                application->update_message(output);
            }*/
        }else{                                                //CASE 4 both search boxes have entry
            
            //streetNum = getNumStreets();
            std::string fixedInput1,fixedInput2,fixedDataName;  //To remove space and lowercase
            fixedInput1 = inputText1; fixedInput2 = inputText2;
            fixedInput1 = lowerCaseNoSpace(fixedInput1);
            fixedInput2 = lowerCaseNoSpace(fixedInput2); 
            
            id1 = findStreetIdsFromPartialStreetName(inputText1);                                 //If possible, give suggestions
            id2 = findStreetIdsFromPartialStreetName(inputText2);
            
            if( id1.size() != 0 && id2.size() != 0 ){
                    // do nothing
            }
            else if(id1.size() == 0 && id2.size() != 0 ){                                             //Different cases of possible outcomes
                    std::string temp = "No such possible first street name. Re-enter first input.";
                    application -> update_message(temp);
                    std::cout << "No such possible first street name. Re-enter first input." << std::endl;
                    return;
            }
            else if(id1.size() !=0 && id2.size() == 0 ){
                    std::string temp = "No such possible second street name. Re-enter second input.";
                    application -> update_message(temp);
                    std::cout << "No such possible second street name. Re-enter second input." << std::endl;
                    return;
            }
            else{
                    std::string temp = "No such possible both street names. Re-enter both inputs.";
                    application -> update_message(temp);
                    std::cout << "No such possible both street names. Re-enter both inputs." << std::endl;
                    return;
            }
            /*
            for( int i = 0 ; i < streetNum ; i++ ){
                if(streetID1 != -1 && streetID2 != -1) break;
                fixedDataName = lowerCaseNoSpace(getStreetName(i));
                if(fixedDataName == fixedInput1){
                    streetID1 = i;
                }
                if(fixedDataName == fixedInput2){
                    streetID2 = i;
                }
            }
            if(streetID1 == streetID2 && streetID1 == -1){                                                //If both are not found
                id1 = findStreetIdsFromPartialStreetName(inputText1);                                 //If possible, give suggestions
                id2 = findStreetIdsFromPartialStreetName(inputText2);
                if( id1.size() != 0 && id2.size() != 0 ){
                    std::string temp = "No such street name, Do you mean "
                            + getStreetName(id1[0]) + " and " + getStreetName(id2[0]) ;
                    application -> update_message(temp);
                    std::cout << "Found no intersections." << std::endl;
                    intersections=findIntersectionsOfTwoStreets({id1[0],id2[0]});
                    for(int i = 0 ; i < intersections.size(); i++){
                        intersection_info[intersections[i]].highlight = true;
                    }
                }else if(id1.size() == 0 && id2.size() != 0 ){                                             //Different cases of possible outcomes
                    std::string temp = "No such possible first street name. Re-enter first input.";
                    application -> update_message(temp);
                    std::cout << "No such possible first street name. Re-enter first input." << std::endl;
                }else if(id1.size() !=0 && id2.size() == 0 ){
                    std::string temp = "No such possible second street name. Re-enter second input.";
                    application -> update_message(temp);
                    std::cout << "No such possible second street name. Re-enter second input." << std::endl;
                }else{
                    std::string temp = "No such possible both street names. Re-enter both inputs.";
                    application -> update_message(temp);
                    std::cout << "No such possible both street names. Re-enter both inputs." << std::endl;
                }
            }
            else if( streetID1 == -1 && streetID2 != -1 ){
                id1 = findStreetIdsFromPartialStreetName(inputText1);
                if(id1.size() != 0){
                    std::string temp = "No such first street name; Do you mean "
                            + getStreetName(id1[0]) + " and " + getStreetName(streetID2);
                    application -> update_message(temp);
                    std::cout<<"No such intersection is suggested."<<std::endl;
                    intersections=findIntersectionsOfTwoStreets({id1[0],streetID2});
                    for(int i = 0 ; i < intersections.size(); i++){
                        intersection_info[intersections[i]].highlight = true;
                    }
                }else{
                     std::string temp="No such possible first street name. Re-enter first input.";
                    application->update_message(temp);
                    std::cout<<"No such possible second street name. Re-enter first input."<<std::endl;
                }
            }
            else if(streetID1 != -1 && streetID2 == -1){                                              //Case of first makes sense but second does not
                id2=findStreetIdsFromPartialStreetName(inputText2);
                if(id2.size()!=0){
                    std::string temp="No such second street name; Do you mean "
                            +getStreetName(streetID1) + " and " + getStreetName(id2[0]);
                    application -> update_message(temp);
                    std::cout<<"No such intersection is suggested."<<std::endl;
                    intersections=findIntersectionsOfTwoStreets({streetID1,id2[0]});
                    for(int i = 0 ; i < intersections.size(); i++){
                        intersection_info[intersections[i]].highlight = true;
                    }
                }else{
                    std::string temp = "No such possible second street name. Re-enter second input.";
                    application -> update_message(temp);
                    std::cout<<"No such possible second street name. Re-enter second input."<<std::endl;
                }
            }else{   */                                                                       //Give suggestions on street names
            
            
            intersections = findIntersectionsOfTwoStreets({id1[0],id2[0]});

            gtk_entry_set_text(text1,street_info[id1[0]].name.c_str());
            gtk_entry_set_text(text2,street_info[id2[0]].name.c_str());
            if(intersections.size() != 0){
                std::cout << "Found some intersections between the two streets."<<std::endl;
                application->update_message("The intersections are highlighted in blue.");

                check_highlighted_intersections(intersections[0]); //highlight the first intersection found 
               std::cout<<getIntersectionName(intersections[0])<<std::endl;

                LatLon coord = getIntersectionPosition(intersections[0]);
                ezgl::point2d pos(xFromLon(coord.longitude(),map_bounds.avg_lat)
                ,yFromLat(coord.latitude()));

                double w = world_w / 2;
                double h = world_h / 2;
                renderer_info.curr_zoom = renderer_info.init_zoom;

                application -> get_renderer() -> set_visible_world(
                {{pos.x - w, pos.y - h}, {pos.x + w, pos.y + h}});

                zoom_to_level(6, application); // zoom to the found intersection

            }else{
                 application->update_message("Auto-completed street names, but no intersections between two streets. You can specify more in details.");
            }
            
            
        }
    
    application->refresh_drawing();
    
}

void find_path_button(GtkWidget *,ezgl::application* application)
{
    if(highlighted_intersections.first == -1 || highlighted_intersections.second == -1)
    {
        application->update_message("Please highlight two intersections.");
        return;
    }
    
    highlighted_segments.clear();
    highlighted_segments = findPathBetweenIntersections(highlighted_intersections.first, 
                                                        highlighted_intersections.second, 15);
    
    if(highlighted_segments.size() != 0){
        
        print_instructions(highlighted_segments);
        highlighted_street = -1;
        application->update_message("The found path is highlighted in purple.");
        
        LatLon pt_1 = getIntersectionPosition(highlighted_intersections.first);
        LatLon pt_2 = getIntersectionPosition(highlighted_intersections.second);
        
        double y_1 = yFromLat(pt_1.latitude());
        double x_1 = xFromLon(pt_1.longitude(), map_bounds.avg_lat);
        
        double y_2 = yFromLat(pt_2.latitude());
        double x_2 = xFromLon(pt_2.longitude(), map_bounds.avg_lat);
        
        double w = world_w / 2;
        double h = world_h / 2;
        double center_x = (x_1 + x_2) / 2;
        double center_y = (y_1 + y_2) / 2;
        renderer_info.curr_zoom = renderer_info.init_zoom;
        
        ezgl::point2d min(center_x - w, center_y - h);
        ezgl::point2d max(center_x + w, center_y + h);
                
        application -> get_renderer() -> set_visible_world({min, max});
                
        zoom_to_level(modify_zoom_level_to_segs(), application);
        ezgl::translate_left(application->get_canvas(application->get_main_canvas_id()), 4);
        
    } // highlight and zoom to the found path
    
    application->refresh_drawing();
    
    GtkWidget* label = (GtkWidget*)application->get_object("Navi");
    GtkWidget* dialog = (GtkWidget*)application->get_object("NavigationWindow");
    
    show_directions(application, label, dialog);

}

void help_button(GtkWidget *,ezgl::application* application)
{
//    GtkTextBuffer* b1 = (GtkTextBuffer*)application->get_object("textbuffer1");
//    GtkTextView* v1 = (GtkTextView*)application->get_object("Text");
//    GtkRevealer* r2 = (GtkRevealer*) application -> get_object("Revealer2");
//    
//    gtk_text_view_set_buffer(v1,b1);
//    gtk_revealer_set_reveal_child(r2,!gtk_revealer_get_child_revealed(r2));
    
    application->update_message("Instructions are displayed in the pop-up box.");
    
    GtkWidget* content_area;
    GtkWidget* label;
    GtkWidget* dialog;
    
    dialog = gtk_dialog_new();
    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    
    label = gtk_label_new("TTR Map User Interface Guide\n\n"
            "Map Selection: Click on the first combo box to select maps\n"
            "Search Modes: Click on the second combo box to select search modes,\n"
            "click search button after typing inputs\n"
            "> Streets Mode: enter a street name\n"
            "> Intersection Mode: enter two street names\n\n"
            "Find Path: Once two intersections are highlighted,\n"
            "click to find the shortest path in between\n"
            "> Click on two locations on the map\n"
            "> Use Intersection Search Mode to specify two intersections");
    gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_CENTER);
    gtk_container_add(GTK_CONTAINER(content_area), label);
    
    gtk_window_set_title(GTK_WINDOW(dialog), "TTR Map Help");
    gtk_window_set_modal(GTK_WINDOW(dialog), FALSE);
    gtk_window_set_keep_above(GTK_WINDOW(dialog), TRUE);
    gtk_window_move(GTK_WINDOW(dialog), 1200, 700);
    
    gtk_widget_set_opacity(dialog, 0.8); // use a slightly transparent window to display the help message
    
    if(help_dialog == 0){
        
        gtk_widget_show_all(dialog);
        help_dialog = 1;
        
    }
    
    g_signal_connect(GTK_DIALOG(dialog), "response", G_CALLBACK(help_dialog_response), NULL);
    
}

void help_dialog_response(GtkDialog* dialog, gint response_id){
    
    if(response_id == GTK_RESPONSE_DELETE_EVENT){
        
        help_dialog = 0;
        //gtk_widget_destroy(GTK_WIDGET(dialog));
        
    }
    
}

void zoom_to_level(int dest, ezgl::application* app){
    
    auto cnv = app->get_canvas("MainCanvas");
    
    std::cout << "destination level is " << dest << std::endl;
    
    int curr = renderer_info.curr_zoom;
    
    if(dest == curr) return;
    
    else if(dest < curr){
        
        for(int i = 0; i < (curr - dest); i++){
            
            ezgl::zoom_out(cnv, 5.0 / 3.0);
            
        }
        
    }
    
    else{
        
        for(int i = 0; i < (dest - curr); i++){
            
            ezgl::zoom_in(cnv, 5.0 / 3.0);
            
        }
        
    }
    
}

void map_selection_box(GtkWidget *widget,ezgl::application* application){                      //Able to switch cities
    
    closeMap();
    
    std::string map_path = "/cad2/ece297s/public/maps/toronto_canada.streets.bin";
    
    GtkComboBoxText* map_select = (GtkComboBoxText*) application -> get_object("map_select");
    
    //GtkEntry* text1 = (GtkEntry*) application -> get_object("TextInput");
    
    gchar* active_map = gtk_combo_box_text_get_active_text(map_select);
    
    std::string name(active_map);
    
        if(!name.compare("Toronto")) map_path = "/cad2/ece297s/public/maps/toronto_canada.streets.bin";
        
        else if(!name.compare("Hamilton")) map_path = "/cad2/ece297s/public/maps/hamilton_canada.streets.bin";
              
        else if(!name.compare("Iceland")) map_path = "/cad2/ece297s/public/maps/iceland.streets.bin";
            
        else if(!name.compare("Moscow")) map_path = "/cad2/ece297s/public/maps/moscow_russia.streets.bin";
        
        else if(!name.compare("Beijing")) map_path = "/cad2/ece297s/public/maps/beijing_china.streets.bin";
        
        else if(!name.compare("Egypt")) map_path = "/cad2/ece297s/public/maps/cairo_egypt.streets.bin";
        
        else if(!name.compare("Tokyo")) map_path = "/cad2/ece297s/public/maps/tokyo_japan.streets.bin";
        
        else if(!name.compare("Cape-Town")) map_path = "/cad2/ece297s/public/maps/cape-town_south-africa.streets.bin";
        
        else if(!name.compare("Golden-Horseshoe")) map_path = "/cad2/ece297s/public/maps/golden-horseshoe_canada.streets.bin";
    
        else if(!name.compare("Interlaken")) map_path = "/cad2/ece297s/public/maps/interlaken_switzerland.streets.bin";
        
        else if(!name.compare("London")) map_path = "/cad2/ece297s/public/maps/london_england.streets.bin";
        
        else if(!name.compare("New-Delhi")) map_path = "/cad2/ece297s/public/maps/new-delhi_india.streets.bin";
        
        else if(!name.compare("New-York")) map_path = "/cad2/ece297s/public/maps/new-york_usa.streets.bin";
        
        else if(!name.compare("Rio-De-Janeiro")) map_path = "/cad2/ece297s/public/maps/rio-de-janeiro_brazil.streets.bin";
        
        else if(!name.compare("Saint-Helena")) map_path = "/cad2/ece297s/public/maps/saint-helena.streets.bin";
        
        else if(!name.compare("Singapore")) map_path = "/cad2/ece297s/public/maps/singapore.streets.bin";
        
        else if(!name.compare("Sydney")) map_path = "/cad2/ece297s/public/maps/sydney_australia.streets.bin";
        
        else if(!name.compare("Tehran")) map_path = "/cad2/ece297s/public/maps/tehran_iran.streets.bin";
        
    map_bounds.max_lat = -91;
    map_bounds.min_lat = 91;
    map_bounds.max_lon = -180;
    map_bounds.min_lon = 180;
    map_bounds.avg_lat = 0; //reset map_bounds
    
    loadMap(map_path); // load new map
    std::cout << "Successfully loaded new map :" << name << std::endl;
    
    ezgl::rectangle new_world{{xFromLon(map_bounds.min_lon, map_bounds.avg_lat), yFromLat(map_bounds.min_lat)}, 
                                  {xFromLon(map_bounds.max_lon, map_bounds.avg_lat), yFromLat(map_bounds.max_lat)}};
    application->change_canvas_world_coordinates("MainCanvas",new_world);
    
    GtkEntry* t1 = (GtkEntry*) application->get_object("TextInput");
    GtkEntry* t2 = (GtkEntry*) application->get_object("TextInput2");
    
        gtk_entry_set_text(t1, "");
        gtk_entry_set_text(t2, "");
    
    application->refresh_drawing();
    
    //std::cout << name << std::endl;
    
}

void mode_selection_box(GtkWidget *widget,ezgl::application* application){
    
    GtkComboBoxText* mode_select = (GtkComboBoxText*) application -> get_object("search_mode");
    
    gchar* active_mode = gtk_combo_box_text_get_active_text(mode_select);
    
    std::string mode(active_mode);
    
    GtkRevealer* r1 = (GtkRevealer*) application -> get_object("Revealer1");
    
    GtkEntry* t1 = (GtkEntry*) application->get_object("TextInput");
    GtkEntry* t2 = (GtkEntry*) application->get_object("TextInput2");

    if(!mode.compare("Streets")){
        
        gtk_revealer_set_reveal_child(r1, FALSE);
        
        gtk_entry_set_placeholder_text(t1, "Enter street name");
        
        gtk_entry_set_text(t2, "");
        
    } // if in search street mode, show only one input box
    
    else if(!mode.compare("Intersections")){
        
        gtk_revealer_set_reveal_child(r1, TRUE);
        
        gtk_entry_set_placeholder_text(t1, "1st street");
        gtk_entry_set_placeholder_text(t2, "2nd street");
        
    } // if in search intersections mode, show both input boxes

}

std::string prediction(std::string input){                                              //Predict the user inputs
    std::vector<int> id=findStreetIdsFromPartialStreetName(input);
    std::string output;
    
    if(id.size()!=0){
        for(int i = 0 ; i < id.size (); i++){
            if( input == getStreetName(id[i]) ){
                output = "This street exists, press button to search " + getStreetName(id[i]) + ".";
                return output;
            }
        }
        output = "Do you mean " + getStreetName(id[0]) + "? ";
    }else{
        output = "There is no matching for " + input + ". ";
    }
    
    return output;
    
}

void clear_button(GtkWidget *,ezgl::application* application){                              //Clear all red highlights
    
    GtkEntry* t1 = (GtkEntry*) application->get_object("TextInput");
    GtkEntry* t2 = (GtkEntry*) application->get_object("TextInput2");
    
        gtk_entry_set_text(t1, "");
        gtk_entry_set_text(t2, "");
    
    highlighted_street = -1;
    
    highlighted_intersections.first = -1;
    highlighted_intersections.second = -1;
    highlighted_segments.clear();
    
    GtkRevealer* r2 = (GtkRevealer*) application -> get_object("Revealer2");
    gtk_revealer_set_reveal_child(r2, FALSE);
    
    application -> update_message("Search inputs and results cleared.");
    
    application->refresh_drawing();
    
}

void show_directions(ezgl::application* application, GtkWidget* label, GtkWidget* dialog){
    
//    GtkTextBuffer* b2 = (GtkTextBuffer*)application->get_object("textbuffer2");
//    GtkTextView* v1 = (GtkTextView*)application->get_object("Text");
//    GtkRevealer* r2 = (GtkRevealer*) application -> get_object("Revealer2");
//    
//    gtk_revealer_set_reveal_child(r2, TRUE);
//    gtk_text_buffer_set_text(b2,instructions.c_str(),instructions.size());
//    gtk_text_view_set_buffer(v1,b2);
    
    application->update_message("Directions are displayed in the pop-up box.");
    //ezgl::canvas* cnv = application->get_canvas(application->get_main_canvas_id());
    
    //GtkWidget* content_area;
    
    //content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    gtk_label_set_text(GTK_LABEL(label), instructions.c_str()); // display the pre-loaded instructions string to a label in a window
    gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
    
    //gtk_container_add(GTK_CONTAINER(content_area), label);
    //gtk_container_add(GTK_CONTAINER(content_area), scrollbar);
    
    gtk_window_set_title(GTK_WINDOW(dialog), "TTR Map Navigation");
    gtk_window_set_modal(GTK_WINDOW(dialog), FALSE);
    gtk_window_set_keep_above(GTK_WINDOW(dialog), TRUE);
    gtk_window_move(GTK_WINDOW(dialog), 5, 125);
        
    gtk_widget_show_all(dialog);
    
    g_signal_connect(GTK_DIALOG(dialog), "delete-event", G_CALLBACK(gtk_widget_hide_on_delete), NULL);
    
}

//void navi_dialog_response(GtkDialog* dialog, gint response_id){
//    
//    if(response_id == GTK_RESPONSE_DELETE_EVENT){
//        
//        std::cout << gtk_widget_hide_on_delete(GTK_WIDGET(dialog)) << std::endl;
//        
//    }
//    
//}