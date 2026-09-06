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
//
//*f FUNCTION: elcompute
//
// ---------------------------------------------------------------------------
// *m FILENAME: elcompute.c
//
// ---------------------------------------------------------------------------
// *p PURPOSE:
// *p  This routine reads an elevation data file into an allocated data structure
// *p  in the initial loop, then looks up the elevation corresponding to the
// *p  input x,y coordinates in the execute loop.  The x,y values are in database
// *p  coordinates (usually feet) relative to the database origin.  The cleanup
// *p  loop is provided to allow memory to be freed.
// *p
// ---------------------------------------------------------------------------
// *c CALL METHOD:
// *c   float elcompute(int initial, char* dbname, float x, float y, float* norm)
//
// *c     initial  - Control flag (1=read data, 0=execute, -1=cleanup)
// *c     dbname   - Database file name
// *c     x, y     - Coordinates of point to be looked up
// *c     norm     - Pointer to normalized normal triple
// *c     return   - Database elevation at x, y
// *c
//
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

#define DEBUG

//
#include "GeodeticEarthElevCompute.hpp"

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>

#include <malloc.h>

#include "GeodeticEarthDatabase.hpp"
#include "UtLog.hpp"
#include "UtSleep.hpp"

#define MAXDB 128

int              GeodeticEarthElevCompute::m_validTerrainData = 0;
int              GeodeticEarthElevCompute::iFaceIndex         = 0;
int              GeodeticEarthElevCompute::jFaceIndex         = 0;
std::vector<int> GeodeticEarthElevCompute::m_BadSubregions;
// int GeodeticEarthElevCompute::m_inMeters         = 0;

float GeodeticEarthElevCompute::elcompute(int    initial,
                                          char*  dbname,
                                          float  x, // aLon
                                          float  y, // aLat
                                          float* norm)
{
   int                      numdb = 0;          // No. databases
   float                    z     = -1.111111f; // Returned elevation
   std::vector<std::string> namelist;           // Array of .eld file names
   static Database_set      dbset;              // Database struct

   // Initial pass
   if (initial == 1)
   {
      // Check if input file is a .eld
      if (strstr(dbname, ".eld"))
      {
         // Allocate one-member name list and put name in name list
         numdb = 1;
         namelist.emplace_back(dbname);
      }
      // Input file is a list
      else
      {
         // Open with error checking
         std::ifstream fp(dbname);
         // FILE* fp = fopen(dbname, "r");
         if (!fp)
         {
            auto out = ut::log::error() << "Could not open database file.";
            out.AddNote() << "File: " << dbname;
            z = -1.0f;
            GeodeticEarthDatabase::SetEncounteredError(true);
            return z;
         }
         // Read lines into name list
         else
         {
            numdb = 0;
            std::string temp;
            while (std::getline(fp, temp))
            {
               numdb++;
               namelist.emplace_back(temp);
            }
         } // List opened
      }    // List input

      // Save db count in struct
      dbset.numdb = numdb;

      // Allocate the Database_set arrays
      dbset.dblist = static_cast<Database**>(malloc(numdb * sizeof(Database)));
      dbset.xmin   = static_cast<float*>(malloc(numdb * sizeof(float)));
      dbset.ymin   = static_cast<float*>(malloc(numdb * sizeof(float)));
      dbset.xmax   = static_cast<float*>(malloc(numdb * sizeof(float)));
      dbset.ymax   = static_cast<float*>(malloc(numdb * sizeof(float)));

      // Loop through the input databases
      for (int i = 0; i < numdb; ++i)
      {
         // Initialize this database
         auto curdb = static_cast<Database*>(malloc(sizeof(Database)));
         if (curdb == nullptr)
         {
            ut::log::error() << "Unable to allocate " << sizeof(Database) << " bytes for database.";
            z = -1.0f;
            GeodeticEarthDatabase::SetEncounteredError(true);
            return z;
         }
         else
         {
            z = elcompute_db(1, namelist[i].c_str(), 0, 0, nullptr, curdb);
         }
         // Set extents to bogus if file not found
         // GetDBType = 1 flat earth
         // GetDBType = 2 round earth
         if ((GeodeticEarthDatabase::GetDBType() == 1) && (z < 0.0f))
         {
            dbset.dblist[i] = nullptr;
            dbset.xmin[i] = dbset.ymin[i] = 1.0f;
            dbset.xmax[i] = dbset.ymax[i] = -1.0f;
         }
         // Good .eld init
         else
         {
            // Save database pointer
            dbset.dblist[i] = curdb;
            if (curdb != nullptr)
            {
               // Compute database extents
               dbset.xmin[i] = curdb->swx;
               dbset.ymin[i] = curdb->swy;
               dbset.xmax[i] = curdb->swx + (static_cast<float>(curdb->nfilex) * curdb->filesizex);
               dbset.ymax[i] = curdb->swy + (static_cast<float>(curdb->nfile / curdb->nfilex) * curdb->filesizey);
            }
         }
         // Signal success
         z = 0;
      } // Database loop
   }    // Initial pass

   // Execute pass
   else if (initial == 0)
   {
      // Loop through databases
      for (int i = 0; i < dbset.numdb; ++i)
      {
         // Test input lat, lon against database extents
         if ((x >= dbset.xmin[i]) && (x <= dbset.xmax[i]) && (y >= dbset.ymin[i]) && (y <= dbset.ymax[i]))
         {
            auto curdb = dbset.dblist[i];
            z          = elcompute_db(0, nullptr, x, y, norm, curdb);
            break;
         }
      } // DB loop
   }    // Execute pass

   // Return elevation
   return z;
}

