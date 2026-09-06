// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************
// Include file for elcompute software

#ifndef GEODETICEARHELCOMPUTE_H
#define GEODETICEARHELCOMPUTE_H

#include "geodata_export.h"

#include <array>
#include <vector>

class GEODATA_EXPORT GeodeticEarthElevCompute
{
public:
   struct Vert
   {
      std::array<float, 3> v;
   };

   struct Face
   {
      int                  nvert;
      Vert*                vert;
      std::array<float, 3> normal;
      float                d;
   };

   struct Subregion
   {
      int   nface;
      Face* faces;
   };

   struct File
   {
      int        nsubregion;
      Subregion* subregions;
   };

   struct Database
   {
      int   nfile;
      float subregsizex, subregsizey;
      int   nsubregx, nsubregy;
      float filesizex, filesizey;
      int   nfilex;
      int   geoflag;
      float swx, swy;
      File* files;
   };

   struct Database_set
   {
      int        numdb;
      float*     xmin;
      float*     xmax;
      float*     ymin;
      float*     ymax;
      Database** dblist;
   };


   GeodeticEarthElevCompute()  = default;
   ~GeodeticEarthElevCompute() = default;

   // Prototypes
   static float elcompute(int initial, char* dbname, float x, float y, float* norm);

   static bool in_test(float x, float y, const Face& face);

   // static void  geotransform(double* vert);

   static void SetValidTerrainData(int aValue) { m_validTerrainData = aValue; }
   static int  ValidTerrainData() { return m_validTerrainData; }

   // static void SetInMeters(int aValue) { m_inMeters = aValue; }
   static int InMeters(); //           { return m_inMeters; }

private:
   static float elcompute_db(int initial, const char* dbname, float x, float y, float* norm, Database*& db);

   static int m_validTerrainData;

   static int iFaceIndex;
   static int jFaceIndex;

   static std::vector<int> m_BadSubregions;
   // static int m_inMeters;
};
#endif
