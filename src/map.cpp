#include "map.hpp"
#include "math.h"

using namespace std;

//**************************************Gestion des paramètres de la map************************************************************




void Map::getDimTile(int* w, int* h)
{
    (*w) = tmx_carte->tile_width;
    (*h) = tmx_carte->tile_height;
}

void Map::getFirstRowPos(int* x, int* y)
{
    tmx_property* property = tmx_carte->properties;
    bool foundX = false;
    bool foundY = false;
    while(property && (!foundX || !foundY))
    {
        if(!foundX && strcmp(property->name,"first_row_pos_x") == 0)
        {
            foundX = true;
            (*x) = atoi(property->value) * tmx_carte->tile_width;
        }
        else if(!foundY && strcmp(property->name,"first_row_pos_y") == 0)
        {
            foundY = true;
            (*y) = atoi(property->value) * tmx_carte->tile_height;
        }
        property = property->next;
    }
    if(!foundX || !foundY)
        cout<<"Erreur : la position de la première rangée n'a pas été trouvée dans les propriétés de la map."<<endl;
}

float mod_float(float a, int b) // cette fonction permet de calculer le modulo d'un flotant par un entier sans perte de précision
{
    float partie_fractionnaire = a - floor(a);
    int mod_int = (int)floor(a) % b;
    return (float) mod_int + partie_fractionnaire;
}


Map::Map(SDL_Renderer *a_renderer, char* fichier, SDL_Rect a_rectMap)
{
    renderer = a_renderer;//récupération des arguments
    rectMap = a_rectMap;



    //initialisation de horsMap
    horsMap = (tmx_property*) malloc(sizeof(tmx_property));
    horsMap->name = "bord_map";
    horsMap->value = "";
    horsMap->next = NULL;

    //chargement de la map
    tmx_carte = NULL;
    tmx_img_load_func = (void* (*)(const char*))IMG_Load;// à remplacer par la loadTextureImg
    tmx_img_free_func = (void  (*)(void*))      SDL_DestroyTexture;//on transforme tout par la suite en surface

    tmx_carte = tmx_load(fichier);
    if(!tmx_carte)
        cout<<"Erreur dans le chargement de la map : chemin du fichier tmx non correct ou map incorrecte. Voir map pour plus d'info."<<endl;

    tmx_tileset* ts = tmx_carte->ts_head;

    while(ts)//conversion des tilesets
    {
        SDL_Surface* tileset_s = (SDL_Surface*) ts->image->resource_image;
        ts->image->resource_image = SDL_CreateTextureFromSurface(renderer,tileset_s);
        if(ts->image->resource_image)
            SDL_FreeSurface(tileset_s);
        ts = ts->next;
    }

    ts = tmx_carte->ts_head;
    //à changer (devrait appartenir à personnage principal
    posInitX = 0;
    posInitY = 160;

    setPosCam(posInitX,posInitY);

    vector<int> m1(tmx_carte->width,-1);
    for(int i =0; i<tmx_carte->height;i++)
        rows.push_back(m1);

    //gestion du mouvement
    posX = posY = movX = movY = NULL;
    movW = movH = 0;
    isStoppedX = isStoppedY = true;
}


//**********************************Gestion des collisions et du mouvement******************************



Map::~Map()
{
    free(horsMap);
    tmx_map_free(tmx_carte);
}
