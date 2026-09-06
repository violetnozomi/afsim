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

#include "VisualDatabase.hpp"

//#define DEBUG

#include <cstring>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "UtVec3.hpp"

/**
 *  FUNCTION: Initialize
 *
 *  PURPOSE:
 *   This routine reads an elevation data file into an allocated data structure.
 *
 *  PARAMETERS:
 *
 *    aDBName : a Database file name
 *    return  : -1 if load failed
 *               0 if success
 *
 */
int VisualDatabase::Initialize(const char* aDBName)
{
   std::vector<std::string> namelist; // Array of .eld file names

   std::string dbNamePath;
   std::string tempPath(aDBName);
   size_t      pos = tempPath.find_last_of("/\\");
   if (pos != std::string::npos)
   {
      dbNamePath = tempPath.substr(0, pos + 1);
   }

   // Open with error checking
   std::ifstream fp(aDBName);
   if (!fp)
   {
      auto out = ut::log::error() << "Could not open file.";
      out.AddNote() << "File: " << aDBName;
      return -1;
   }
   else // Read lines into name list
   {
      std::string temp;
      while (fp >> temp)
      {
         if (!temp.empty())
         {
            namelist.push_back(dbNamePath + temp);
         }
      }
   }

   if (namelist.empty())
   {
      auto out = ut::log::error() << "No databases were found in file.";
      out.AddNote() << "File: " << aDBName;
      return -1;
   }

   // Save db count in struct
   mDBset.numdb = static_cast<int>(namelist.size());

   // Allocate the Database_set arrays
   mDBset.dblist.resize(namelist.size());
   mDBset.xmin.resize(namelist.size());
   mDBset.ymin.resize(namelist.size());
   mDBset.xmax.resize(namelist.size());
   mDBset.ymax.resize(namelist.size());

   // Loop through the input databases
   for (size_t i = 0; i < namelist.size(); ++i)
   {
      // Initialize this database
      auto curdb = ut::make_unique<Database>();

      // Set extents to bogus if file not found
      if (InitializeDB(namelist[i].c_str(), curdb.get()) < 0)
      {
         return -1;
      }
      else if (curdb->geoflag != 1)
      {
         auto out = ut::log::error() << "Database is not geocentric.";
         out.AddNote() << "File: " << namelist[i];
         return -1;
      }
      else // Good .eld init
      {
         // Compute database extents
         mDBset.xmin[i] = curdb->swx;
         mDBset.ymin[i] = curdb->swy;
         mDBset.xmax[i] = curdb->swx + (static_cast<float>(curdb->nfilex) * curdb->filesizex);
         mDBset.ymax[i] = curdb->swy + (static_cast<float>(curdb->nfile / curdb->nfilex) * curdb->filesizey);

         // Save database pointer
         mDBset.dblist[i] = curdb.release();
      }
   }

   return 0;
}

/**
 *  FUNCTION: ComputeElev
 *
 *  PURPOSE:
 *   This routine looks up the elevation corresponding to the
 *   input x,y coordinates. The x,y values are in database
 *   coordinates (usually feet) relative to the database origin.
 *
 *  PARAMETERS:
 *
 *    aLat, aLon : Coordinates of point to be looked up
 *    aElev      : Database elevation at x, y
 *    aNorm      : Pointer to normalized normal triple
 *    return     : 0 if requested location is in last face
 *                 1 if a face switch was required
 */
int VisualDatabase::ComputeElev(double aLat, double aLon, float& aElev, double aNorm[3])
{
   int status = 0;
   aElev      = -9999.0; // signal that lookup point is outside boundaries of the database

   // Loop through databases
   for (int i = 0; i < mDBset.numdb; ++i)
   {
      // Test input x,y against database extents
      if ((aLon >= mDBset.xmin[i]) && (aLon <= mDBset.xmax[i]) && (aLat >= mDBset.ymin[i]) && (aLat <= mDBset.ymax[i]))
      {
         status = ComputeElevDB(aLat, aLon, aElev, aNorm, mDBset.dblist[i]);
         break;
      }
   }

   return status;
}

VisualDatabase::~VisualDatabase()
{
   // Loop through databases
   for (int i = 0; i < mDBset.numdb; ++i)
   {
      delete mDBset.dblist[i];
   }
}

