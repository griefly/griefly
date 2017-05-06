#pragma once

#include <unordered_map>

#include "core/objects/MainObject.h"

typedef kv::Object* (*ItemCreator)();
typedef kv::Object* (*VoidItemCreator)();

std::unordered_map<QString, ItemCreator>* GetItemsCreators();

std::unordered_map<QString, VoidItemCreator>* GetVoidItemsCreators();

typedef void (*VariableSetter)(kv::Object* ptr, FastDeserializer& str);

typedef std::unordered_map<QString, VariableSetter> SettersForType;

std::unordered_map<QString, SettersForType>& GetSettersForTypes();

void InitRealTypes();
void InitCreators();
void InitSettersForTypes();
