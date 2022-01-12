#pragma once
#include "GameObj3D.hpp"

vector<GameObj3D *> scene;


void DeleteFromScene(int id)
{
    int index = -1;
    for (int i = 0; i < scene.size(); i++)
    {
        if (scene[i]->id == id)
        {
            index = i;
            break;
        }
    }
    if (index > -1)
    {
        //delete scene[index];
        scene.erase(scene.begin() + index);
    }
}

bool CollidesWithSth(GameObj3D &check)
{
    for (int i = 0; i < scene.size(); i++)
    {
        GameObj3D *obj = scene[i];
        if (obj->isCollid && check.id != obj->id)
        {
            if (intersect(check, *obj))
            {
                return true;
            }
        }
    }
    return false;
}

