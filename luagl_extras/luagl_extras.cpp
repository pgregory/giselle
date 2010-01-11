#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#endif
#if defined (__APPLE__) || defined (OSX)
	#include <OpenGL/gl.h>
	#include <OpenGL/glu.h>
#else
	#include <GL/gl.h>
	#include <GL/glu.h>
#endif

#include <lua.h>
#include <lauxlib.h>

#include "luagl_extras.h"
#include "luagl_util.h"


static const luaglConst luagl_extras_const[] = {
   {0, 0}
};

#define PI 3.1412

/* Sphere() -> none */
static int luagl_extras_partialsphere(lua_State *L)
{
    if(!( lua_isnumber(L, 1) && lua_isnumber(L, 2) && lua_isnumber(L, 3) && lua_isnumber(L, 4) && lua_isnumber(L, 5)))
	{
        luaL_error(L, "incorrect argument to function 'gle.PartialSphere'");
		return 0;
	}
    GLfloat radius = lua_tonumber(L, 1);
    GLfloat thetamin = lua_tonumber(L, 2);
    GLfloat thetamax = lua_tonumber(L, 3);
    GLfloat zmin = lua_tonumber(L, 4);
    GLfloat zmax = lua_tonumber(L, 5);

    GLfloat thetan = ceil(std::max(3.0f, ((thetamax - thetamin)/360.0f) * 18.0f));
    GLfloat phimin = -90.0f;
    GLfloat phimax = 90.0f;
    if(zmin > -radius)
        phimin = asin(zmin/radius)*(180.0f/PI);
    if(zmax < radius)
        phimax = asin(zmax/radius)*(180.0f/PI);

    GLfloat phin = ceil(std::max(3.0f, ((phimax - phimin)/180.0f) * 18.0f));
    GLfloat phi1 = std::max(phimin, -90.0f)*(PI/180.0f);
    GLfloat phi2 = std::min(phimax, 90.0f)*(PI/180.0f);
    GLfloat theta1 = std::max(thetamin, 0.0f)*(PI/180.0f);
    GLfloat theta2 = std::min(thetamax, 360.0f)*(PI/180.0f);
    for(int j = 0; j < phin; ++j)
    {
        GLfloat t1 = phi1 + j * (phi2 - phi1) / phin;
        GLfloat t2 = phi1 + (j + 1) * (phi2 - phi1) / phin;
        glBegin(GL_QUAD_STRIP);
        for(int i = 0; i <= thetan; ++i)
        {
            GLfloat t3 = theta1 + i * (theta2 - theta1) / thetan;

            GLfloat ex = cos(t1) * cos(t3);
            GLfloat ey = cos(t1) * sin(t3);
            GLfloat ez = sin(t1);
            GLfloat px = radius * ex;
            GLfloat py = radius * ey;
            GLfloat pz = radius * ez;
            glNormal3f(ex, ey, ez);
            glTexCoord2f((GLfloat)i/thetan, (GLfloat)j/phin);
            glVertex3f(px, py, pz);

            ex = cos(t2) * cos(t3);
            ey = cos(t2) * sin(t3);
            ez = sin(t2);
            px = radius * ex;
            py = radius * ey;
            pz = radius * ez;
            glNormal3f(ex, ey, ez);
            glTexCoord2f((GLfloat)i/thetan, (GLfloat)(j+1)/phin);
            glVertex3f(px, py, pz);
        }
        glEnd();
    }

	return 0;
}	

static const luaL_reg luagl_extras_lib[] = {
  {"PartialSphere", luagl_extras_partialsphere},
  {NULL, NULL}
};


int luaopen_luagl_extras(lua_State *L)
{
  luaL_openlib(L, "gle", luagl_extras_lib, 0);

  luagl_initconst(L, luagl_extras_const);

  return 1;
}