// ---------------------------- Single DB elcompute -------------------------

float GeodeticEarthElevCompute::elcompute_db(int         initial,
                                             const char* dbname,
                                             float       x, // aLon
                                             float       y, // aLat
                                             float*      norm,
                                             Database*&  db)

{
   static Face* lastface; // Pointer to last face in which point
                          // found
   float z = -1.111111f;  // Returned elevation

   // --------------------------- Initial Path --------------------------------
   if (initial == 1)
   {
      // Open the database file
      std::ifstream fp(dbname, std::ios::in | std::ios::binary);
      if (!fp)
      {
         // sprintf(mesg, "\n ** ERROR ** Could not open elevation database %s",
         //         dbname);
         auto out = ut::log::error() << "Unable to open elevation database.";
         out.AddNote() << "File: " << dbname;
         GeodeticEarthDatabase::SetEncounteredError(true);
         z = -1.0f;
      }
      // Database opened OK
      else
      {
         // Read the database into the data structure, allocating as you go
         if (db == nullptr)
         {
            ut::log::error() << "Allocation failed in GeodeticEarthElevCompute::elcompute.";
            GeodeticEarthDatabase::SetEncounteredError(true);
            return -1.0f;
         }

         // Read in Database Structure Elements
         fp.read(reinterpret_cast<char*>(&db->nfile), sizeof(db->nfile));
         fp.read(reinterpret_cast<char*>(&db->subregsizex), sizeof(db->subregsizex));
         fp.read(reinterpret_cast<char*>(&db->subregsizey), sizeof(db->subregsizey));
         fp.read(reinterpret_cast<char*>(&db->nsubregx), sizeof(db->nsubregx));
         fp.read(reinterpret_cast<char*>(&db->nsubregy), sizeof(db->nsubregy));
         fp.read(reinterpret_cast<char*>(&db->filesizex), sizeof(db->filesizex));
         fp.read(reinterpret_cast<char*>(&db->filesizey), sizeof(db->filesizey));
         fp.read(reinterpret_cast<char*>(&db->nfilex), sizeof(db->nfilex));
         fp.read(reinterpret_cast<char*>(&db->geoflag), sizeof(db->geoflag));
         fp.read(reinterpret_cast<char*>(&db->swx), sizeof(db->swx));
         fp.read(reinterpret_cast<char*>(&db->swy), sizeof(db->swy));

         db->files = static_cast<File*>(malloc(sizeof(File) * db->nfile));
         if (db->files == nullptr)
         {
            ut::log::error() << "Allocation failed in GeodeticEarthElevCompute::elcompute for files.";
            GeodeticEarthDatabase::SetEncounteredError(true);
            fp.close();
            return (-1.0f);
         }
         bool bVertFail  = false;
         bool bFacesFail = false;
         iFaceIndex      = db->nfile;
         for (int i = 0; i < db->nfile; ++i)
         {
            bVertFail  = false;
            bFacesFail = false;
            // Read in File Structure Elements
            fp.read(reinterpret_cast<char*>(&db->files[i].nsubregion), sizeof(db->files[i].nsubregion));

            db->files[i].subregions = static_cast<Subregion*>(malloc(sizeof(Subregion) * db->files[i].nsubregion));

            if (db->files[i].subregions == nullptr)
            {
               m_BadSubregions.push_back(i);
               ut::log::error() << "Allocation failed in GeodeticEarthElevCompute::elcompute for subregions.";
               break;
               // GeodeticEarthDatabase::SetEncounteredError(true);
               // fclose(fp);
               // return(-1.0f);
            }

            jFaceIndex = db->files[i].nsubregion;

            for (int j = 0; j < db->files[i].nsubregion; ++j)
            {
               // Read in Subregion Structure Elements */
               fp.read(reinterpret_cast<char*>(&db->files[i].subregions[j].nface),
                       sizeof(db->files[i].subregions[j].nface));

               db->files[i].subregions[j].faces =
                  static_cast<Face*>(malloc(sizeof(Face) * db->files[i].subregions[j].nface));
               if (db->files[i].subregions[j].faces == nullptr)
               {
                  iFaceIndex = i;
                  jFaceIndex = j;

                  auto out = ut::log::warning()
                             << "Allocation failed in GeodeticEarthElevCompute::elcompute for faces.";
                  out.AddNote() << "Database had errors loading.";
                  out.AddNote() << "Some locations will have invalid elevation data.";

                  bFacesFail = true;

                  // UtSleep::Sleep(3.0);
                  // GeodeticEarthDatabase::SetEncounteredError(true);
                  // fclose(fp);

                  // return(-1.0f);
               }
               if (bFacesFail)
               {
                  break;
               }

               for (int k = 0; k < db->files[i].subregions[j].nface; ++k)
               {
                  // Read in Face Structure Elements
                  fp.read(reinterpret_cast<char*>(&db->files[i].subregions[j].faces[k].nvert),
                          sizeof(db->files[i].subregions[j].faces[k].nvert));
                  fp.read(reinterpret_cast<char*>(&db->files[i].subregions[j].faces[k].normal[0]),
                          sizeof(db->files[i].subregions[j].faces[k].normal[0]));
                  fp.read(reinterpret_cast<char*>(&db->files[i].subregions[j].faces[k].normal[1]),
                          sizeof(db->files[i].subregions[j].faces[k].normal[1]));
                  fp.read(reinterpret_cast<char*>(&db->files[i].subregions[j].faces[k].normal[2]),
                          sizeof(db->files[i].subregions[j].faces[k].normal[2]));
                  fp.read(reinterpret_cast<char*>(&db->files[i].subregions[j].faces[k].d),
                          sizeof(db->files[i].subregions[j].faces[k].d));
#ifdef DEBUG
                  int badnorm = 0;
                  if ((db->files[i].subregions[j].faces[k].normal[0] == 0.0) &&
                      (db->files[i].subregions[j].faces[k].normal[1] == 0.0) &&
                      (db->files[i].subregions[j].faces[k].normal[2] == 0.0))
                  {
                     badnorm = 1;
                  }
#endif
                  db->files[i].subregions[j].faces[k].vert =
                     static_cast<Vert*>(malloc(sizeof(Vert) * db->files[i].subregions[j].faces[k].nvert));

                  if (db->files[i].subregions[j].faces[k].vert == nullptr)
                  {
                     iFaceIndex = i;
                     jFaceIndex = j;

                     auto out = ut::log::warning()
                                << "Allocation failed in GeodeticEarthElevCompute::elcompute for vertices.";
                     out.AddNote() << "Database had errors loading.";
                     out.AddNote() << "Some locations will have invalid elevation data.";

                     bVertFail = true;
                     // UtSleep::Sleep(3.0);
                     // GeodeticEarthDatabase::SetEncounteredError(true);

                     // fclose(fp);
                     // return(-1.0f);
                  }
                  if (bVertFail)
                  {
                     break;
                  }
                  for (int l = 0; l < db->files[i].subregions[j].faces[k].nvert; ++l)
                  {
                     if (db->files[i].subregions[j].faces[k].vert)
                     {
                        // Read in Vert Structure Elements
                        fp.read(reinterpret_cast<char*>(&db->files[i].subregions[j].faces[k].vert[l].v[0]),
                                sizeof(db->files[i].subregions[j].faces[k].vert[l].v[0]));
                        fp.read(reinterpret_cast<char*>(&db->files[i].subregions[j].faces[k].vert[l].v[1]),
                                sizeof(db->files[i].subregions[j].faces[k].vert[l].v[1]));
                        fp.read(reinterpret_cast<char*>(&db->files[i].subregions[j].faces[k].vert[l].v[2]),
                                sizeof(db->files[i].subregions[j].faces[k].vert[l].v[2]));
#ifdef DEBUG
                        if (badnorm)
                        {
                           ut::log::debug()
                              << "Vertex:" << std::setprecision(3) << db->files[i].subregions[j].faces[k].vert[l].v[0]
                              << ", " << db->files[i].subregions[j].faces[k].vert[l].v[1] << ", "
                              << db->files[i].subregions[j].faces[k].vert[l].v[2];
                        }
#endif
                     }
                  } // Vertex loop
               }    // Face loop
            }       // Subregion loop
         }          // File loop

         // Close the file

         fp.close();
         auto out = ut::log::info() << "Finished reading elevation data from file.";
         out.AddNote() << "File: " << dbname;

         // Invalidate last face the first time
         lastface = nullptr;
         z        = 0;
      } // Database opened OK
   }    // Data initialization
   // -------------------------- Lookup path ----------------------------------
   else if (initial == 0)
   {
      // Compute x,y wrt SW corner of data
      float gx = x - db->swx;
      float gy = y - db->swy;

      // Compute file index
      int ix     = static_cast<int>(gx / db->filesizex);
      int iy     = static_cast<int>(gy / db->filesizey);
      int findex = ix + (db->nfilex * iy);

      // Be sure point falls with in the database area
      int                    nfiley      = db->nfile / db->nfilex;
      static constexpr float NO_GND_DATA = -99999.0;
      if ((gx < 0.0f) || (gx >= (db->nfilex * db->filesizex)))
      {
         //			z = -9999.0;
         z = NO_GND_DATA;
      }
      else if ((gy < 0.0f) || (gy >= (nfiley * db->filesizey)))
      {
         //			z = -9999.0;
         z = NO_GND_DATA;
      }

      // Point is in database - check that it is not in previous face
      else if ((lastface == nullptr) || (!in_test(x, y, *lastface)))
      {
         // Compute subregion index
         ix         = static_cast<int>((gx - (static_cast<float>(ix) * db->filesizex)) / db->subregsizex);
         iy         = static_cast<int>((gy - (static_cast<float>(iy) * db->filesizey)) / db->subregsizey);
         int sindex = ix + (db->nsubregx * iy);
         // if ((findex <= iFaceIndex) && (sindex <= jFaceIndex))
         //{
         //  Loop through faces in this subregion
         //  But first see if the subregion at file index (findex) was flagged as unreadable,
         //  if so, then skip this and send back last known data.
         bool bSubregionOk = true;
         for (int badSubregion : m_BadSubregions)
         {
            if (findex >= badSubregion)
            {
               bSubregionOk = false;
               break;
            }
         }
         if (db->files[findex].subregions == nullptr || !bSubregionOk)
         {
            if (lastface)
            {
               // Get the parameter for that face
               float a = lastface->normal[0];
               float b = lastface->normal[1];
               float c = lastface->normal[2];
               float d = lastface->d;

               // Compute the elevation
               z = (d - (a * x) - (b * y)) / c;

               // Save the normals for output
               norm[0] = a;
               norm[1] = b;
               norm[2] = c;
               return z;
            }
         }
         for (int i = 0; i < db->files[findex].subregions[sindex].nface; ++i)
         {
            // Capture the face x and y extremes
            float xmin = 1.e10;
            float ymin = 1.e10;
            float xmax = -1.e10;
            float ymax = -1.e10;
            if (db->files[findex].subregions[sindex].faces != nullptr)
            {
               for (int j = 0; j < db->files[findex].subregions[sindex].faces[i].nvert; ++j)
               {
                  if (db->files[findex].subregions[sindex].faces[i].vert)
                  {
                     float vx = db->files[findex].subregions[sindex].faces[i].vert[j].v[0];
                     float vy = db->files[findex].subregions[sindex].faces[i].vert[j].v[1];
                     xmin     = (vx < xmin) ? vx : xmin;
                     xmax     = (vx > xmax) ? vx : xmax;
                     ymin     = (vy < ymin) ? vy : ymin;
                     ymax     = (vy > ymax) ? vy : ymax;
                  }
                  else
                  {
                     if (lastface)
                     {
                        // Get the parameter for that face
                        float a = lastface->normal[0];
                        float b = lastface->normal[1];
                        float c = lastface->normal[2];
                        float d = lastface->d;

                        // Compute the elevation
                        z = (d - (a * x) - (b * y)) / c;

                        // Save the normals for output
                        norm[0] = a;
                        norm[1] = b;
                        norm[2] = c;
                        break;
                     }
                  }
               } // Vertex loop
            }
            // Check if the query point is in this region
            if ((x >= xmin) && (x <= xmax) && (y >= ymin) && (y <= ymax))
            {
               // Check if the query point is within the face boundary
               if (in_test(x, y, db->files[findex].subregions[sindex].faces[i]))
               {
                  // Get the parameter for this face
                  float a = db->files[findex].subregions[sindex].faces[i].normal[0];
                  float b = db->files[findex].subregions[sindex].faces[i].normal[1];
                  float c = db->files[findex].subregions[sindex].faces[i].normal[2];
                  float d = db->files[findex].subregions[sindex].faces[i].d;

                  // Compute the elevation
                  z = (d - (a * x) - (b * y)) / c;

                  // Save the normals for output
                  norm[0] = a;
                  norm[1] = b;
                  norm[2] = c;

                  // Save this face pointer
                  lastface = &db->files[findex].subregions[sindex].faces[i];

                  // Break out of loop
                  break;

               } // Point is within face

            } // Point is within extremes

         } // Face loop

      } // Point is in database, but not previous face

      // Point is in previous face
      else
      {
         // Get the parameter for that face
         float a = lastface->normal[0];
         float b = lastface->normal[1];
         float c = lastface->normal[2];
         float d = lastface->d;

         // Compute the elevation
         z = (d - (a * x) - (b * y)) / c;

         // Save the normals for output
         norm[0] = a;
         norm[1] = b;
         norm[2] = c;

      } // Point is in previous face

   } // Lookup path

   // -------------------------- Cleanup path ---------------------------------
   else
   {
      // Free memory from the inside out
      for (int i = 0; i < db->nfile; ++i)
      {
         for (int j = 0; j < db->files[i].nsubregion; ++j)
         {
            for (int k = 0; k < db->files[i].subregions[j].nface; ++k)
            {
               free(db->files[i].subregions[j].faces[k].vert);
            } // Face loop

            free(db->files[i].subregions[j].faces);

         } // Subregion loop

         free(db->files[i].subregions);

      } // File loop

      free(db->files);
      free(db);
      db = nullptr;
   } // Cleanup path

   // Return elevation
   return z;
}
// -------------------------------------------------------------------------
//
// *f FUNCTION: in_test
//
// ---------------------------------------------------------------------------
// *m FILENAME: elcompute.c
//
// ---------------------------------------------------------------------------
// *p PURPOSE:
// *p  This routine tests the input point to see if it is in the input face.
// *p  This is a 2D test.  The function returns a 1 if the point is inside,
// *p  0 otherwise.
// *p
// *p
// *p
// ---------------------------------------------------------------------------
// *c CALL METHOD:
// *c   int  (float x, float y, Face face)
// *c
// *c     x, y     - Coordinates of point to be looked up
// *c     Face     - Face data structure
// *c     return   - 0 if point not in, 1 if it is
// *c
//
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
//
bool GeodeticEarthElevCompute::in_test(float       x, // aLat
                                       float       y, // aLon
                                       const Face& face)

