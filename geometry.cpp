#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>

typedef struct
{
    //Vertex coordinate
    double X;
    double Y;
    double Z;

    //Vertex normal
    double nX;
    double nY;
    double nZ;

    //Vertex texture coordinate
    double U;
    double V;
} VERTICES;

typedef struct
{
    VERTICES * vtx;
    int numVtx;
    double radius;
} OBJ;

OBJ * CreateSphere (double R, double space)
{
    OBJ *obj = new OBJ;

    obj->numVtx = (180 / space) *
                  (2 + 360 / (2*space)) * 4;
    obj->vtx = new VERTICES[ obj->numVtx ];
    obj->radius = R;

    int n;
    double vR, lVR;
    double hR, lHR;
    double norm;
    n = 0;
    for( vR = 0; vR <= 180-space; vR+=space){
        for(hR = 0; hR <= 360+2*space; hR+=2*space)
        {
            lVR = vR;
            lHR = hR;
            obj->vtx[n].X = R *
                    sin(lHR / 180 * M_PI) *
                    sin(lVR / 180 * M_PI);
            obj->vtx[n].Y = R *
                    cos(lHR / 180 * M_PI) *
                    sin(lVR / 180 * M_PI);
            obj->vtx[n].Z = R *
                    cos(lVR / 180 * M_PI);
            obj->vtx[n].V = lVR / 180;
            obj->vtx[n].U = lHR / 360;
            norm = sqrt(
                    obj->vtx[n].X*obj->vtx[n].X+
                    obj->vtx[n].Y*obj->vtx[n].Y+
                    obj->vtx[n].Z*obj->vtx[n].Z);
            obj->vtx[n].nX = obj->vtx[n].X/norm;
            obj->vtx[n].nY = obj->vtx[n].Y/norm;
            obj->vtx[n].nZ = obj->vtx[n].Z/norm;
            n++;

            lVR = vR + space;
            lHR = hR;
            obj->vtx[n].X = R * sin(lHR / 180 * M_PI) * sin(lVR / 180 * M_PI);
            obj->vtx[n].Y = R * cos(lHR / 180 * M_PI) * sin(lVR / 180 * M_PI);
            obj->vtx[n].Z = R * cos(lVR / 180 * M_PI);
            obj->vtx[n].V = lVR / 180;
            obj->vtx[n].U = lHR / 360;
            norm = sqrt(obj->vtx[n].X*obj->vtx[n].X+obj->vtx[n].Y*obj->vtx[n].Y+obj->vtx[n].Z*obj->vtx[n].Z);
            obj->vtx[n].nX = obj->vtx[n].X/norm;
            obj->vtx[n].nY = obj->vtx[n].Y/norm;
            obj->vtx[n].nZ = obj->vtx[n].Z/norm;
            n++;

            lVR = vR;
            lHR = hR + space;
            obj->vtx[n].X = R * sin(lHR / 180 * M_PI) * sin(lVR / 180 * M_PI);
            obj->vtx[n].Y = R * cos(lHR / 180 * M_PI) * sin(lVR / 180 * M_PI);
            obj->vtx[n].Z = R * cos(lVR / 180 * M_PI);
            obj->vtx[n].V = lVR / 180;
            obj->vtx[n].U = lHR / 360;
            norm = sqrt(obj->vtx[n].X*obj->vtx[n].X+obj->vtx[n].Y*obj->vtx[n].Y+obj->vtx[n].Z*obj->vtx[n].Z);
            obj->vtx[n].nX = obj->vtx[n].X/norm;
            obj->vtx[n].nY = obj->vtx[n].Y/norm;
            obj->vtx[n].nZ = obj->vtx[n].Z/norm;
            n++;

            lVR = vR + space;
            lHR = hR + space;
            obj->vtx[n].X = R * sin(lHR / 180 * M_PI) * sin(lVR / 180 * M_PI);
            obj->vtx[n].Y = R * cos(lHR / 180 * M_PI) * sin(lVR / 180 * M_PI);
            obj->vtx[n].Z = R * cos(lVR / 180 * M_PI);
            obj->vtx[n].V = lVR / 180;
            obj->vtx[n].U = lHR / 360;
            norm = sqrt(obj->vtx[n].X*obj->vtx[n].X+obj->vtx[n].Y*obj->vtx[n].Y+obj->vtx[n].Z*obj->vtx[n].Z);
            obj->vtx[n].nX = obj->vtx[n].X/norm;
            obj->vtx[n].nY = obj->vtx[n].Y/norm;
            obj->vtx[n].nZ = obj->vtx[n].Z/norm;
            n++;
        }
    }
    return obj;
}

void drawSky(float radius, float space, GLuint texture)
{
    OBJ* sky = CreateSphere(radius,space);

    GLfloat materialEmission[] = { 0.10, 0.10, 0.10, 1};
    GLfloat materialColorA[] = { 0.2, 0.2, 0.2, 1};
    GLfloat materialColorD[] = { 1.0, 1.0, 1.0, 1};
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1};
    GLfloat mat_shininess[] = { 100.0 };
    glColor3f(1,1,1);

    glMaterialfv(GL_FRONT, GL_EMISSION, materialEmission);
    glMaterialfv(GL_FRONT, GL_AMBIENT, materialColorA);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, materialColorD);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    glBindTexture (GL_TEXTURE_2D, texture);
    glBegin (GL_TRIANGLE_STRIP);
    for ( int i = 0; i <sky->numVtx; i++)
    {
        glNormal3f(-sky->vtx[i].nX, -sky->vtx[i].nY, -sky->vtx[i].nZ);
        glTexCoord2f (sky->vtx[i].U, sky->vtx[i].V);
        glVertex3f (sky->vtx[i].X, sky->vtx[i].Y, sky->vtx[i].Z);
    }
    glEnd();
}

void drawFinishLine(){

  
}
