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

#ifndef VISUALDATABASE_HPP
#define VISUALDATABASE_HPP

#include "geodata_export.h"

#include <mutex>
#include <vector>

class GEODATA_EXPORT VisualDatabase
{
public:
   VisualDatabase()
      : mDBset()
      , mLastFace(nullptr)
      , mCacheLock(){};

   ~VisualDatabase();

   int Initialize(const char* aDBName);

   int ComputeElev(double aLat, double aLon, float& aElev, double aNorm[3]);

private:
   struct Vert;
   struct Face;
   struct Subregion;
   struct File;
   struct Database;
   struct Database_set;

   // prevent Sun compiler error regarding nested access
   friend struct Vert;
   friend struct Face;
   friend struct Subregion;
   friend struct File;
   friend struct Database;
   friend struct Database_set;

   struct Vert
   {
      float v[3];
   };

   struct Face
   {
      int               nvert;
      std::vector<Vert> vert;
      float             normal[3];
      float             d;
   };

   struct Subregion
   {
      int               nface;
      std::vector<Face> faces;
   };

   struct File
   {
      int                    nsubregion;
      std::vector<Subregion> subregions;
   };

   struct Database
   {
      int               nfile;
      float             subregsizex, subregsizey;
      int               nsubregx, nsubregy;
      float             filesizex, filesizey;
      int               nfilex;
      int               geoflag;
      float             swx, swy;
      std::vector<File> files;
   };

   struct Database_set
   {
      int                    numdb;
      std::vector<float>     xmin;
      std::vector<float>     xmax;
      std::vector<float>     ymin;
      std::vector<float>     ymax;
      std::vector<Database*> dblist;
   };

   Database_set mDBset;
   Face*        mLastFace;

   static int InitializeDB(const char* aDBName, Database* aDB);

   int ComputeElevDB(double aLat, double aLon, float& aElev, double aNorm[3], Database* aDB);

   static bool PointInFace(double aLat, double aLon, const Face& aFace);

   std::recursive_mutex mCacheLock;
};


#endif
