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

#include "GeodeticEarthDatabase.hpp"

#include <cstring>
#include <fstream>
#include <iomanip>
#include <math.h>

#include "GeodeticEarthElevCompute.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"

#define BEARTH 20855591.0

std::map<std::string, bool> GeodeticEarthDatabase::m_databaseNames;
std::string                 GeodeticEarthDatabase::m_databaseName     = "";
unsigned int                GeodeticEarthDatabase::m_dbType           = 2; // round earth; 1 = flat earth
bool                        GeodeticEarthDatabase::m_EncounteredError = false;

GeodeticEarthDatabase::GeodeticEarthDatabase()
{
   m_dbLat                = 35.0;
   m_dbLon                = -118.0;
   m_ftpdla               = (double)(111120.0 * UtMath::cFT_PER_M);
   m_ftpdlo               = (double)(91024.175 * UtMath::cFT_PER_M);
   m_clampAltAboveTerrain = 0.0;

   mRefLat = 0;
   mRefLon = 0;

   clla.mapType       = -1;
   clla.padding       = 0;
   clla.originLat     = 0.0;
   clla.originLon     = 0.0;
   clla.feetPerDegLat = 0.0;
   clla.feetPerDegLon = 0.0;
   m_database         = 4; // default to EDWARDS 4 Database
   // m_dbType   = 2;   // round earth
   m_databaseName = "EDWARDS";
}

void GeodeticEarthDatabase::SetDBNames()
{
   // in meters will be true
   m_databaseNames.insert(std::make_pair("TERRAIN_MAP", false));
   m_databaseNames.insert(std::make_pair("CS_EDWARDS", false));
   m_databaseNames.insert(std::make_pair("EDWARDS", true));
   m_databaseNames.insert(std::make_pair("WISCONSIN", false));
   m_databaseNames.insert(std::make_pair("HUNTER_LIGGETT", false));
   m_databaseNames.insert(std::make_pair("BOSNIA", false));
   m_databaseNames.insert(std::make_pair("GCS", false));
   m_databaseNames.insert(std::make_pair("OLD_KOREA", false));
   m_databaseNames.insert(std::make_pair("UTAH", false));
   m_databaseNames.insert(std::make_pair("EDWARDS3", false));
   m_databaseNames.insert(std::make_pair("IEIST", false));
   m_databaseNames.insert(std::make_pair("STL", false));
   m_databaseNames.insert(std::make_pair("MIDEAST", true));
   m_databaseNames.insert(std::make_pair("NEVADA", true));
   m_databaseNames.insert(std::make_pair("KOREA", true));
   m_databaseNames.insert(std::make_pair("ELMENDORF", true));
   m_databaseNames.insert(std::make_pair("HORMUZ", true));
   m_databaseNames.insert(std::make_pair("SSWA", true));
   m_databaseNames.insert(std::make_pair("CASPIAN", true));
   m_databaseNames.insert(std::make_pair("BAGHDAD", false));
   m_databaseNames.insert(std::make_pair("AFGHANISTAN", false));
   m_databaseNames.insert(std::make_pair("DB_IN_METERS", true));
   m_databaseNames.insert(std::make_pair("DB_IN_FEET", false));
}

// This function will initialize necessary reference variables for
// use in the compute functions
void GeodeticEarthDatabase::initLatLonRefValues(double ref_lat, double ref_lon, double ref_mpdla)
{
   double coslatxe;

   clla.originLat     = ref_lat;
   clla.originLon     = ref_lon;
   clla.feetPerDegLat = UtMath::cFT_PER_M * ref_mpdla;
   coslatxe           = cos(UtMath::cRAD_PER_DEG * clla.originLat);
   clla.feetPerDegLon = clla.feetPerDegLat * coslatxe;
}

// This function will return necessary reference variables for
// use in the compute functions
void GeodeticEarthDatabase::get_database_values(int database, double* originLat, double* originLon, double* feetPerDegLat)
{
   *originLat     = m_dbLat;
   *originLon     = m_dbLon;
   *feetPerDegLat = m_ftpdla;
}

// This function will return the total latitude and longitude values in decimal degrees
// given the north/east in feet based on the database being used.

