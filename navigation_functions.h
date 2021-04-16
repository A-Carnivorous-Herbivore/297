/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   navigation_functions.h
 * Author: qiuyunf1
 *
 * Created on March 20, 2021, 2:19 PM
 */

#ifndef NAVIGATION_FUNCTIONS_H
#define NAVIGATION_FUNCTIONS_H

#include "m1.h"
#include "m2.h"
#include "StreetsDatabaseAPI.h"
#include "helper_functions.h"
#include <list>

bool astarPath(int sourceNode, IntersectionIdx dest, double turn_penalty);

void completeResetNodes();

double timeEstim(IntersectionIdx start, IntersectionIdx end);

std::vector<StreetSegmentIdx> backTrace(IntersectionIdx destID);

void resetNodes();

void print_instructions(std::vector<StreetSegmentIdx> input);

std::vector<std::string> segToName(std::vector<StreetSegmentIdx> input);

int angleBetweenSegments(StreetSegmentIdx before, StreetSegmentIdx after);

void print_turn(double angle,double indicator);

bool multiPath(int sourceNode, int numberOfDelivers, double turn_penalty);
       
#endif /* NAVIGATION_FUNCTIONS_H */

