#version 150
// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

in float oStateMain; //already normalized by geom shader from 0 to 1
in float oStateEdge; //already normalized by geom shader from 0 to 1
in float edgeSide;//from -1 to 1
in float oIsAlive;//1 for is alive otherwise the alpha value for dead transparency
uniform vec4 mainMaxColor;
uniform vec4 mainMinColor;
uniform vec4 edgeMaxColor;
uniform vec4 edgeMinColor;
uniform vec4 mainMaxFlippedColor;
uniform vec4 mainMinFlippedColor;
uniform vec4 edgeMaxFlippedColor;
uniform vec4 edgeMinFlippedColor;

#pragma import_defines (HIGHLIGHTING)
#ifdef HIGHLIGHTING
#define EXTRA_WIDTH_FOR_HIGHLIGHT 0.35f
#define HIGHLIGHTED_WIDTH         1.35f
uniform vec4 highlightColor;
#endif

uniform float hardEdgeWidth; //will display the color of the edge
uniform float softEdgeWidth; //will display a mix of main and edge colors

void main(void)
{
   #ifdef HIGHLIGHTING
      if(abs(edgeSide)>1){
         //set color to the highlightColor and add a transparency gradient
         gl_FragColor = highlightColor;
         //first third of highlighted color is full highlight color, rest is a
         //transparency gradient down to fully transparent.
         gl_FragColor.a *= min((EXTRA_WIDTH_FOR_HIGHLIGHT-(abs(edgeSide)-1.0f) ) / (EXTRA_WIDTH_FOR_HIGHLIGHT/1.5f),1);
         return;
      }
   #endif

   vec4 mainCol, edgeCol;
   if(gl_FrontFacing){
      mainCol=mix(mainMinColor,mainMaxColor,oStateMain);
      edgeCol=mix(edgeMinColor,edgeMaxColor,oStateEdge);
   }else{
      mainCol=mix(mainMinFlippedColor,mainMaxFlippedColor,oStateMain);
      edgeCol=mix(edgeMinFlippedColor,edgeMaxFlippedColor,oStateEdge);
   }


   if(abs(edgeSide)<1-hardEdgeWidth-softEdgeWidth){
      //rendering the center/main color of the ribbon
      gl_FragColor=mainCol;//mix(mainMinColor,mainMaxColor,oStateMain);
   }else if(abs(edgeSide)>=1-hardEdgeWidth){
      //rendering the center/main color of the ribbon
      gl_FragColor=edgeCol;//mix(edgeMinColor,edgeMaxColor,oStateEdge);
   }else{
      //rendering transition between edge and main color
      //this implementation of mixing edge and main color is meant to act as a form of anti-aliasing
      gl_FragColor=mix(mainCol,edgeCol,(abs(edgeSide)-(1-hardEdgeWidth-softEdgeWidth))/(softEdgeWidth));
   }
   gl_FragColor.a*=oIsAlive;
}
