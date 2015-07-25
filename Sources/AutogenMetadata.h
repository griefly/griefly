#pragma once

#include "MainInt.h"

typedef void (*variable_setter)(IMainObject* ptr, std::stringstream& str);

typedef std::unordered_map<std::string, variable_setter> setters_for_type;

std::unordered_map<std::string, setters_for_type>& get_setters_for_types();

void InitRealTypes();
void InitCreators();
void InitSettersForTypes();