void GeodeticEarthDatabase::GenTotalLatLon(double posX, double posY, int database, double* degLat, double* degLon)
{
   static int mapType = -1;

   static double coslatxe;
   static double originLat, originLon;
   static double feetPerDegLat, feetPerDegLon;

   double degLatTotal, degLonTotal;

   if (database <= 0)
   {
      // no database specified, use init values
      originLat     = clla.originLat;
      originLon     = clla.originLon;
      feetPerDegLat = clla.feetPerDegLat;
      feetPerDegLon = clla.feetPerDegLon;
   }
   else
   {
      // determine which database is being used (similar code is
      // found in the otwinfo.c file in the interf/otw directory so
      // if we add a database here we need to add it to that file also)
      if (mapType != database)
      {
         get_database_values(database, &originLat, &originLon, &feetPerDegLat);

         coslatxe      = cos(UtMath::cRAD_PER_DEG * originLat);
         feetPerDegLon = feetPerDegLat * coslatxe;

         mapType = database;
      }
   }

   degLatTotal = ((double)posX / feetPerDegLat) + originLat;
   *degLat     = degLatTotal;

   degLonTotal = ((double)posY / feetPerDegLon) + originLon;
   *degLon     = degLonTotal;
}

//  This function will return the latitude and longitude values in
//  degrees:min:seconds given the north/east in feet based on the
//  database being used.

void GeodeticEarthDatabase::GetLatLon(double posX,
                                      double posY,
                                      int    database,
                                      int*   degLat,
                                      int*   minLat,
                                      float* secLat,
                                      int*   degLon,
                                      int*   minLon,
                                      float* secLon)
{
   static int mapType = -1;

   static double coslatxe;
   static double originLat, originLon;
   static double feetPerDegLat, feetPerDegLon;

   double degLatTotal, degLonTotal;
   double minLatTotal, minLonTotal;
   double secLatTotal, secLonTotal;


   if (database <= 0)
   {
      // no database specified, use init values
      originLat     = clla.originLat;
      originLon     = clla.originLon;
      feetPerDegLat = clla.feetPerDegLat;
      feetPerDegLon = clla.feetPerDegLon;
   }
   else
   {
      // determine which database is being used (similar code is
      // found in the otwinfo.c file in the interf/otw directory so
      // if we add a database here we need to add it to that file also)
      if (mapType != database)
      {
         get_database_values(database, &originLat, &originLon, &feetPerDegLat);
         coslatxe      = cos(UtMath::cRAD_PER_DEG * originLat);
         feetPerDegLon = feetPerDegLat * coslatxe;

         mapType = database;
      }
   }

   degLatTotal = ((double)posX / feetPerDegLat) + originLat;
   *degLat     = (int)degLatTotal;
   minLatTotal = fabs((degLatTotal - *degLat) * 60.0);
   *minLat     = (int)minLatTotal;
   secLatTotal = (minLatTotal - *minLat) * 60.0;
   *secLat     = (float)secLatTotal;

   if (*secLat >= 60.0)
   {
      *secLat = *secLat - 60.0f;
      *minLat = *minLat + 1;
   }
   if (*minLat >= 60)
   {
      *minLat = *minLat - (int)60.0f;
      *degLat = *degLat + 1;
   }

   degLonTotal = ((double)posY / feetPerDegLon) + originLon;
   *degLon     = (int)degLonTotal;
   minLonTotal = fabs((degLonTotal - *degLon) * 60.0);
   *minLon     = (int)minLonTotal;
   secLonTotal = (minLonTotal - *minLon) * 60.0;
   *secLon     = (float)secLonTotal;

   if (*secLon >= 60.0)
   {
      *secLon = *secLon - 60.0f;
      *minLon = *minLon + 1;
   }
   if (*minLon >= 60.0f)
   {
      *minLon = *minLon - (int)60.0f;
      *degLon = *degLon + 1;
   }
}

//   This function will return the North, East values given the
//   latitude and longitude values in degrees based on the database being used.