int VisualDatabase::InitializeDB(const char* aDBName, Database* aDB)
{
#ifdef DEBUG
   int badnorm;
#endif

   // Open the database file
   std::ifstream fp(aDBName, std::ios::binary);
   if (!fp)
   {
      auto out = ut::log::error() << "Could not open elevation database.";
      out.AddNote() << "File: " << aDBName;
      return -1;
   }

   // Database opened OK
   // Read the database into the data structure, allocating as you go
   if (!aDB)
   {
      ut::log::error() << "Null database in InitializeDB.";
      return -1;
   }

   // Read in Database Structure Elements
   fp.read(reinterpret_cast<char*>(&aDB->nfile), sizeof(aDB->nfile));
   fp.read(reinterpret_cast<char*>(&aDB->subregsizex), sizeof(aDB->subregsizex));
   fp.read(reinterpret_cast<char*>(&aDB->subregsizey), sizeof(aDB->subregsizey));
   fp.read(reinterpret_cast<char*>(&aDB->subregsizey), sizeof(aDB->subregsizey));
   fp.read(reinterpret_cast<char*>(&aDB->nsubregx), sizeof(aDB->nsubregx));
   fp.read(reinterpret_cast<char*>(&aDB->nsubregy), sizeof(aDB->nsubregy));
   fp.read(reinterpret_cast<char*>(&aDB->filesizex), sizeof(aDB->filesizex));
   fp.read(reinterpret_cast<char*>(&aDB->filesizey), sizeof(aDB->filesizey));
   fp.read(reinterpret_cast<char*>(&aDB->nfilex), sizeof(aDB->nfilex));
   fp.read(reinterpret_cast<char*>(&aDB->geoflag), sizeof(aDB->geoflag));
   fp.read(reinterpret_cast<char*>(&aDB->swx), sizeof(aDB->swx));
   fp.read(reinterpret_cast<char*>(&aDB->swy), sizeof(aDB->swy));

   aDB->files.resize(aDB->nfile);

   for (int i = 0; i < aDB->nfile; ++i)
   {
      // Read in File Structure Elements */
      fp.read(reinterpret_cast<char*>(&aDB->files[i].nsubregion), sizeof(aDB->files[i].nsubregion));

      aDB->files[i].subregions.resize(aDB->files[i].nsubregion);

      for (int j = 0; j < aDB->files[i].nsubregion; ++j)
      {
         // Read in Subregion Structure Elements
         fp.read(reinterpret_cast<char*>(&aDB->files[i].subregions[j].nface), sizeof(aDB->files[i].subregions[j].nface));

         aDB->files[i].subregions[j].faces.resize(aDB->files[i].subregions[j].nface);

         for (int k = 0; k < aDB->files[i].subregions[j].nface; ++k)
         {
            // Read in Face Structure Elements
            fp.read(reinterpret_cast<char*>(&aDB->files[i].subregions[j].faces[k].nvert),
                    sizeof(aDB->files[i].subregions[j].faces[k].nvert));
            fp.read(reinterpret_cast<char*>(&aDB->files[i].subregions[j].faces[k].normal[0]),
                    sizeof(aDB->files[i].subregions[j].faces[k].normal[0]));
            fp.read(reinterpret_cast<char*>(&aDB->files[i].subregions[j].faces[k].normal[1]),
                    sizeof(aDB->files[i].subregions[j].faces[k].normal[1]));
            fp.read(reinterpret_cast<char*>(&aDB->files[i].subregions[j].faces[k].normal[2]),
                    sizeof(aDB->files[i].subregions[j].faces[k].normal[2]));
            fp.read(reinterpret_cast<char*>(&aDB->files[i].subregions[j].faces[k].d),
                    sizeof(aDB->files[i].subregions[j].faces[k].d));
#ifdef DEBUG
            badnorm = 0;
            if ((aDB->files[i].subregions[j].faces[k].normal[0] == 0.0) &&
                (aDB->files[i].subregions[j].faces[k].normal[1] == 0.0) &&
                (aDB->files[i].subregions[j].faces[k].normal[2] == 0.0))
            {
               badnorm = 1;
            }
#endif

            aDB->files[i].subregions[j].faces[k].vert.resize(aDB->files[i].subregions[j].faces[k].nvert);

            for (int l = 0; l < aDB->files[i].subregions[j].faces[k].nvert; ++l)
            {
               // Read in Vert Structure Elements
               fp.read(reinterpret_cast<char*>(&aDB->files[i].subregions[j].faces[k].vert[l].v[0]),
                       sizeof(aDB->files[i].subregions[j].faces[k].vert[l].v[0]));
               fp.read(reinterpret_cast<char*>(&aDB->files[i].subregions[j].faces[k].vert[l].v[1]),
                       sizeof(aDB->files[i].subregions[j].faces[k].vert[l].v[1]));
               fp.read(reinterpret_cast<char*>(&aDB->files[i].subregions[j].faces[k].vert[l].v[2]),
                       sizeof(aDB->files[i].subregions[j].faces[k].vert[l].v[2]));
#ifdef DEBUG
               if (badnorm)
               {
                  ut::log::debug() << "Vertex:" << std::setw(12) << std::setprecision(3)
                                   << aDB->files[i].subregions[j].faces[k].vert[l].v[0] << ", "
                                   << aDB->files[i].subregions[j].faces[k].vert[l].v[1] << ", "
                                   << aDB->files[i].subregions[j].faces[k].vert[l].v[2];
               }
#endif
            } // Vertex loop
         }    // Face loop
      }       // Subregion loop
   }          // File loop

   return 0;
}