{
   const float TOL = .000001f; // Position tolerance


   std::array<float, 3> vert;  // General purpose vertex
   std::array<float, 3> vertp; // Next vertex

   // Initialize inside flag on
   bool inside = true;

   // Initialize retry flag on
   bool retry = true; // Continue testing flag

   // Check if point is on a vertex
   if (inside)
   {
      for (int i = 0; i < face.nvert; ++i)
      {
         vert[0] = face.vert[i].v[0];
         vert[1] = face.vert[i].v[1];
         if ((fabs(x - vert[0]) < TOL) && (fabs(y - vert[1]) < TOL))
         {
            retry = false;
            break;
         }
      }
   }

   // Not a vertex
   if (retry)
   {
      for (int i = 0; i < face.nvert; ++i)
      {
         int ip   = (i < (face.nvert - 1)) ? i + 1 : 0;
         vert[0]  = face.vert[i].v[0];
         vert[1]  = face.vert[i].v[1];
         vertp[0] = face.vert[ip].v[0];
         vertp[1] = face.vert[ip].v[1];

         // Compute vectors from start vertex to point and along edge
         float px = x - vert[0];
         float py = y - vert[1];
         float sx = vertp[0] - vert[0];
         float sy = vertp[1] - vert[1];


         // Compute side crossed with point vector
         float cross = sx * py - px * sy;

         // Compute side dotted with point vector
         float dot = sx * px + sy * py;


         // If cross is negative, point is outside
         if (cross < 0.0f)
         {
            inside = false;
            break;
         }

         // If cross is zero, point may be on edge
         else if (fabs(cross) < TOL)
         {
            // Compute vector lengths (squared)
            float plen = px * px + py * py;
            float slen = sx * sx + sy * sy;

            // Point is on the edge if point vector is shorter and
            // dot product is positive
            if ((plen < slen) && (dot > 0.0f))
            {
               inside = true;
               break;
            }
         }
      }
   }

   return inside;
}

int GeodeticEarthElevCompute::InMeters()
{
   int inMeters = GeodeticEarthDatabase::InMeters();

   return inMeters;
}
