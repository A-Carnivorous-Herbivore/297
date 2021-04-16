/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "data_structures.h"
#include "helper_functions.h"
#include "StreetsDatabaseAPI.h"
#include "m1.h"
#include "m2.h"
#include "m3.h"
#include "navigation_functions.h"
#include "math.h"
#include <iomanip>
#include "chrono"
#define PI 3.14159265358979
#define SPEED 27.78 // int m/s, equivalent to 100km/h
#define DEFAULT_TIME 36000 // equivalent to travelling to a intersection in 10 hours,
                            // long enough to allow the nodes to reset during astar

double computePathTravelTime(const std::vector<StreetSegmentIdx>& path, const double turn_penalty){
    
    double result = 0;
    
    for(auto it = path.begin(); it != path.end(); it++)
    {
        result += segment_info[*it].time; // add up all segment travel time
    }
    
    
    StreetIdx firstSegStreet, secondSegStreet;
    
    for(auto it = path.begin(); it != (--(path.end()));)    //Go through the list
    {
        firstSegStreet = segment_info[*it].streetID;
        it++;
        secondSegStreet = segment_info[*it].streetID;
        
        if(firstSegStreet != secondSegStreet)
            result += turn_penalty;
    }
    
    return result;
    
}

std::vector<StreetSegmentIdx> findPathBetweenIntersections(const IntersectionIdx intersect_id_start, 
                                                            const IntersectionIdx intersect_id_destination,
                                                            const double turn_penalty){
   
    bool found  = astarPath(intersect_id_start, intersect_id_destination, turn_penalty);
    
    std::vector<StreetSegmentIdx> result;
    
    if(!found)
    {
        std::cout << "Not found";
        resetNodes();
        return result;
    }
  
    return backTrace(intersect_id_destination);
}

bool astarPath(int sourceNode, IntersectionIdx dest, double turn_penalty)
{
   
    resetNodes();
    std::map<double,waveElem> waveFront;
    waveFront.insert(std::make_pair(0,waveElem(sourceNode,-1,0)));
    
    while(waveFront.size() != 0)
    {
        waveElem current = (*(waveFront.begin())).second;
        waveFront.erase(waveFront.begin());
        
        int currNode = current.nodeIdx;
        
        if(current.travelTime < nodes[currNode].bestTime)
        {
            nodes[currNode].reachingEdge = current.segID;
            nodes[currNode].bestTime = current.travelTime; // update the best route to a node
            
            if(currNode == dest)
                return true;
            
            for(auto it = intersection_info[currNode].streetSegmentIdVector.begin();
                    it != intersection_info[currNode].streetSegmentIdVector.end();
                    it++)
            {
                if(*it == nodes[currNode].reachingEdge) // don't go back where you came
                    continue;
                IntersectionIdx toID = segment_info[*it].to;
                if(segment_info[*it].oneway)
                {
                    if(segment_info[*it].to == currNode)
                        continue;
                }
                else
                {
                    if(segment_info[*it].from == currNode)
                       toID = segment_info[*it].to;
                    else
                       toID = segment_info[*it].from; // get the node at the other end of the segment
                }
                
                double curr_time = nodes[currNode].bestTime+segment_info[*it].time;
                if(nodes[currNode].reachingEdge != -1 &&
                   segment_info[*it].streetID != segment_info[nodes[currNode].reachingEdge].streetID)
                {
                    curr_time += turn_penalty;
                }
                
                
                waveFront.insert(std::make_pair(curr_time+timeEstim(toID,dest), //use total time to judge a path
                waveElem(toID,*it,curr_time)));
            }
        }
        
    }
   
    return false;
}

std::vector<StreetSegmentIdx> backTrace(IntersectionIdx destID)
{
    std::list<StreetSegmentIdx> path;
    int currNode = destID;
    StreetSegmentIdx prevSeg = nodes[currNode].reachingEdge;
    
    //keep track of the reaching edges till reaches start
    while (prevSeg != -1)
    {
        path.push_front(prevSeg);
        if(segment_info[prevSeg].from != currNode)
            currNode = segment_info[prevSeg].from;
        else
            currNode = segment_info[prevSeg].to;
        prevSeg = nodes[currNode].reachingEdge;
    }
    
    std::vector<StreetSegmentIdx> path_vec;
    while(path.size() != 0)
    {
        path_vec.push_back(path.front());
        path.pop_front();
    } // convert to a vector
    
    resetNodes(); // reset the nodes for next round of computation
    
    return path_vec;
}

