
#include "map.hpp"

using namespace std;

void Map::setRectMap(SDL_Rect a_rectMap)//changer rectangle d'affichage de la map
{
    rectMap = a_rectMap;
}

void Map::getDimMap(int* w, int* h)
{
    (*w) = tmx_carte->tile_width * tmx_carte->width;
    (*h) = tmx_carte->tile_height * tmx_carte->height;
}

SDL_Rect Map::getRectMap()//récupère rectangle d'affichage de la map
{
    return rectMap;
}

SDL_Rect Map::posMapToPosRend(SDL_Rect pos)//conversion position
{
    return {rectMap.x + pos.x - scrollX,rectMap.y + pos.y - scrollY,pos.w,pos.h};
}

void Map::getPosInit(float* x, float* y)//récupère position initiale
{
    (*x) = (float)posInitX;
    (*y) = (float)posInitY;
}


// gestion du scrolling par rapport à une position donnée.
void Map::setPosCam(float a_x, float a_y)
{
    int x = round(a_x);
    int y = round(a_y);

    int tailleW = tmx_carte->tile_width * tmx_carte->width;
    int tailleH = tmx_carte->tile_height * tmx_carte->height;
    if(x<0 || y<0 || x>=tailleW || y>=tailleH)
        cout<<"Erreur dans la position de la map : la position dépasse la map"<<endl;

    //gestion position x :
    if(tailleW<rectMap.w || x < rectMap.w/2) //gestion des bords
        scrollX = 0;

    else if(tailleW - x < rectMap.w/2)
        scrollX = tailleW - rectMap.w;
    else // on a suffisamment de place des deux côtés
        scrollX = x - rectMap.w/2;

    if(tailleH<rectMap.h || y < rectMap.h/2) //gestion des bords
        scrollY = 0;
    else if(tailleH - y < rectMap.h/2)
        scrollY = tailleH -rectMap.h;
    else // on a suffisamment de place des deux côtés
        scrollY = y - rectMap.h/2;
}


