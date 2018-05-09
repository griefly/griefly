#pragma once

#include <unordered_map>

#include "objects/Object.h"

using ItemCreator = kv::Object*(*)();
using VoidItemCreator = kv::Object*(*)();

std::unordered_map<QString, ItemCreator>* GetItemsCreators();

std::unordered_map<QString, VoidItemCreator>* GetVoidItemsCreators();

using VariableSetter = void(*)(kv::Object* ptr, kv::FastDeserializer& str);

struct VariableInfo
{
    VariableSetter setter;
    QString type;
};

using VariablesForType = std::unordered_map<QString, VariableInfo>;

std::unordered_map<QString, VariablesForType>& GetVariablesForTypes();

void InitRealTypes();
void InitCreators();
void InitSettersForTypes();
