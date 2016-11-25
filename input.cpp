#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <time.h>
#include <GL/glut.h>
#include "input.h"
#include "misc.h"
#include "tinyxml2.h"
#include "circle.h"
#include "rect.h"
#include "car.h"
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <png.h>

using namespace tinyxml2;
using namespace std;

extern vector<Car*> enemies;
extern Circle* arenaOut;
extern Circle* arenaIn;
extern Car* player;
extern Rect* startEnd;
extern float PLAYER_SPEED;
extern float PLAYER_BULLET_SPEED;
extern float PLAYER_SIZE;
extern float ENEMY_SPEED;
extern float ENEMY_SHOT_FREQ;
extern float ENEMY_BULLET_SPEED;

string parseConfigFile (string filePath){

  XMLDocument configFile;

  configFile.LoadFile(filePath.c_str());

	if(configFile.ErrorID() != 0)
		cout << "Erro no arquivo de entrada config.xml" << endl;
	else{

		XMLElement* adaElement = configFile.FirstChildElement("aplicacao")->FirstChildElement("arquivoDaArena");

    string name = adaElement->Attribute("nome");

    string type = adaElement->Attribute("tipo");

    string path = adaElement->Attribute("caminho");

    XMLElement* carElement = configFile.FirstChildElement("aplicacao")->FirstChildElement("carro");

    carElement->QueryFloatAttribute("velCarro",&PLAYER_SPEED);
    carElement->QueryFloatAttribute("velTiro",&PLAYER_BULLET_SPEED);

    XMLElement* enemyElement = configFile.FirstChildElement("aplicacao")->FirstChildElement("carroInimigo");

    enemyElement->QueryFloatAttribute("freqTiro",&ENEMY_SHOT_FREQ);
    enemyElement->QueryFloatAttribute("velCarro",&ENEMY_SPEED);
    enemyElement->QueryFloatAttribute("velTiro",&ENEMY_BULLET_SPEED);

    return path + name + "." + type;
  }

  return "";
}

Color parseColor(string color){

  if(color == "black")
    return BLACK;
  else if (color == "blue")
    return BLUE;
  else if (color == "green")
    return GREEN;
  else if (color == "cyan")
    return CYAN;
  else if (color == "red")
    return RED;
  else if (color == "magenta")
    return MAGENTA;
  else if (color == "yellow")
    return YELLOW;

    return WHITE;
}

void parseCircle(XMLElement* c){

  float cx,cy,r;
  c->QueryFloatAttribute("cx",&cx);
  c->QueryFloatAttribute("cy",&cy);
  c->QueryFloatAttribute("r",&r);

  string fill = c->Attribute("fill");
  string id = c->Attribute("id");

  Point center = {cx,cy};
  Color fillColor = parseColor(fill);

  Circle* circle = new Circle(center,r,fillColor);

  if(id == "Pista"){
    if(arenaOut == NULL)
      arenaOut = circle;
    else{
      if(arenaOut->get_radius() > circle->get_radius())
        arenaIn = circle;
      else{
        arenaIn = arenaOut;
        arenaOut = circle;
      }
    }
  }else if(id == "Inimigo"){

    //We define an enemy's initial angle later
    Car* enemy = new Car(center,r,RED,0,0,0);
    enemies.push_back(enemy);

  }else if(id == "Jogador"){
    player = new Car(center,r,GREEN,0,0,0);

  }
}

void parseRect(XMLElement* r){

  float x,y,width,height;
  r->QueryFloatAttribute("x",&x);
  r->QueryFloatAttribute("y",&y);
  r->QueryFloatAttribute("width",&width);
  r->QueryFloatAttribute("height",&height);

  string fill = r->Attribute("fill");
  string id = r->Attribute("id");

  Color fillColor = parseColor(fill);

  Rect* rect = new Rect(x,y,width,height,fillColor);

  startEnd = rect;
}


