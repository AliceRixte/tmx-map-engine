#include "tme.hpp"

using namespace std;
namespace tme 
{
  TmxME::TmxME(SDL_Renderer *a_renderer, char* fichier, SDL_Rect a_rectMap)
  {
    renderer = a_renderer;//récupération des arguments
    rectMap = a_rectMap;


    //loading the map
    tmxMap = NULL;
    tmx_img_load_func = (void* (*)(const char*))IMG_Load;//set of the img loading function
    tmx_img_free_func = (void  (*)(void*))  SDL_DestroyTexture;//don't use free surface because all the images are then converted to SDL_Texture
    tmxMap = tmx_load(fichier);

    if(!tmxMap)
      cout<<"Error in the map loading : the path to the tmx map is not correct or the file itself is corrupted."<<endl;

    tmx_tileset* ts = tmxMap->ts_head;

    while(ts)//conversion of the tilesets to SDL_Texture
      {
        SDL_Surface* tileset_s = (SDL_Surface*) ts->image->resource_image;
        ts->image->resource_image = SDL_CreateTextureFromSurface(renderer,tileset_s);
        if(ts->image->resource_image)
	  SDL_FreeSurface(tileset_s);
        ts = ts->next;
      }

    ts = tmxMap->ts_head;

    setCameraPosition(0,0);

    //gestion du mouvement
    //posX = posY = movX = movY = NULL;
    //movW = movH = 0;
    //isStoppedX = isStoppedY = true;
  }

}
