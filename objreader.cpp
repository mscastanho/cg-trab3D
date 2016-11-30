#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <map>
#include "geometry.h"
#include "misc.h"
#include "objreader.h"

using namespace std;

map<string,Material>* readMTLFile(string filePath){
  string line;
  ifstream input;
  Material m;
  char type[8];
  map<string,Material> *materials = new map<string,Material>();

  input.open(filePath.c_str(),ifstream::in);

  if(input.good()){
    while( !input.eof() ){
      char buffer[100];
      input.getline(buffer,100);

      if(strlen(buffer) == 0)
        continue;

      char* token = strtok(buffer," ");

      if(!strcmp(token,"newmtl")){
        token = strtok(NULL," ");
        strcpy(m.name,token);

        for(int i = 0; i < 3 ; i++){
          input.getline(buffer,100);

          token = strtok(buffer," ");
          strcpy(type,token);

          token = strtok(NULL," ");
          double x = atof(token);

          token = strtok(NULL," ");
          double y = atof(token);

          token = strtok(NULL," ");
          double z = atof(token);

          Point p;
          p.x = x;
          p.y = y;
          p.z = z;

          if(!strcmp(type,"Ka"))
            m.ambient = p;
          else if(!strcmp(type,"Kd"))
            m.diffuse = p;
          else if(!strcmp(type,"Ks"))
            m.specular = p;
        }

        input.getline(buffer,100);
        if(strlen(buffer) != 0){
          token = strtok(buffer," ");

          if(!strcmp(token,"map_Kd")){
            token = strtok(NULL," ");
            strcpy(m.fileName,token);
          }
        }

        materials->insert(make_pair(string(m.name),m));
      }

    }
  }

  input.close();
  return materials;

}