void Map::showMap()
{

    tmx_tileset *ts;
    tmx_layer* layer = tmx_carte->ly_head;

    int modScrollX = scrollX%tmx_carte->tile_width;//permet de ne coller que des bouts de tile
    int modScrollY = scrollY%tmx_carte->tile_height;
    int i = scrollY/tmx_carte->tile_height ;
    int dep_j = scrollX/tmx_carte->tile_width ;
    int ecranW = rectMap.w / tmx_carte->tile_width + dep_j + 1;//dernier tile de l'écran
    int ecranH = rectMap.h / tmx_carte->tile_height + i + 1;

    SDL_Rect clip = {modScrollX,modScrollY,tmx_carte->tile_width - modScrollX,tmx_carte->tile_height - modScrollY};//coin sup gauche
    SDL_Rect pos = {rectMap.x,rectMap.y,tmx_carte->tile_width - modScrollX,tmx_carte->tile_height - modScrollY};

    ts = tmx_get_tileset(tmx_carte, layer->content.gids[(i*tmx_carte->width)+dep_j], (unsigned int*)&(clip.x), (unsigned int*)&(clip.y));
    clip.x += modScrollX;
    clip.y += modScrollY;
    SDL_RenderCopy(renderer,(SDL_Texture*) ts->image->resource_image,&clip,&pos);// coin supérieur gauche


    pos.x += pos.w;
    pos.w = tmx_carte->tile_width;
    //clip.x = 0;
    clip.w = tmx_carte->tile_width;
    for(int j = dep_j + 1; j < min(ecranW,(int)tmx_carte->width); j++) //ligne du haut + vérification : écran plus gd que map?
    {
        ts = tmx_get_tileset(tmx_carte, layer->content.gids[(i*tmx_carte->width)+j], (unsigned int*)&(clip.x), (unsigned int*)&(clip.y));
        clip.y += modScrollY;
        SDL_RenderCopy(renderer,(SDL_Texture*) ts->image->resource_image,&clip,&pos);
        pos.x += pos.w;
    }
    //ligne finie convertir à partir d'ici

    clip.w = modScrollX;//coin supérieur droit
    pos.w = modScrollX;
    ts = tmx_get_tileset(tmx_carte, layer->content.gids[(i*tmx_carte->width)+ecranW], (unsigned int*)&(clip.x), (unsigned int*)&(clip.y));
    clip.y += modScrollY;
    if(ecranW<tmx_carte->width)//on vérifie que l'écran n'est pas plus grand que la map
        SDL_RenderCopy(renderer,(SDL_Texture*) ts->image->resource_image,&clip,&pos);

    clip.h = tmx_carte->tile_height;
    pos.y += pos.h;
    pos.h = tmx_carte->tile_height;
    i++;


    for(; i<min((int)tmx_carte->height,ecranH); i++) //corps
    {
        ts = tmx_get_tileset(tmx_carte, layer->content.gids[(i*tmx_carte->width)+dep_j], (unsigned int*)&(clip.x), (unsigned int*)&(clip.y));
        clip.x += modScrollX;
        clip.w = tmx_carte->tile_width - modScrollX;

        pos.x = rectMap.x; //réinitialisation à chaque début de line
        pos.w = clip.w;

        SDL_RenderCopy(renderer,(SDL_Texture*) ts->image->resource_image,&clip,&pos);//bord gauche

        clip.w = tmx_carte->tile_width;
        pos.x += pos.w;
        pos.w = clip.w;

        for(int j = dep_j + 1; j<min((int)tmx_carte->width,ecranW) ; j++)//corps de l'affichage
        {
            ts = tmx_get_tileset(tmx_carte, layer->content.gids[(i*tmx_carte->width)+j], (unsigned int*)&(clip.x), (unsigned int*)&(clip.y));
            SDL_RenderCopy(renderer,(SDL_Texture*) ts->image->resource_image,&clip,&pos);
            pos.x += pos.w;
        }

        if(ecranW<tmx_carte->width)
        {
            ts = tmx_get_tileset(tmx_carte, layer->content.gids[(i*tmx_carte->width)+ecranW], (unsigned int*)&(clip.x), (unsigned int*)&(clip.y));
            clip.w = modScrollX;
            pos.w = clip.w;
            SDL_RenderCopy(renderer,(SDL_Texture*) ts->image->resource_image,&clip,&pos);
        }

        pos.y += pos.h;
    }



    if(i<tmx_carte->height)
    {
        ts = tmx_get_tileset(tmx_carte, layer->content.gids[(i*tmx_carte->width)+dep_j], (unsigned int*)&(clip.x), (unsigned int*)&(clip.y));
        clip.x += modScrollX; // coin inférieur gauche
        clip.w = tmx_carte->tile_width - modScrollX;
        clip.h = modScrollY;
        pos.x = rectMap.x;
        pos.w = clip.w;
        pos.h = clip.h;
        SDL_RenderCopy(renderer,(SDL_Texture*) ts->image->resource_image,&clip,&pos);


        clip.w = tmx_carte->tile_width;
        pos.x += pos.w;
        pos.w = clip.w;

        for(int j = dep_j + 1; j < min(ecranW,(int)tmx_carte->width); j++) //ligne du bas
        {
            ts = tmx_get_tileset(tmx_carte, layer->content.gids[(i*tmx_carte->width)+j], (unsigned int*)&(clip.x), (unsigned int*)&(clip.y));
            if(SDL_RenderCopy(renderer,(SDL_Texture*) ts->image->resource_image,&clip,&pos)<0)
                cout<<"Erreur dans l'affichage de la map (dernière ligne) : "<<SDL_GetError()<<endl;
            pos.x += pos.w;
        }

        if(ecranW < tmx_carte->width)
            ts = tmx_get_tileset(tmx_carte, layer->content.gids[(i*tmx_carte->width)+ecranW], (unsigned int*)&(clip.x), (unsigned int*)&(clip.y));
        clip.w = modScrollX;
        pos.w = clip.w;
        SDL_RenderCopy(renderer,(SDL_Texture*) ts->image->resource_image,&clip,&pos);//coin inférieur droit
    }

}