int VisualDatabase::ComputeElevDB(double aLat, double aLon, float& aElev, double aNorm[3], Database* aDB)
{
   int           status = 0;
   static double lastNorm[3];

   // Compute x,y wrt SW corner of data
   double gx = aLon - aDB->swx;
   double gy = aLat - aDB->swy;

   // Compute file index
   int ix     = (int)(gx / aDB->filesizex);
   int iy     = (int)(gy / aDB->filesizey);
   int findex = ix + (aDB->nfilex * iy);

   std::lock_guard<std::recursive_mutex> lock(mCacheLock);

   // Be sure point falls within the database area
   int nfiley = aDB->nfile / aDB->nfilex;
   if ((gx < 0.0) || (gx >= (aDB->nfilex * aDB->filesizex)))
   {
      aElev = -9999.0;
   }
   else if ((gy < 0.0) || (gy >= (nfiley * aDB->filesizey)))
   {
      aElev = -9999.0;
   }
   else if ((mLastFace == nullptr) ||
            (!PointInFace(aLat, aLon, *mLastFace))) // Point is in database - check that it is not in previous face
   {
      // Compute subregion index
      ix         = (int)((gx - ((float)ix * aDB->filesizex)) / aDB->subregsizex);
      iy         = (int)((gy - ((float)iy * aDB->filesizey)) / aDB->subregsizey);
      int sindex = ix + (aDB->nsubregx * iy);

      // Loop through faces in this subregion
      for (int i = 0; i < aDB->files[findex].subregions[sindex].nface; ++i)
      {
         // Capture the face x and y extremes
         float xmin, ymin, xmax, ymax;
         xmin = ymin = 1.e10;
         xmax = ymax = -1.e10;
         // Loop through each vertex
         for (int j = 0; j < aDB->files[findex].subregions[sindex].faces[i].nvert; ++j)
         {
            float vx = aDB->files[findex].subregions[sindex].faces[i].vert[j].v[0];
            float vy = aDB->files[findex].subregions[sindex].faces[i].vert[j].v[1];
            xmin     = (vx < xmin) ? vx : xmin;
            xmax     = (vx > xmax) ? vx : xmax;
            ymin     = (vy < ymin) ? vy : ymin;
            ymax     = (vy > ymax) ? vy : ymax;
         }

         // Check if the query point is in this region
         if ((aLon >= xmin) && (aLon <= xmax) && (aLat >= ymin) && (aLat <= ymax))
         {
            // Check if the query point is within the face boundary
            if (PointInFace(aLat, aLon, aDB->files[findex].subregions[sindex].faces[i]))
            {
               // Get the parameter for this face
               float a = aDB->files[findex].subregions[sindex].faces[i].normal[0];
               float b = aDB->files[findex].subregions[sindex].faces[i].normal[1];
               float c = aDB->files[findex].subregions[sindex].faces[i].normal[2];
               float d = aDB->files[findex].subregions[sindex].faces[i].d;

               // Compute the elevation
               aElev = (float)((d - (a * aLon) - (b * aLat)) / c);

               double swElev = (d - (a * xmin) - (b * ymin)) / c;
               double seElev = (d - (a * xmax) - (b * ymin)) / c;
               double nwElev = (d - (a * xmin) - (b * ymax)) / c;
               double neElev = (d - (a * xmax) - (b * ymax)) / c;

               double latSize = (ymax - ymin) * 60.0 * 1852.0;
               double lonSize = cos(ymin * UtMath::cRAD_PER_DEG) * (xmax - xmin) * 60.0 * 1852.0;

               double v1[3] = {latSize, lonSize, neElev - swElev};
               double v2[3] = {-latSize, lonSize, nwElev - seElev};
               UtVec3d::CrossProduct(aNorm, v1, v2);
               UtVec3d::Normalize(aNorm);
               UtVec3d::Set(lastNorm, aNorm);

               // Save the normals for output
               /*
               aNorm[0] = a;
               aNorm[1] = b;
               aNorm[2] = c;
               */

               // Save this face pointer
               mLastFace = &aDB->files[findex].subregions[sindex].faces[i];
               status    = 1;
               break;
            }
         }
      } // Face loop
   }
   else // Point is in previous face
   {
      // Get the parameter for that face
      float a = mLastFace->normal[0];
      float b = mLastFace->normal[1];
      float c = mLastFace->normal[2];
      float d = mLastFace->d;

      // Compute the elevation
      aElev = (float)((d - (a * aLon) - (b * aLat)) / c);

      // Save the normals for output
      /*
      aNorm[0] = a;
      aNorm[1] = b;
      aNorm[2] = c;
      */
      UtVec3d::Set(aNorm, lastNorm);
   }
   return status;
}