void GeodeticEarthDatabase::GenFeet(double* posX, double* posY, int database, double totalLat, double totalLon)
{
   static int mapType = -1;

   static double coslatxe;
   static double originLat, originLon, originLatSec, originLonSec;
   static double feetPerDegLat, feetPerDegLon;
   static double feetPerMinLat, feetPerMinLon;
   static double feetPerSecLat, feetPerSecLon;
   static int    northValueOrg, eastValueOrg;

   double totalSecLat, totalSecLon;

   static int debugPrint = 0;

   // flags to indicate if incoming values are North and East
   int northValue = 0;
   int eastValue  = 0;

   if (database <= 0)
   {
      // no database specified, use init values
      originLat     = clla.originLat;
      originLon     = clla.originLon;
      feetPerDegLat = clla.feetPerDegLat;
      feetPerDegLon = clla.feetPerDegLon;
   }
   else
   {
      // determine which database is being used (similar code is
      // found in the otwinfo.c file in the interf/otw directory so
      // if we add a database here we need to add it to that file also)
      if (mapType != database)
      {
         get_database_values(database, &originLat, &originLon, &feetPerDegLat);

         coslatxe      = cos(UtMath::cRAD_PER_DEG * originLat);
         feetPerDegLon = feetPerDegLat * coslatxe;
         if (debugPrint)
         {
            auto out = ut::log::debug() << "GenFeet data about origin.";
            out.AddNote() << "Origin Latitude: " << std::setprecision(7) << originLat;
            out.AddNote() << "Origin Longitude: " << std::setprecision(7) << originLon;
            out.AddNote() << "Feet Per Deg Lat: " << feetPerDegLat;
            out.AddNote() << "Feet Per Deg Lon: " << feetPerDegLon;
         }
      }

      feetPerMinLat = feetPerDegLat / 60.0;
      feetPerMinLon = feetPerDegLon / 60.0;

      feetPerSecLat = feetPerMinLat / 60.0;
      feetPerSecLon = feetPerMinLon / 60.0;

      if (originLat >= 0.0)
         northValueOrg = 1;
      else
         northValueOrg = -1;

      if (originLon >= 0.0)
         eastValueOrg = 1;
      else
         eastValueOrg = -1;

      originLatSec = (fabs(originLat) * 3600.0) * northValueOrg;
      originLonSec = (fabs(originLon) * 3600.0) * eastValueOrg;

      mapType = database;
   }

   // set up multiplier value
   if (totalLat > 0)
      northValue = 1;
   else
      northValue = -1;

   if (totalLon > 0)
      eastValue = 1;
   else
      eastValue = -1;

   // total seconds
   totalSecLat = (fabs(totalLat) * 3600.0) * northValue;
   totalSecLon = (fabs(totalLon) * 3600.0) * eastValue;

   *posX = (totalSecLat - originLatSec) * feetPerSecLat;
   *posY = (totalSecLon - originLonSec) * feetPerSecLon;
}

void GeodeticEarthDatabase::SetRefLatLon(double aLatitude, double aLongitude)
{
   mRefLat = aLatitude;
   mRefLon = aLongitude;
}

