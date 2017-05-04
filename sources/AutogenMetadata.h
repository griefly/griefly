#pragma once

#include <unordered_map>

#include "core/objects/MainObject.h"

typedef kv::IMainObject* (*ItemCreator)();
typedef kv::IMainObject* (*VoidItemCreator)();

std::unordered_map<QString, ItemCreator>* GetItemsCreators();

std::unordered_map<QString, VoidItemCreator>* GetVoidItemsCreators();

typedef void (*VariableSetter)(kv::IMainObject* ptr, FastDeserializer& str);

typedef std::unordered_map<QString, VariableSetter> SettersForType;

std::unordered_map<QString, SettersForType>& GetSettersForTypes();

void InitRealTypes();
void InitCreators();
void InitSettersForTypes();
