#pragma once

#include "MobInt.h"
#include "Idptr.h"

void ChangeMob(id_ptr_on<IMob> new_mob);
id_ptr_on<IMob> GetMob();
void SetMob(size_t new_mob);

void SetCreator(size_t new_creator);
size_t GetCreator();