void parseSVGFile (string filePath){

  XMLDocument arenaFile;

  // Treat case when the filePath has '~'
  if(filePath[0] == '~'){
    filePath.erase(0,1);
    struct passwd *pw = getpwuid(getuid());
    string userHome = string(pw->pw_dir);
    filePath = userHome + filePath;

  }
  arenaFile.LoadFile(filePath.c_str());

  if(arenaFile.ErrorID() != 0)
		cout << "Erro no arquivo de entrada config.xml" << endl;
	else{

		XMLElement* svgElement = arenaFile.FirstChildElement("svg");

    XMLElement* next = svgElement->FirstChildElement();

    while(next != NULL){

      string tagType = next->Value();

      if(tagType == "rect"){
          parseRect(next);
      }else if (tagType == "circle"){
          parseCircle(next);
      }

      next = next->NextSiblingElement();

    }

  }
}

// Function to load png files as textures
// By David Grayson from Stack Overflow
// Link: http://stackoverflow.com/questions/11296644/loading-png-textures-to-opengl-with-libpng-only
GLuint png_texture_load(const char * file_name, int * width, int * height)
{
    png_byte header[8];

    FILE *fp = fopen(file_name, "rb");
    if (fp == 0)
    {
        perror(file_name);
        return 0;
    }

    // read the header
    fread(header, 1, 8, fp);

    if (png_sig_cmp(header, 0, 8))
    {
        fprintf(stderr, "error: %s is not a PNG.\n", file_name);
        fclose(fp);
        return 0;
    }

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
    {
        fprintf(stderr, "error: png_create_read_struct returned 0.\n");
        fclose(fp);
        return 0;
    }

    // create png info struct
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        fprintf(stderr, "error: png_create_info_struct returned 0.\n");
        png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
        fclose(fp);
        return 0;
    }

    // create png info struct
    png_infop end_info = png_create_info_struct(png_ptr);
    if (!end_info)
    {
        fprintf(stderr, "error: png_create_info_struct returned 0.\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
        fclose(fp);
        return 0;
    }

    // the code in this if statement gets called if libpng encounters an error
    if (setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stderr, "error from libpng\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        fclose(fp);
        return 0;
    }

    // init png reading
    png_init_io(png_ptr, fp);

    // let libpng know you already read the first 8 bytes
    png_set_sig_bytes(png_ptr, 8);

    // read all the info up to the image data
    png_read_info(png_ptr, info_ptr);

    // variables to pass to get info
    int bit_depth, color_type;
    png_uint_32 temp_width, temp_height;

    // get info about png
    png_get_IHDR(png_ptr, info_ptr, &temp_width, &temp_height, &bit_depth, &color_type,
        NULL, NULL, NULL);

    if (width){ *width = temp_width; }
    if (height){ *height = temp_height; }

    // Update the png info struct.
    png_read_update_info(png_ptr, info_ptr);

    // Row size in bytes.
    int rowbytes = png_get_rowbytes(png_ptr, info_ptr);

    // glTexImage2d requires rows to be 4-byte aligned
    rowbytes += 3 - ((rowbytes-1) % 4);

    // Allocate the image_data as a big block, to be given to opengl
    png_byte * image_data;
    image_data = (png_byte*) malloc(rowbytes * temp_height * sizeof(png_byte)+15);
    if (image_data == NULL)
    {
        fprintf(stderr, "error: could not allocate memory for PNG image data\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        fclose(fp);
        return 0;
    }

    // row_pointers is for pointing to image_data for reading the png with libpng
    png_bytep * row_pointers = (png_byte**) malloc(temp_height * sizeof(png_bytep));
    if (row_pointers == NULL)
    {
        fprintf(stderr, "error: could not allocate memory for PNG row pointers\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        free(image_data);
        fclose(fp);
        return 0;
    }

    // set the individual row_pointers to point at the correct offsets of image_data
    int i;
    for (i = 0; i < temp_height; i++)
    {
        row_pointers[temp_height - 1 - i] = image_data + i * rowbytes;
    }

    // read the png into image_data through row_pointers
    png_read_image(png_ptr, row_pointers);

    // Generate the OpenGL texture object
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, temp_width, temp_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // clean up
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    free(image_data);
    free(row_pointers);
    fclose(fp);
    return texture;
}