double timeEstim(IntersectionIdx start, IntersectionIdx end)
{
    double dist = findDistanceBetweenTwoPoints(std::make_pair(
          getIntersectionPosition(start), getIntersectionPosition(end)));
    return (dist/SPEED); // assume the rest of the way is fast and smooth at 100km/h
}

void resetNodes()
{
    int size = nodes.size();
    for(int i = 0; i < size; i++)
    {
        nodes[i].reachingEdge = -1;
        nodes[i].bestTime = 36000;
    }
}

void completeResetNodes()
{
    int size = nodes.size();
    for(int i = 0; i < size; i++)
    {
        nodes[i].reachingEdge = -1;
        nodes[i].bestTime = 36000;
        nodes[i].pickUp = {};
        nodes[i].dropOff = {};
        nodes[i].depot = -1;
    }
}

void print_instructions(std::vector<StreetSegmentIdx> input){   //Print instructions of paths
    std::vector<std::string> name;
    name.resize(input.size());
    name = segToName(input);
    instructions.clear();
                                                                //Make a string to store information
    instructions.append("Start: ");
    instructions.append(getIntersectionName(highlighted_intersections.first));
    instructions.append("\n");
    instructions.append("Destination: ");
    instructions.append(getIntersectionName(highlighted_intersections.second));
    instructions.append("\n");
    std::cout << "\nThe navigation has started."<< std::endl;
    instructions.append("The navigation has started.\n\n");
//    std::cout << "Take " << name[0] << "\n";
//    instructions.append("Take ");
//    instructions.append(name[0]);
//    instructions.append(".\n");
    double distance = segment_info[input[0]].length;
    if(name.size()>=2){                                     //In case the destination follows the starting point closely
        for(int i  = 1 ; i < name.size(); i++){

            if( name[i] == name [i-1] ){
                distance += segment_info[input[i-1]].length;
            }else{
                                                            //Set floor distance for user convenience while closer than 1000 meters
                if(distance<1000){                            
                    std::cout<< "Follow "<< name[i-1]<< " for " <<  ((int)(distance/10))*10 << " meters, then ";
                    instructions.append("Follow ");
                    instructions.append(name[i-1]);
                    instructions.append(" for ");
                    instructions.append(std::to_string(((int)(distance/10))*10));
                    instructions.append(" meters,\nthen ");
                    distance = segment_info[input[i]].length;
                    angleBetweenSegments(input[i-1],input[i]);
                    std::cout<< " "<< name[i]<< std::endl;
                    instructions.append(" ");
                    instructions.append(name[i]);
                    instructions.append("\n\n");
                }else{                                  //Convert to km if over 1000 meters, also take floor value.
                    
                    std::cout<< "Follow " << name[i-1]<< " for "  << std::fixed << 
                       std::setprecision(1) <<  ((distance-50)/1000) << " kilometers, then ";
                    instructions.append("Follow ");
                    instructions.append(name[i-1]);
                    instructions.append(" for ");                   //Store instructions in string
                                                        
                    int beforeDecimal = (int)(distance);            //Find values before and after decimal for storage convenience
                    int afterDecimal = (int)((distance*10)/1000) - ((int)distance/1000)*10;
                    
                    instructions.append(std::to_string(beforeDecimal/1000));
                    instructions.append(".");
                    instructions.append(std::to_string(afterDecimal));
                    instructions.append(" kilometers,\nthen ");
                    distance = segment_info[input[i]].length;
                    angleBetweenSegments(input[i-1],input[i]);
                    
                    std::cout << " " << name[i] << std::endl;
                    instructions.append(" ");
                    instructions.append(name[i]);
                    instructions.append("\n\n");
                }
            }

        }
    }
                                                    // If distance closer than 1000 meters
    if(distance<1000){
        
        std::cout<< "Follow " << name[name.size()-1] << " for " << 
            ((int)(distance/10))*10 << " meters, and the destination will be on the side. \n";
    }else{                                         //If over 1000 meters distance
        
        std::cout<< "Follow " << name[name.size()-1] << " for " << std::fixed << std::setprecision(1) <<
                (distance-50)/1000 << " kilometers, and the destination will be on the side. \n";
    }
    
    instructions.append("Follow "); 
    instructions.append(name[name.size()-1]);
    instructions.append(" for ");
                                                   //If distance less than 1000 meters
    if(distance < 1000){
        
        instructions.append(std::to_string(((int)(distance/10))*10));
        instructions.append(" meters,\nand the destination will be on the side.\n\n");
    }else{                                         //If distance over 1000 meters
        
        int beforeDecimal = (int)(distance);
        int afterDecimal = (int)((distance*10)/1000) - ((int)distance/1000)*10;
        instructions.append(std::to_string(beforeDecimal/1000));
        instructions.append(".");
        instructions.append(std::to_string(afterDecimal));
        instructions.append(" kilometers,\nthen ");
    }
                                                           //Computer travel time in total
    double time = computePathTravelTime(input,15);
    std::cout<< "Total travel time: " << ((int)(time/60)) 
             << " minutes and " << ((int)(time))%60  << " seconds\n\n";
    instructions.append("Total travel time: ");
    instructions.append(std::to_string(((int)(time/60))));
    instructions.append(" minutes and ");
    instructions.append(std::to_string(((int)(time))%60));
    instructions.append(" seconds.");
    
}

