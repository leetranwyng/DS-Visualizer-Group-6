#pragma once
#include <cstdlib> 
#include <ctime>  
#include <fstream>

const int ACTION_NONE = 0;
const int ACTION_CREATE = 1;
const int ACTION_SEARCH = 2;
const int ACTION_INSERT = 3;
const int ACTION_REMOVE = 4;
const int ACTION_LOAD = 5;

void RenderHashTable();