/**
 *  FUNCTION: PointInFace
 *
 *  PURPOSE:
 *   This routine tests the input point to see if it is in the input face.
 *   This is a 2D test.  The function returns true if the point is inside,
 *   false otherwise.
 *
 *  PARAMETERS:
 *
 *     aLat, aLon : Coordinates of point to be looked up
 *     aFace      : Face data structure
 *     return     : false if point not in, true if it is
 *
 */
bool VisualDatabase::PointInFace(double aLat, double aLon, const Face& aFace)
{
   const float TOL = .000001f; // Position tolerance

   float vert[3];  // General purpose vertex
   float vertp[3]; // Next vertex

   // Initialize inside flag on
   bool inside = true;

   // Initialize retry flag on
   bool retry = true;

   // Check if point is on a vertex
   if (inside)
   {
      for (int i = 0; i < aFace.nvert; ++i)
      {
         vert[0] = aFace.vert[i].v[0];
         vert[1] = aFace.vert[i].v[1];
         if ((fabs(aLon - vert[0]) < TOL) && (fabs(aLat - vert[1]) < TOL))
         {
            retry = false;
            break;
         }
      }
   }

   // Not a vertex
   if (retry)
   {
      for (int i = 0; i < aFace.nvert; ++i)
      {
         int ip   = (i < (aFace.nvert - 1)) ? i + 1 : 0;
         vert[0]  = aFace.vert[i].v[0];
         vert[1]  = aFace.vert[i].v[1];
         vertp[0] = aFace.vert[ip].v[0];
         vertp[1] = aFace.vert[ip].v[1];

         // Compute vectors from start vertex to point and along edge
         double px = aLon - vert[0];
         double py = aLat - vert[1];
         double sx = vertp[0] - vert[0];
         double sy = vertp[1] - vert[1];

         // Compute side crossed with point vector
         double cross = sx * py - px * sy;

         // Compute side dotted with point vector
         double dot = sx * px + sy * py;

         // If cross is negative, point is outside
         if (cross < 0.0)
         {
            inside = false;
            break;
         }

         // If cross is zero, point may be on edge
         else if (fabs(cross) < TOL)
         {
            // Compute vector lengths (squared)
            double plen = px * px + py * py;
            double slen = sx * sx + sy * sy;

            // Point is on the edge if point vector is shorter and dot product is positive
            if ((plen < slen) && (dot > 0.0))
            {
               inside = true;
               break;
            }
         }
      }
   }

   return inside;
}