std::vector<std::string> segToName(std::vector<StreetSegmentIdx> input){
    std::vector<std::string> name = {};
    for (int i = 0 ; i < input.size() ; i++){
        name.push_back(getStreetName(getStreetSegmentInfo(input[i]).streetID));
    }
    return name;
}

int angleBetweenSegments(StreetSegmentIdx before, StreetSegmentIdx after){
   
    // this function distinguishes turning conditions of two segments
    
    int point1 = getStreetSegmentInfo(before).from;
    int point2 = getStreetSegmentInfo(before).to;
    int point3 = getStreetSegmentInfo(after).from;
    int point4 = getStreetSegmentInfo(after).to; // 4 intersections of the two segments
    
    double streetLength1 = findStreetSegmentLength(before);
    double streetLength2 = findStreetSegmentLength(after);
    
    LatLon position1 = getIntersectionPosition(point1);
    LatLon position2 = getIntersectionPosition(point2);
    LatLon position3 = getIntersectionPosition(point3);
    LatLon position4 = getIntersectionPosition(point4);
    
    double streetLength3;
    double angle;
    
    // see which point connects the two segments
    
    if (point1 == point3){                      //If point 1 and point 3 are the same point
                                                //Talk about the cases of from && to overlap
        streetLength3 = findDistanceBetweenTwoPoints({position2,position4});
        angle = abs(acos (( streetLength1 * streetLength1 + streetLength2 * streetLength2 - streetLength3 * streetLength3)
                /(2 * streetLength1 * streetLength2)) * 180.0 / PI);
        /*if(position3.latitude()>position2.latitude()){
            double position3_x = position3.latitude() - position2.latitude();
            double position3_y = position3.longitude() - position2.longitude();
            double position4_x = position4.latitude() - position2.latitude();
            double position4_y = position4.longitude() - position2.longitude();
            double ratio = position3_y/position3_x;
            
        }else if (position3.longitude()<position2.longitude()&&position3.latitude()>position2.latitude()){
            
        }*/
        
        //Vector 1 is x vector, vecotor 2 is y vector.
        std::pair <double,double> vector1 = {position3.longitude()-position2.longitude(),position3.latitude()-position2.latitude()};
        std::pair <double,double> vector2 = {position4.longitude()-position3.longitude(),position4.latitude()-position3.latitude()};
        double indicator = vector1.first * vector2.second - vector1.second * vector2.first;     //Cross product between vectors
        print_turn(angle,indicator);
        
    }else if(point2 == point3){                    
        //If point 2 and point 3 are the same
        streetLength3 = findDistanceBetweenTwoPoints({position1,position4});                //Simply taking square to find distance
        angle = abs(acos ( ( streetLength1*streetLength1 + streetLength2*streetLength2 - streetLength3*streetLength3 )/
                (2 * streetLength1 * streetLength2) )*180.0 / PI);
        
        //The following vector name is not vector! It is cross vector product!
        
        std::pair <double,double> vector1 = {position3.longitude() - position1.longitude(), position3.latitude() - position1.latitude() };
        std::pair <double,double> vector2 = {position4.longitude()-position3.longitude(),position4.latitude()-position3.latitude()};
        double indicator = vector1.first * vector2.second - vector1.second * vector2.first; 
        print_turn(angle,indicator);
        
    }else if (point1 == point4){   
        //If point 1 and point 4 are the same 
        
        streetLength3 = findDistanceBetweenTwoPoints({position2,position3});
        angle = abs(acos (( streetLength1*streetLength1 + streetLength2*streetLength2 - streetLength3*streetLength3)
                / (2 * streetLength1 * streetLength2) ) * 180.0 / PI);
        std::pair <double,double> vector1 = {position4.longitude()-position2.longitude(),position4.latitude()-position2.latitude()};
        std::pair <double,double> vector2 = {position3.longitude()-position4.longitude(),position3.latitude()-position4.latitude()};
        double indicator = vector1.first * vector2.second - vector1.second * vector2.first; 
        print_turn(angle,indicator);
            
    }else{  
        //If point 2 and point 4 are the same
        
        streetLength3 = findDistanceBetweenTwoPoints({position1,position3});
        angle = abs(acos (( streetLength1*streetLength1 + streetLength2*streetLength2 - streetLength3*streetLength3)
                /(2 * streetLength1 * streetLength2))* 180.0 / PI);
        std::pair <double,double> vector1 = {position4.longitude()-position1.longitude(),position4.latitude()-position1.latitude()};
        std::pair <double,double> vector2 = {position3.longitude()-position4.longitude(),position3.latitude()-position4.latitude()};
        double indicator = vector1.first * vector2.second - vector1.second * vector2.first; 
        print_turn(angle,indicator);
       
    } // use the cross product to find out if the driver should turn right or left
    // then use the formula to calculate angles and print out turn conditions
    
    return angle;
}

