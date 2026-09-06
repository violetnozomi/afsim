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


#ifndef GEODETICEARTHDATABASE_H
#define GEODETICEARTHDATABASE_H

#include "geodata_export.h"

#include <map>
#include <string>

class GEODATA_EXPORT GeodeticEarthDatabase
{
public:
   typedef struct
   {
      int    mapType;
      int    padding;
      double originLat, originLon;
      double feetPerDegLat, feetPerDegLon;
   } latlon_struct_type;

   enum
   {
      NO_DATABASE             = 0,
      TERRAIN_MAP_DATABASE    = 1,
      CS_EDWARDS_DATABASE     = 3,
      EDWARDS_DATABASE        = 4,
      WISCONSIN_DATABASE      = 5,
      HUNTER_LIGGETT_DATABASE = 6,
      BOSNIA_DATABASE         = 7,
      GCS_DATABASE            = 11,
      OLD_KOREA_DATABASE      = 11,
      UTAH_DATABASE           = 12, // not really defined, but gave it a value
      EDWARDS3_DATABASE       = 14,
      IEIST_DATABASE          = 15,
      STL_DATABASE            = 16,
      MIDEAST_DATABASE        = 17,
      NEVADA_DATABASE         = 18,
      KOREA_DATABASE          = 19,
      ELMENDORF_DATABASE      = 20,
      HORMUZ_DATABASE         = 21,
      SSWA_DATABASE           = 22,
      CASPIAN_EAFB4_DATABASE  = 23,
      BAGHDAD_DATABASE        = 24,
      AFGHANISTAN_DATABASE    = 25,
      METERS_DATABASE         = 26,
      FEET_DATABASE           = 27
   };


   GeodeticEarthDatabase();
   ~GeodeticEarthDatabase() {}

   void initLatLonRefValues(double ref_lat, double ref_lon, double ref_mpdla);

   void get_database_values(int database, double* originLat, double* originLon, double* feetPerDegLat);

   void GetLatLon(double posX,
                  double posY,
                  int    database,
                  int*   degLat,
                  int*   minLat,
                  float* secLat,
                  int*   degLon,
                  int*   minLon,
                  float* secLon);

   void GenTotalLatLon(double posX, double posY, int database, double* degLat, double* degLon);

   void GenFeet(double* posX, double* posY, int database, double totalLat, double totalLon);

   int                 GetDatabase() { return m_database; }
   static unsigned int GetDBType() { return m_dbType; }

   double GetDatabaseLat() { return m_dbLat; }
   double GetDatabaseLon() { return m_dbLon; }
   double GetDatabaseFtpdla() { return m_ftpdla; }
   double GetDatabaseFtpdlo() { return m_ftpdlo; }

   void SetDatabase(std::string aDBName);
   void SetGeodeticEarthFilePathname(std::string& aPathname);
   void SetClampAltAboveTerrain(double aAlt) { m_clampAltAboveTerrain = aAlt; }
   void SetDBType(unsigned int aType) { m_dbType = aType; }
   void SetRefLatLon(double aLatitude, double aLongitude);

   bool OpenTheDatabase();

   static bool                         InMeters();
   static void                         SetDBNames();
   static bool                         ValidateDBName(std::string aGeodeticEarthDBName);
   static std::map<std::string, bool>* GetDatabaseNamesMap() { return &m_databaseNames; }

   static bool EncounteredError() { return m_EncounteredError; }
   static void SetEncounteredError(bool aBool) { m_EncounteredError = aBool; }

   static std::string GetDatabaseName() { return m_databaseName; }

private:
   double m_dbLat;
   double m_dbLon;
   double m_ftpdla;
   double m_ftpdlo;
   double m_clampAltAboveTerrain;
   double mRefLat;
   double mRefLon;

   latlon_struct_type clla;

   int                 m_database;
   static unsigned int m_dbType;

   static std::string                 m_databaseName;
   std::string                        m_pathNameOrFile;
   static std::map<std::string, bool> m_databaseNames;
   static bool                        m_EncounteredError;
};
#endif
