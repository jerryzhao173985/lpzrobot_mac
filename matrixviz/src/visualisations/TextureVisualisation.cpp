/***************************************************************************
 *   Copyright (C) 2008-2011 LpzRobots development team                    *
 *    Antonia Siegert (original author)                                  *
 *    Georg Martius  <georg dot martius at web dot de>                     *
 *    Frank Guettler <guettler at informatik dot uni-leipzig dot de        *
 *    Frank Hesse    <frank at nld dot ds dot mpg dot de>                  *
 *    Ralf Der       <ralfder at mis dot mpg dot de>                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 *                                                                         *
 ***************************************************************************/

#include "TextureVisualisation.h"
#include "math.h"
#include <iostream>
#include <string>


using namespace std;

TextureVisualisation::TextureVisualisation(MatrixPlotChannel *channel, ColorPalette *colorPalette, QWidget *parent)
: AbstractVisualisation(channel, colorPalette, parent){

  if(debug) cout << "TextureVisualisation Konstruktor" << endl;
  this->channel = channel;
  this->colorPalette = colorPalette;
  object = 0;
  maxX = channel->getDimension(0);
  maxY = channel->getDimension(1);
  //setUpdatesEnabled(true);
  setMouseTracking(true); // enables tooltips while mousemoving over widget
}

TextureVisualisation::~TextureVisualisation(){
  if(debug) cout << "TextureVisualisation Destruktor" << endl;
  if(object != 0) {
    makeCurrent();
    glDeleteLists( object, 1 );
  }
}

void TextureVisualisation::initializeGL(){
  if(debug) cout << "TextureVisualisation initializeGL" << endl;
  initializeOpenGLFunctions();  // Qt5: Initialize OpenGL functions
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);    // Let OpenGL clear to black
  object = makeObject();    // Generate an OpenGL display list
  glShadeModel( GL_FLAT );

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glGenTextures(1, &texName);
  glBindTexture(GL_TEXTURE_2D, texName);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  //initialize texture
  // Georg: das geht mit memset
  for(int i = 0; i < texSize; i++)
    for(int j = 0; j < texSize; j++)
      for(int k = 0; k < 3; k++) tex[i][j][k] = (GLubyte) 255;

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texSize, texSize, 0, GL_RGB, GL_UNSIGNED_BYTE, tex);
}

void TextureVisualisation::resizeGL(int w, int h){
  if(debug) cout << "TextureVisualisation resizeGL" << endl;
  glViewport(0, 0, (GLint) w, (GLint) h);
  glMatrixMode( GL_PROJECTION);
  glLoadIdentity();
  glFrustum(-1.0, 1.0, -1.0, 1.0, 1.0, 1.0);
  glMatrixMode( GL_MODELVIEW);
}

void TextureVisualisation::paintGL(){
  if(debug) cout << "TextureVisualisation PaintGL" << endl;
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  glLoadIdentity();
  glBindTexture(GL_TEXTURE_2D, texName);

  GLubyte subTex[maxX][maxY][3];
  for (int i = 0; i < maxX; i++)
    for (int j = 0; j < maxY; j++) {
      QColor color;
      color = colorPalette->pickColor(clip(colorPalette->getScaledValue(channel->getValue(i, j))));
      if(debug) cout << "at pickColor i: " << i << ", " << j << "\t val: " << channel->getValue(i, j) << " \t red:" << color.red() <<endl;

      subTex[i][j][0] = (GLubyte) color.red();
      subTex[i][j][1] = (GLubyte) color.green();
      subTex[i][j][2] = (GLubyte) color.blue();
    }

  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                           maxY, maxX, GL_RGB,
                           GL_UNSIGNED_BYTE, subTex);
  glCallList( object );
}

double TextureVisualisation::clip(double val){
  if( val > colorPalette->getMax())
    return colorPalette->getMax();
  if( val < colorPalette->getMin())
    return colorPalette->getMin();
  else
    return val;
}

GLuint TextureVisualisation::makeObject() {
  if(debug) cout << "TextureVisualisation makeObject" << endl;
  GLuint list;

  list = glGenLists(1);

  glNewList(list, GL_COMPILE);

  glEnable(GL_TEXTURE_2D);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  //glBindTexture(GL_TEXTURE_2D, texName);
  double onePixel = 1./texSize;
  glBegin( GL_QUADS); // Draw A Quadab
  glTexCoord2d(0.0,0.0);glVertex2f(-1.0f, 1.0f); // Top Left
  glTexCoord2d(onePixel*maxY,0.0);glVertex2f(1.0f, 1.0f); // Top Right
  glTexCoord2d(onePixel*maxY,onePixel*maxX);glVertex2f(1.0f, -1.0f); // Bottom Right
  glTexCoord2d(0.0,onePixel*maxX);glVertex2f(-1.0f, -1.0f); // Bottom Left
  glEnd(); // Done Drawing The Quad
  glFlush();
  glDisable(GL_TEXTURE_2D);
  glEndList();

  return list;
}


void TextureVisualisation::mouseMoveEvent ( QMouseEvent *event ){
  QString tTip;
  // Our maxX and maxY are transposed to the screen coords
  double xStep = width() / maxY;
  double yStep = height() / maxX;
  int n= (int) (event->y() / yStep);
  int m= (int) (event->x() / xStep);
  // Georg: we need clipping to avoid access to out of range fields (the event can be negative and larger then width and hight)
  if(n >= maxX) n=maxX-1;
  if(n < 0) n=0;
  if(m >= maxY) m=maxY-1;
  if(m < 0) m=0;

  VectorPlotChannel *vectorPC = dynamic_cast<VectorPlotChannel *> (channel);
  if ( vectorPC == NULL){
    MatrixElementPlotChannel *elem = channel->getChannel(n, m);
    tTip = QString(elem->getChannelName().c_str()) + ": " + QString::number(elem->getValue());

  }else{
    VectorElementPlotChannel * elem = vectorPC->getChannel(n);
    //    tTip += QString(elem->getChannelName().c_str()) + ", " + QString::number(m) + ": " + QString::number(elem->getValue());
    // Georg: m is not used here
    tTip += QString(elem->getChannelName().c_str()) + ": " + QString::number(elem->getValue());
  }
  setToolTip((const QString) tTip);  // shows up ToolTip "M[x,y]"
}