void print_turn(double angle,double indicator)
{
    // print out turn instruction according to calculated turn angle and cross product
    
    if(indicator > 0){
            if(angle<=180 && angle >=160){
                std::cout<<"𐍊 keep straight onto";
                instructions.append("𐍊 keep straight onto");
            }else if(angle>=120 && angle<160){
                std::cout<< "↖ slight left turn onto"; 
                instructions.append("↖ slight left turn onto");
            }else if(angle>=60 && angle<120 ){
                std::cout<< "↰ left turn onto";
                instructions.append("↰ left turn onto");
            }else if(angle<60 && angle >= 40){
                std::cout<< "↙ steep left turn onto";
                instructions.append("↙ steep left turn onto");
            }else{
                std::cout<< "↶ take a legal u-turn onto";
                instructions.append("↶ take a legal u-turn onto");
            }
        }else if (indicator < 0){
            if(angle<=180 && angle >= 160){
                std::cout<<"𐍊 keep straight onto";
                instructions.append("𐍊 keep straight onto");
            }else if(angle>=120 && angle<160){
                std::cout<< "↗ slight right turn onto";
                instructions.append("↗ slight right turn onto");
            }else if(angle>=60 && angle<120 ){
                std::cout<< "↱ right turn onto";
                instructions.append("↱ right turn onto");
            }else if(angle<60 && angle >= 40){
                std::cout<< "↘ steep right turn onto";
                instructions.append("↘ steep right turn onto");
            }else{
                std::cout<< "↶ take a legal u-turn onto";
                instructions.append("↶ take a legal u-turn onto");
            }
        }else{
            std::cout<< "𐍊 keep straight onto ";
            instructions.append("𐍊 keep straight onto ");
        }
}

