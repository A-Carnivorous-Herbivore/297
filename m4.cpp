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
#include "algorithm"
#include "math.h"
#include <iomanip>
#include "m4.h"
#include <chrono>
#include <queue>
CourierSubPath reverse_path(CourierSubPath path);
bool check_on_board(int num, std::vector<int> on_board);
void erase_elem(int num, std::vector<int>& on_board);
void multidijkstra(int numberOfDepots, int numberOfDelivers, const std::vector<DeliveryInf>& deliveries, 
		            const float turn_penalty, const std::vector<int>& depots);
void multiPath(int sourceNode, int numberOfDelivers, double turn_penalty, int numberOfDepots, int row);
std::vector <std::vector<double> > arr;
void multiPath_v2( int numberOfDelivers, double turn_penalty, int numberOfDepots, const std::vector<int>& depots);
struct spot_info{
    spot_info(int x, bool y, int z){
        intersection = x;
        isPickUp = y;
        job = z;
    }
    
    IntersectionIdx intersection;
    bool isPickUp;
    int job; // the index of the delivery
};
std::vector <int> close_depot_pickUp;
std::vector<spot_info> perturbator(const std::vector<spot_info>& dlv_order);

std::vector<CourierSubPath> travelingCourier(
		            const std::vector<DeliveryInf>& deliveries,
	       	        const std::vector<int>& depots, 
		            const float turn_penalty)
{
    // first we code greedy
    // then we try a local perturbator
    // use multithread when possible it's not too complicated with open mp
    // we can use some helper functions like reversing a subpath or multi-destination
    // dijkstra
    // nevetheless go watch the lectures first
    auto const start = std::chrono::high_resolution_clock::now();

    multidijkstra(depots.size(),deliveries.size(),deliveries,turn_penalty,depots);
    auto const end = std::chrono::high_resolution_clock::now();
    auto const delta = std::chrono::duration_cast<std::chrono::seconds> (end-start);
    std::cout << delta.count() << std::endl;
    std::vector<spot_info> dlv_order;
    std::vector<int> on_board,finished;
    spot_info up(0,true,0);
    spot_info down(0,false,0);
    double up_time = 1e6, down_time = 1e6;
    /*
    for(int i = 0; i < deliveries.size(); i++)
    {
        for(int j = 0; j < depots.size(); j++)
        {
            double temp_t = arr[i][j+2*deliveries.size()];
            {
                up_time = temp_t;
                up.intersection = deliveries[i].pickUp;
                up.job = i;
            }
        }
    }
    */
    for(int i  = 0 ; i < arr.size(); i++){
        for(int j = 0; j < arr[i].size(); j++){
            std::cout<<arr[i][j]<<" ";
        }
    }
    std::cout<<"JAY IS NO WRONG"<<close_depot_pickUp.size()<<std::endl;
    up.intersection = close_depot_pickUp[close_depot_pickUp.size()-1];
    for(int i = 0 ; i < depots.size(); i++){
        std::cout<< depots[i] << " ";
    }
    std::cout<< close_depot_pickUp[1]<< std::endl;
    for(int i = 0; i < deliveries.size(); i++){
        if(deliveries[i].pickUp == up.intersection){
            up.job = i;
            break;
        }
    }
    dlv_order.push_back(up);
    on_board.push_back(up.job);
    
    up_time = 1e6;
    
    int head;
    bool trait;
    while(deliveries.size() > finished.size() )
    {
        head = (--dlv_order.end())->job;
        trait = (--dlv_order.end())->isPickUp;
        for(int i = 0 ; i < deliveries.size(); i++)
        {
            if(check_on_board(i,finished))
                continue;
            
            if(check_on_board(i,on_board))
                continue;
            
            double temp_time = arr[head + deliveries.size()*(trait?0:1)][i];
            
            if(temp_time < up_time)
            {
                up_time = temp_time;
                up.intersection = deliveries[i].pickUp;
                up.job = i;
            }
        }
        
        if(on_board.size() != 0)
        {
            for(auto it = on_board.begin(); it != on_board.end(); it++)
            {

                double temp_time = arr[head + deliveries.size()*(trait?0:1)][*it + deliveries.size()];
                if(temp_time < down_time)
                {
                    down_time = temp_time;
                    down.intersection = deliveries[*it].dropOff;
                    down.job = *it;
                }
            }
        }
        else
        {
            down_time = 1e6;
        }
        //find the nearest pickup and legal dropoff
        double sec_time = arr[up.job][down.job+deliveries.size()];
        
        if(up_time == 0)
        {
            on_board.push_back(up.job);
            dlv_order.push_back(up);
            up_time = 1e6;
            down_time = 1e6;
            continue;
        }
        if(down_time== 0)
        {
            finished.push_back(down.job);
            erase_elem(down.job,on_board);
            dlv_order.push_back(down);
            up_time = 1e6;
            down_time = 1e6;
            continue;
        }
        if( down_time < up_time && sec_time < up_time*1.3 )
        {
            dlv_order.push_back(down);
            finished.push_back(down.job);
            erase_elem(down.job,on_board);
            head = down.intersection; // choose the dropoff as the next arrival, register finished dlv, and remove from on_boar
        }
        else
        {
            dlv_order.push_back(up);
            on_board.push_back(up.job); 
            head = up.job;// choose the pickup as the next arrival, register on_board dlv
        }
        
        up_time = 1e6;
        down_time = 1e6;
        // get a estimate and choose bewteen a dropoff or a pickup
    }
    
    
    for(int i = 0; i < 15; i++)
    {
        dlv_order = perturbator(dlv_order);
    }
    
    
    down_time = 1e6;
    for(int i = 0; i < depots.size(); i++)
    {
        double temp_t = arr[(--dlv_order.end())->job+deliveries.size()][2*deliveries.size() + i];
                if(temp_t< down_time)
                {
                    down_time = temp_t;
                    down.intersection = depots[i];
                   
                }
    } // find the closest depot to the last dropoff
    
    up_time = 1e6;
    for(int i = 0; i < depots.size(); i++)
    {
        double temp_t = arr[dlv_order.begin()->job][2*deliveries.size() + i];
                if(temp_t < up_time)
                {
                    up_time = temp_t;
                    up.intersection = depots[i];
                   
                }
    } // find the closest depot to the first pickup
    
    
    std::vector<CourierSubPath> cour_path;
    CourierSubPath temp_path;
    
    
    temp_path.end_intersection = (dlv_order.begin())->intersection;
    temp_path.start_intersection = up.intersection;
    temp_path.subpath = findPathBetweenIntersections(temp_path.start_intersection,temp_path.end_intersection,turn_penalty);
    if(temp_path.subpath.size() == 0)
    {
        cour_path.clear();
        return cour_path;
    }
    cour_path.push_back(temp_path); // find and store the path from start depot to first pickup
    
    
    for(int i = 0; i < dlv_order.size()-1; i++)
    {
        
        temp_path.end_intersection = dlv_order[i+1].intersection;
        temp_path.start_intersection = dlv_order[i].intersection;
        if(temp_path.start_intersection == temp_path.end_intersection)
        {
            continue;
        }
        temp_path.subpath = findPathBetweenIntersections(temp_path.start_intersection,temp_path.end_intersection,turn_penalty);
        if(temp_path.subpath.size() == 0 && temp_path.end_intersection != temp_path.start_intersection)
        {
            cour_path.clear();
            return cour_path;
        }
        cour_path.push_back(temp_path);
        
    } // find and store the paths from the dlv_order
    
    
    temp_path.end_intersection = down.intersection;
    temp_path.start_intersection = (--dlv_order.end())->intersection;
    temp_path.subpath = findPathBetweenIntersections(temp_path.start_intersection,temp_path.end_intersection,turn_penalty);
    if(temp_path.subpath.size() == 0)
    {
        cour_path.clear();
        return cour_path;
    }
    cour_path.push_back(temp_path); // find and store the paths from the last dropoff to the return depot
    
    
    return cour_path;
}