void GeodeticEarthDatabase::SetDatabase(std::string aDBName)
{
   m_databaseName = aDBName;

   if (m_databaseName == "TERRAIN_MAP")
   {
      m_database = 1;
      m_dbLat    = 35.0;
      m_dbLon    = -118.0;
      m_ftpdla   = (double)(364566.78);
      m_ftpdlo   = 0.0;
   }
   else if (m_databaseName == "CS_EDWARDS")
   {
      m_database = 2;
      m_dbLat    = 35.0;
      m_dbLon    = -118.0;
      m_ftpdla   = (double)(364566.78);
      m_ftpdlo   = 0.0;
   }
   else if (m_databaseName == "EDWARDS")
   {
      m_database = 4;
      m_dbLat    = 35.0;
      m_dbLon    = -118.0;
      m_ftpdla   = (double)(111120.0 * UtMath::cFT_PER_M);
      m_ftpdlo   = (double)(91024.175 * UtMath::cFT_PER_M);
   }
   else if (m_databaseName == "WISCONSIN")
   {
      m_database = 5;
      m_dbLat    = 38.5;
      m_dbLon    = -118.0;
      m_ftpdla   = (double)(364566.78);
      m_ftpdlo   = 0.0;
   }
   else if (m_databaseName == "HUNTER_LIGGETT")
   {
      m_database = 6;
      m_dbLat    = 38.5;
      m_dbLon    = -118.0;
      m_ftpdla   = (double)(364566.78);
      m_ftpdlo   = 0.0;
   }
   else if (m_databaseName == "BOSNIA")
   {
      m_database = 7;
      m_dbLat    = 43.585;
      m_dbLon    = 18.668;
      m_ftpdla   = (double)(364566.78);
      m_ftpdlo   = 0.0;
   }
   else if (m_databaseName == "GCS")
   {
      m_database = 11;
      m_dbLat    = 46.0;
      m_dbLon    = 22.0;
      m_ftpdla   = (double)(364566.78);
      m_ftpdlo   = 0.0;
   }
   else if (m_databaseName == "OLD_KOREA")
   {
      m_database = 11;
      m_dbLat    = 38.0;
      m_dbLon    = 127.0;
      m_ftpdla   = (double)(364566.78);
      m_ftpdlo   = 0.0;
   }
   else if (m_databaseName == "UTAH")
   {
      m_database = 12; // unknown number
      m_dbLat    = 40.282;
      m_dbLon    = -113.797;
      m_ftpdla   = (double)(364566.78);
      m_ftpdlo   = 0.0;
   }
   else if (m_databaseName == "EDWARDS3")
   {
      m_database = 14;
      m_dbLat    = 35.0;
      m_dbLon    = -118.0;
      m_ftpdla   = (double)(364566.932);
      m_ftpdlo   = (double)(298635.748);
   }
   else if (m_databaseName == "IEIST")
   {
      m_database = 15;
      m_dbLat    = 43.5;
      m_dbLon    = 19.5;
      m_ftpdla   = (double)(364566.78);
      m_ftpdlo   = 0.0;
   }
   else if (m_databaseName == "STL")
   {
      m_database = 16;
      m_dbLat    = 39.0;
      m_dbLon    = -91.0;
      m_ftpdla   = (double)(364566.9291);
      m_ftpdlo   = (double)(283321.7159);
   }
   else if (m_databaseName == "MIDEAST")
   {
      m_database = 17;
      m_dbLat    = 27.53528;
      m_dbLon    = 46.2445;
      m_ftpdla   = (double)(111120.0 * UtMath::cFT_PER_M);
      m_ftpdlo   = 0.0;
   }
   else if (m_databaseName == "NEVADA")
   {
      m_database = 18;
      m_dbLat    = 38.0;
      m_dbLon    = -117.0;
      m_ftpdla   = (double)(111120.0 * UtMath::cFT_PER_M);
      m_ftpdlo   = (double)(87563.754941 * UtMath::cFT_PER_M);
   }
   else if (m_databaseName == "KOREA")
   {
      m_database = 19;
      m_dbLat    = 38.0;
      m_dbLon    = 127.0;
      m_ftpdla   = (double)(111120.0 * UtMath::cFT_PER_M);
      m_ftpdlo   = (double)(87563.754941 * UtMath::cFT_PER_M);
   }
   else if (m_databaseName == "ELMENDORF")
   {
      m_database = 20;
      m_dbLat    = 61.200489;
      m_dbLon    = -149.902835;
      m_ftpdla   = (double)(111120.0 * UtMath::cFT_PER_M);
      m_ftpdlo   = 0.0;
   }
   else if (m_databaseName == "HORMUZ")
   {
      m_database = 21;
      m_dbLat    = 25.0;
      m_dbLon    = 55.0;
      m_ftpdla   = (double)(111120.0 * UtMath::cFT_PER_M);
      m_ftpdlo   = 0.0;
   }
   else if (m_databaseName == "SSWA")
   {
      m_database = 22;
      m_dbLat    = 27.0;
      m_dbLon    = 51.0;
      m_ftpdla   = (double)(111120.0 * UtMath::cFT_PER_M);
      m_ftpdlo   = 0.0;
   }
   else if (m_databaseName == "CASPIAN")
   {
      m_database = 23;
      m_dbLat    = 32.93523;
      m_dbLon    = 51.78411;
      m_ftpdla   = (double)(111120.0 * UtMath::cFT_PER_M);
      m_ftpdlo   = 0.0;
   }
   else if (m_databaseName == "BAGHDAD")
   {
      m_database = 24;
      m_dbLat    = 33.0;
      m_dbLon    = 44.0;
      m_ftpdla   = (double)(364566.932);
      m_ftpdlo   = (double)(298635.748);
   }
   else if (m_databaseName == "AFGHANISTAN")
   {
      m_database = 25;
      m_dbLat    = 34.0;
      m_dbLon    = 70.0;
      m_ftpdla   = (double)(364566.932);
      m_ftpdlo   = (double)(298635.748);
   }
   else if (m_databaseName == "IN_METERS")
   {
      m_database = 26;
      m_dbLat    = mRefLat;
      m_dbLon    = mRefLon;
      m_ftpdla   = (double)(364566.9291);
      m_ftpdlo   = (double)cos(mRefLat * UtMath::cRAD_PER_DEG) * m_ftpdla;
   }
   else if (m_databaseName == "IN_FEET")
   {
      m_database = 27;
      m_dbLat    = mRefLat;
      m_dbLon    = mRefLon;
      m_ftpdla   = (double)(364566.9291);
      m_ftpdlo   = (double)cos(mRefLat * UtMath::cRAD_PER_DEG) * m_ftpdla;
   }
}

