#include "map.hpp"

using namespace std;

SDL_Texture* loadTextureImg(SDL_Renderer* renderer, const char* fichier)
{
    SDL_Surface* surf_tamp = NULL;
    surf_tamp = IMG_Load(fichier);
    if(!surf_tamp)
    {
        cout<<"Problème dans le chragement du fichier "<<fichier<<". Erreur SDL : "<<SDL_GetError()<<endl;
        return NULL;
    }

    else
    {
        SDL_Texture* image = SDL_CreateTextureFromSurface(renderer,surf_tamp);
        SDL_FreeSurface(surf_tamp);
        return image;
    }

}


bool isPosInRect(const int x, const int y, const SDL_Rect rect)
{
    return x>= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h;
}

bool isPosInRect(const SDL_Rect pos, const SDL_Rect rect)
{
    return pos.x>= rect.x && pos.x <= rect.x + rect.w && pos.y >= rect.y && pos.y <= rect.y + rect.h;
}

void printRect(const SDL_Rect rect)
{
    cout<<"x : "<<rect.x<<", y : "<<rect.y<<", w : "<<rect.w<<", h : "<<rect.h<<endl;
}


/***************************conversions***************************/
string intToString(int n)
{
    stringstream ss;
    ss<<n;
    return ss.str();
}