CourierSubPath reverse_path(CourierSubPath path)
{
    CourierSubPath result;
    
    result.start_intersection = path.end_intersection;
    result.end_intersection = path.start_intersection;
    
    while(path.subpath.size() != 0)
    {
        result.subpath.push_back(path.subpath.front());
        path.subpath.erase(path.subpath.begin());
    }
    
    return result;
}

bool check_on_board(int num, std::vector<int> on_board)
{
    if(on_board.size() == 0)
        return false;
    
    for(auto it = on_board.begin(); it != on_board.end(); it++)
    {
        if(*it == num)
            return true;
    }
    
    return false;
}

void erase_elem(int num, std::vector<int>& on_board)
{
    if(on_board.size() == 0)
        return;
    
    for(auto it = on_board.begin(); it != on_board.end(); it++)
    {
        if(*it == num)
        {
            on_board.erase(it);
            return;
        }
    }
    return;
}

void multidijkstra(int numberOfDepots, int numberOfDelivers, const std::vector<DeliveryInf>& deliveries, 
		            const float turn_penalty, const std::vector<int>& depots){
    
    completeResetNodes();
    std::cout<<"There are this many deliveries."<< deliveries.size() <<std::endl;
    std::cout<<"There are this many depots."<< depots.size() <<std::endl;
    arr.resize(numberOfDelivers*2);
    for(int i  = 0 ; i < numberOfDelivers; i++){
        arr[i].resize(numberOfDelivers*2 + numberOfDepots);
        arr[i+numberOfDelivers].resize(numberOfDelivers*2 + numberOfDepots);
        nodes[deliveries[i].pickUp].pickUp.push_back(i);
        nodes[deliveries[i].dropOff].dropOff.push_back(i);
    }
    
    for(int i  = 0 ;i < numberOfDepots; i++){
       // arr[i+numberOfDelivers*2].resize(numberOfDelivers*2 + numberOfDepots);
        nodes[depots[i]].depot = i;
    }

    for(int i = 0 ; i < numberOfDelivers; i++){
       // auto const start = std::chrono::high_resolution_clock::now();
        multiPath(deliveries[i].pickUp,numberOfDelivers,turn_penalty, numberOfDepots, i);
       // auto const end = std::chrono::high_resolution_clock::now();
       // auto const delta = std::chrono::duration_cast<std::chrono::milliseconds> (end-start);
       // std::cout << delta.count() << std::endl;
        multiPath(deliveries[i].dropOff,numberOfDelivers,turn_penalty, numberOfDepots, i+numberOfDelivers);
    }
   multiPath_v2( numberOfDelivers, turn_penalty, numberOfDepots, depots);
}
void multiPath(int sourceNode, int numberOfDelivers, double turn_penalty, int numberOfDepots, int row)
{   
    int numberOfFounds = 0;
    resetNodes();
    //std::priority_queue<int, vector<int>, less<int> > waveFront;
    std::map<double,waveElem> waveFront;
    waveFront.insert(std::make_pair(0,waveElem(sourceNode,-1,0)));
    
    while(numberOfFounds != numberOfDelivers*2 + numberOfDepots){
        waveElem current = (*(waveFront.begin())).second;
        waveFront.erase(waveFront.begin());
        
        int currNode = current.nodeIdx;
       /* std::vector<int> foundInPickUp;
        std::vector<int> foundInDropOff;*/
       
        if(current.travelTime < nodes[currNode].bestTime)
        {
            nodes[currNode].reachingEdge = current.segID;
            nodes[currNode].bestTime = current.travelTime; // update the best route to a node
            
            if(nodes[currNode].pickUp.size()!=0 || nodes[currNode].dropOff.size()!=0 || nodes[currNode].depot != -1){
                if(nodes[currNode].depot != -1){
                    arr[row][nodes[currNode].depot + numberOfDelivers*2] = current.travelTime;
                    numberOfFounds++;
                }
                for(int i = 0 ; i < nodes[currNode].pickUp.size(); i++){
                    numberOfFounds++;
                    arr[row][nodes[currNode].pickUp[i]] = current.travelTime;
                }
                for(int i = 0 ; i < nodes[currNode].dropOff.size(); i++){
                    numberOfFounds++;
                    arr[row][numberOfDelivers+nodes[currNode].dropOff[i]] = current.travelTime;
                }
                
            }
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
                
                
                waveFront.insert(std::make_pair(curr_time, //use total time to judge a path
                waveElem(toID,*it,curr_time)));
            }
        }
    }
    
    return ;
}
void multiPath_v2( int numberOfDelivers, double turn_penalty, int numberOfDepots, const std::vector<int>& depots)
{   
    resetNodes();
    close_depot_pickUp = {};
    //std::priority_queue<int, vector<int>, less<int> > waveFront;
    std::map<double,waveElem> waveFront;
    for(int i = 0 ; i < numberOfDepots; i++){
        waveFront.insert(std::make_pair(0,waveElem(depots[i],-1,0)));
    }
    while(1){
        waveElem current = (*(waveFront.begin())).second;
        waveFront.erase(waveFront.begin());
        
        int currNode = current.nodeIdx;
       /* if(nodes[currNode].pickUp.size()!=0){
            //std::cout<<nodes[currNode].reachingEdge<<std::endl;
            close_depot_pickUp = backTrace(currNode);
            break;
        }*/
       /* std::vector<int> foundInPickUp;
        std::vector<int> foundInDropOff;*/
       
        if(current.travelTime < nodes[currNode].bestTime)
        {
            nodes[currNode].reachingEdge = current.segID;
            nodes[currNode].bestTime = current.travelTime; // update the best route to a node
             if(nodes[currNode].pickUp.size()!=0){
                close_depot_pickUp = backTrace(currNode);
                return ;
             }
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
                
                
                waveFront.insert(std::make_pair(curr_time, //use total time to judge a path
                waveElem(toID,*it,curr_time)));
            }
        }
    }
    
    return ;
}

