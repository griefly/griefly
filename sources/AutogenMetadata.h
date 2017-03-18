#pragma once

#include <unordered_map>

#include "core/objects/MainObject.h"

typedef IMainObject* (*ItemCreator)(quint32 id);
typedef IMainObject* (*VoidItemCreator)();

std::unordered_map<QString, ItemCreator>* GetItemsCreators();

std::unordered_map<QString, VoidItemCreator>* GetVoidItemsCreators();

typedef void (*VariableSetter)(IMainObject* ptr, FastDeserializer& str);

typedef std::unordered_map<QString, VariableSetter> SettersForType;

std::unordered_map<QString, SettersForType>& GetSettersForTypes();

void InitRealTypes();
void InitCreators();
void InitSettersForTypes();