void GeodeticEarthDatabase::SetGeodeticEarthFilePathname(std::string& aPathname)
{
   m_pathNameOrFile = aPathname;
}

bool GeodeticEarthDatabase::OpenTheDatabase()
{
   // Determine the terrain database
   bool ok = true;
   // This is the file that would point to the default flat earth Edwards database list
   std::string terrainListPath = "./db_eafb4.lst";

   if ((m_database == EDWARDS_DATABASE) || (m_database == BAGHDAD_DATABASE) || (m_database == AFGHANISTAN_DATABASE) ||
       (m_database == METERS_DATABASE) || (m_database == FEET_DATABASE))
   {
      // GeodeticEarthElevCompute::SetInMeters(1);
      //  enable ground check
      if (m_pathNameOrFile != "")
      {
         terrainListPath = m_pathNameOrFile;
      }

      auto out = ut::log::info() << "Attempting to open terrain.";
      out.AddNote() << "Database File: " << terrainListPath;
      out.AddNote() << "Initializing terrain database " << m_database << ".";

      float elx = 0.0;
      float ely = 0.0;
      float elz = -1.0;
      float elnorm[3];
      FILE* fp;
      /* try opening list file first */
      fp = fopen(terrainListPath.c_str(), "r");
      if (fp)
      {
         fclose(fp);
         out.AddNote() << "List File: " << terrainListPath;
         out.Send();

         char new_path[256];
         strcpy(new_path, terrainListPath.c_str());

         elz = GeodeticEarthElevCompute::elcompute(1, new_path, elx, ely, elnorm);
      }
      else
      {
         auto note = out.AddNote("error") << "Could not open list file.";
         note.AddNote() << "File: " << terrainListPath;
         SetEncounteredError(true);
         ok = false;
      }
      if (elz < 0.0)
      {
         out.AddNote() << "Database unavailable at this location. Check ground disabled.";
         GeodeticEarthElevCompute::SetValidTerrainData(0);
      }
      else
      {
         GeodeticEarthElevCompute::SetValidTerrainData(1);
      }
   }
   return ok;
}

bool GeodeticEarthDatabase::ValidateDBName(std::string aGeodeticEarthDBName)
{
   bool                                  foundDBName = false;
   std::map<std::string, bool>::iterator iter        = m_databaseNames.find(aGeodeticEarthDBName);
   if (iter != m_databaseNames.end())
      foundDBName = true;
   return foundDBName;
}

bool GeodeticEarthDatabase::InMeters()
{
   bool        inMeters = false;
   std::string dbName   = GeodeticEarthDatabase::m_databaseName;

   // flat earth databases, some return feet, some meters find out which by database name
   std::map<std::string, bool>::iterator iter = m_databaseNames.find(dbName);
   if (iter != m_databaseNames.end())
   {
      if ((*iter).second)
         inMeters = true;
      else
         inMeters = false;
   }
   return inMeters;
}