std::vector<spot_info> perturbator(const std::vector<spot_info>& dlv_order)
{
    int size = dlv_order.size();
    
    std::vector<spot_info> result = dlv_order;
    
    
    for(int i = 1; i < size - 3; i += rand()%2 + 2)
    {
        double org_time = arr[dlv_order[i-1].job + size/2*(dlv_order[i-1].isPickUp?0:1)][dlv_order[i].job + size/2*(dlv_order[i].isPickUp?0:1)]
                            + arr[dlv_order[i+1].job + size/2*(dlv_order[i+1].isPickUp?0:1)][dlv_order[i+2].job + size/2*(dlv_order[i+2].isPickUp?0:1)];
        
        double curr_time = arr[result[i-1].job + size/2*(result[i-1].isPickUp?0:1)][result[i].job + size/2*(result[i].isPickUp?0:1)]
                            + arr[result[i+1].job + size/2*(result[i+1].isPickUp?0:1)][result[i+2].job + size/2*(result[i+2].isPickUp?0:1)];
        
        double new_time = arr[result[i-1].job + size/2*(result[i-1].isPickUp?0:1)][result[i+1].job + size/2*(result[i+1].isPickUp?0:1)]
                            + arr[result[i].job + size/2*(result[i].isPickUp?0:1)][result[i+2].job + size/2*(result[i+2].isPickUp?0:1)];
        
        if(new_time < curr_time && new_time < org_time)
        {
            if(result[i].isPickUp && !result[i+1].isPickUp)
            {
                if(result[i].job == result[i+1].job)
                    continue;
            }
            spot_info exch = result[i];
            result[i] = result[i+1];
            result[i+1] = exch;
        }
        
    }
    
    return result;
}