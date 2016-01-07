#pragma once

struct point
{
    int posx;
    int posy;
    int posz;
};

struct HashAmount
{
    unsigned int hash;
    unsigned int amount;
};

struct square
{
    bool inOpenList;
    bool inCloseList;
    int cost;
    int realcost;
    int posx;
    int posy;
    square* before;
};

struct Reaction
{
    unsigned int one;
    unsigned int oneamount;

    unsigned int two;
    unsigned int twoamount;

    unsigned int three;
    unsigned int threeamount;

    unsigned int result;
    unsigned int resultamount;
};

struct idpoint
{
    int posx;
    int posy;
    int find_hash;
};