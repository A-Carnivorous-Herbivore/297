/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   load_functions.h
 * Author: nimingha
 *
 * Created on February 4, 2021, 12:33 PM
 */

#ifndef LOAD_FUNCTIONS_H
#define LOAD_FUNCTIONS_H
void load_intersection_info();
void load_OSM_info();
void load_segment_info();
void load_feature_info();
void load_POI_info();
void street_name_ID_Map();
void street_name_ID_Map_No_Space();
double featureAreaHelper(FeatureIdx feature_id);
bool compareByArea(const feature_data &a, const feature_data &b);

#endif /* LOAD_FUNCTIONS_H */



