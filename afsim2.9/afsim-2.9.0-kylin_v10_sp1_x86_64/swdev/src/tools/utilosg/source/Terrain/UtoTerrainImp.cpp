// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

// UtoTerrainImp.cpp: implementation of the UtoTerrainImp class.
//
//////////////////////////////////////////////////////////////////////

#include "UtoTerrainImp.hpp"

#include <assert.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>

#include "UtoGeometryUtil.hpp"
#include "UtoOptions.hpp"
#include "UtoPriv.hpp"

// osg includes
#include <osgUtil/IntersectVisitor>

#ifdef _WIN32
#define STRNCASECMP _strnicmp
#else
#define STRNCASECMP strncasecmp
#endif

Utok::Properties_T<UtoTerrainImp> UtoTerrainImp::m_GenericProperties;

// the ray that will be used by the HOT function
static osg::ref_ptr<osg::LineSegment> g_pRay = nullptr;
// defines the basic height of terrain attributes
bool HeightOfTerrain(osg::Node* pRoot, double* rPosn, UtoOrientation* pOrient)
{
   // local(s)
   bool bResult = false;

   // validate the values passed in
   if (pRoot)
   {
      // create a intersect visitor
      osgUtil::IntersectVisitor oCaster;
      // create a ray for the caster to use
      if (!g_pRay)
         g_pRay = new osg::LineSegment;
      // add the points to the ray
      g_pRay->set(osg::Vec3(rPosn[0], rPosn[1], -1000.0), osg::Vec3(rPosn[0], rPosn[1], 30000.0));

      // add the ray to the caster
      oCaster.addLineSegment(g_pRay.get());

      // cast the ray into the scene
      pRoot->accept(oCaster);

      // validate there were some hits
      if (oCaster.hits())
      {
         // obtain the items that were hit from ray
         osgUtil::IntersectVisitor::HitList& rList = oCaster.getHitList(g_pRay.get());
         // validate the ray hit something
         if (!rList.empty())
         {
            // obtain the front item
            osgUtil::IntersectVisitor::HitList::iterator itFront = rList.begin();

            // obtain the world intersected point
            rPosn[2] = itFront->getWorldIntersectPoint().z();

            // calculate the orientation
            if (pOrient)
            {
               // orientation code from Mark Drajeske
               // north and east directions corresponds to y and x
               // obtain the normal intersected
               osg::Vec3 vWorldNormal = itFront->getWorldIntersectNormal();
               // construct the north, east, and down unit
               osg::Vec3 vNormalNED(vWorldNormal.y(), vWorldNormal.x(), -vWorldNormal.z());

               // cap the z value if it is really small
               if (fabs(vNormalNED.z()) < 0.00001)
                  vNormalNED.z() = vNormalNED.z() > 0 ? 0.00001 : -0.00001;

               // find the cos and sin of the heading
               double dYawRads = DtoR(pOrient->yaw);
               double dCosYaw  = cos(dYawRads);
               double dSinYaw  = sin(dYawRads);
               // find the point in front using the plane equation
               osg::Vec3 vFrontPt(dCosYaw, dSinYaw, (-vNormalNED.x() * dCosYaw - vNormalNED.y() * dSinYaw) / vNormalNED.z());
               // compute the angle
               double dSinTheta = -vFrontPt.z() / vFrontPt.length();

               // cap theta
               if (-1.0 > dSinTheta || dSinTheta > 1.0)
                  dSinTheta = dSinTheta > 0 ? 1.0 : -1.0;

               // assign to pitch
               pOrient->pitch = RtoD(asin(dSinTheta));

               // find the point to the right using the plane equation
               osg::Vec3 vRightPt(-dSinYaw,
                                  dCosYaw,
                                  (-vNormalNED.x() * -dSinYaw - vNormalNED.y() * dCosYaw) / vNormalNED.z());
               // compute the roll angle based on the right pt
               double dSinPhi = vRightPt.z() / vRightPt.length();

               // cap phi
               if (-1.0 > dSinPhi || dSinPhi > 1.0)
                  dSinPhi = dSinPhi > 0 ? 1.0 : -1.0;

               // assign to roll
               pOrient->roll = RtoD(asin(dSinPhi));
            }

            // indicate a hit
            bResult = true;
         }
      }
   }

   return bResult;
}

bool IntersectTerrain(osg::Node*    pRoot,
                      const double* rPosition,
                      const double* rVector,
                      double*       rIntersectPoint,
                      double*       rIntersectNormal,
                      double        aMagnitude)
{
   // local(s)
   bool bResult = false;

   // validate the values passed in
   if (pRoot)
   {
      // create a intersect visitor
      osgUtil::IntersectVisitor oCaster;
      // create a ray for the caster to use
      if (!g_pRay)
         g_pRay = new osg::LineSegment();
      // add the points to the ray
      g_pRay->set(osg::Vec3d(rPosition[0] - rVector[0] * aMagnitude,
                             rPosition[1] - rVector[1] * aMagnitude,
                             rPosition[2] - rVector[2] * aMagnitude),
                  osg::Vec3d(rPosition[0] + rVector[0] * aMagnitude,
                             rPosition[1] + rVector[1] * aMagnitude,
                             rPosition[2] + rVector[2] * aMagnitude));

      // add the ray to the caster
      oCaster.addLineSegment(g_pRay.get());

      // cast the ray into the scene
      pRoot->accept(oCaster);

      // validate there were some hits
      if (oCaster.hits())
      {
         // obtain the items that were hit from ray
         osgUtil::IntersectVisitor::HitList& rList = oCaster.getHitList(g_pRay.get());
         // validate the ray hit something
         if (!rList.empty())
         {
            osg::Vec3d ip      = rList.front().getWorldIntersectPoint();
            rIntersectPoint[0] = ip[0];
            rIntersectPoint[1] = ip[1];
            rIntersectPoint[2] = ip[2];

            osg::Vec3d nv       = rList.front().getWorldIntersectNormal();
            rIntersectNormal[0] = nv[0];
            rIntersectNormal[1] = nv[1];
            rIntersectNormal[2] = nv[2];

            // indicate a hit
            bResult = true;
         }
      }
   }

   return bResult;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UtoTerrainImp::UtoTerrainImp()
   : m_oBlendColor(UtoColor(0xFF, 0xFF, 0xFF, 0xFF))
   , m_sName("")
   , m_pProperties(nullptr)
   , m_bDisableColorBlending(false)
   , MakeProjection(nullptr)
   , m_pProjection()
{
   InitGenericProperties();
}

UtoTerrainImp::UtoTerrainImp(const UtoTerrainImp& rhs)
   : m_oBlendColor(rhs.m_oBlendColor)
   , m_sName(rhs.Name())
   , m_pProperties(nullptr)
   , m_bDisableColorBlending(rhs.m_bDisableColorBlending)
   , m_EQRParams(rhs.m_EQRParams)
   , MakeProjection(rhs.MakeProjection)
   , m_pProjection()
{
   if (MakeProjection)
   {
      UtoMapProjection* tempProj = (this->*MakeProjection)();
      m_pProjection              = *tempProj;
      delete tempProj;
   }
}

UtoTerrainImp::~UtoTerrainImp() {}

#if 0
bool UtoTerrainImp::Load( std::istream& is )
{
   const int BUFSIZE = 128;
   std::vector<char> buffer (BUFSIZE);
   bool done = false;

   if (!PreLoad ())
      return false;

   // read in section details.
   while (is.good () && !done)
   {
      std::istream::pos_type pos = is.tellg ();
      is.getline (&buffer[0], buffer.size ());
      buffer.resize (is.gcount ());

      if (buffer.size () == 0 || buffer[0] == '\0')
      {
         // blank line, skip.
      }
      else if (buffer[0] == '#')
      {
         // this line is a comment, ignore.
      }
      else if (STRNCASECMP("terrain_type", &buffer[0], 12) == 0)
      {
         // starting another terrain definition.
         done = true;

         // must put the seek pointer back to beginning of this line.
         is.seekg (pos);
      }
      else if (!is.eof ())
      {
         std::istringstream sstr (std::string(buffer.begin(), buffer.end()));
         std::string attr;
         sstr >> attr;

         //std::string buf (buffer.begin() + attr.length()+1, buffer.end());
         const char* temp = &buffer[attr.length()+1];
         std::string buf (temp, strlen(temp));
         ReadAttribute (attr, buf);
      }

      // resize the buffer.
      buffer.resize (BUFSIZE);
   }

   return PostLoad ();
}
#endif

bool UtoTerrainImp::PreLoad()
{
   return true;
}

bool UtoTerrainImp::PostLoad()
{
   if (MakeProjection)
   {
      UtoMapProjection* tempProj = (this->*MakeProjection)();
      m_pProjection              = *tempProj;
      delete tempProj;
   }
   return true;
}

bool UtoTerrainImp::ReadAttribute(const std::string& attr, const std::string& buf)
{
   if (!m_GenericProperties.Dispatch(this, attr, buf))
      return m_pProperties->Dispatch(this, attr, buf);
   return true;
}

void UtoTerrainImp::InitGenericProperties()
{
   // reset member variables
   memset(&m_EQRParams, 0x00, sizeof(m_EQRParams));

   if (m_GenericProperties.IsEmpty())
   {
      // m_GenericProperties.Set("models_path", ReadPath);
      m_GenericProperties.Set("projection", &UtoTerrainImp::ReadProjection);

      // Common properties
      m_GenericProperties.Set("earth_model", &UtoTerrainImp::ReadEarthModel);
      m_GenericProperties.Set("angle_units", &UtoTerrainImp::ReadAngleUnits);
      m_GenericProperties.Set("dist_units", &UtoTerrainImp::ReadDistUnits);

      // LCC properties
      m_GenericProperties.Set("lcc_lat_origin", &UtoTerrainImp::ReadLCCParam);
      m_GenericProperties.Set("lcc_lon_origin", &UtoTerrainImp::ReadLCCParam);
      m_GenericProperties.Set("lcc_parallel1", &UtoTerrainImp::ReadLCCParam);
      m_GenericProperties.Set("lcc_parallel2", &UtoTerrainImp::ReadLCCParam);

      // TM properties
      m_GenericProperties.Set("tm_central_meridian", &UtoTerrainImp::ReadTMParam);
      m_GenericProperties.Set("tm_false_easting", &UtoTerrainImp::ReadTMParam);
      m_GenericProperties.Set("tm_false_northing", &UtoTerrainImp::ReadTMParam);
      m_GenericProperties.Set("tm_k0", &UtoTerrainImp::ReadTMParam);

      // EQR properties
      m_GenericProperties.Set("eqr_lat_origin", &UtoTerrainImp::ReadEQRParam);
      m_GenericProperties.Set("eqr_lon_origin", &UtoTerrainImp::ReadEQRParam);

      // color properties
      m_GenericProperties.Set("color3", &UtoTerrainImp::ReadColorParam);
      m_GenericProperties.Set("color4", &UtoTerrainImp::ReadColorParam);

      // disables color blending
      m_GenericProperties.Set("disable_color_blending", &UtoTerrainImp::ReadDisableColorBlending);
   }
}

bool UtoTerrainImp::ReadProjection(const std::string& attr, const std::string& buf)
{
   if (STRNCASECMP(buf.c_str(), "LCC", 3) == 0)
   {
      return false;
      //      MakeProjection = &UtoTerrainImp::MakeLCCProjection;
   }
   else if (STRNCASECMP(buf.c_str(), "TM", 2) == 0)
   {
      return false;
      //      MakeProjection = &UtoTerrainImp::MakeTMProjection;
   }
   else if (STRNCASECMP(buf.c_str(), "EQR", 3) == 0)
   {
      return false;
      //      MakeProjection = &UtoTerrainImp::MakeEQRProjection;
   }
   else if (STRNCASECMP(buf.c_str(), "CME_EQR", 7) == 0)
   {
      MakeProjection = &UtoTerrainImp::MakeCME_EQRProjection;
   }
   else if (STRNCASECMP(buf.c_str(), "CME_WCS", 7) == 0)
   {
      MakeProjection = &UtoTerrainImp::MakeCME_WCSProjection;
   }
   else if (STRNCASECMP(buf.c_str(), "WCS", 3) == 0)
   {
      return false;
      //      MakeProjection = &UtoTerrainImp::MakeWCSProjection;
   }
   else if (STRNCASECMP(buf.c_str(), "GDC", 3) == 0)
   {
      return false;
      //      MakeProjection = &UtoTerrainImp::MakeGDCProjection;
   }
   return true;
}

bool UtoTerrainImp::ReadEarthModel(const std::string& attr, const std::string& buf)
{
   if (STRNCASECMP(buf.c_str(), "WGS84", 5) == 0)
   {
      //      m_CommonParams.em = CCU::WGS84;
   }
   else if (STRNCASECMP(buf.c_str(), "GRS80", 5) == 0)
   {
      //      m_CommonParams.em = CCU::GRS80;
   }
   else if (STRNCASECMP(buf.c_str(), "CLARKE66", 8) == 0)
   {
      //      m_CommonParams.em = CCU::CLARKE66;
   }
   else if (STRNCASECMP(buf.c_str(), "EQR_TERRA_VISTA", 15) == 0)
   {
      //      m_CommonParams.em = CCU::EQR_TERRA_VISTA;
   }
   else if (STRNCASECMP(buf.c_str(), "EQR_MULTIGEN", 12) == 0)
   {
      //      m_CommonParams.em = CCU::EQR_MULTIGEN_CREATOR;
   }
   else if (STRNCASECMP(buf.c_str(), "EQR_COMPUSCENE", 14) == 0)
   {
      //      m_CommonParams.em = CCU::EQR_COMPUSCENE;
   }
   else
   {
      // <radius>,<flattening>
      std::istringstream sstr(buf);
      double             r, f;
      char               c;
      sstr >> r >> c >> f;
      if (!sstr.fail())
      {
         //         m_CommonParams.em.radius = r;
         //         m_CommonParams.em.flattening = f;
      }
   }

   return true;
}

bool UtoTerrainImp::ReadAngleUnits(const std::string& attr, const std::string& buf)
{
   if (STRNCASECMP(buf.c_str(), "RAD", 3) == 0 || STRNCASECMP(buf.c_str(), "RADIANS", 7) == 0)
   {
      //      m_CommonParams.angles = CCU::RADIANS;
   }
   else if (STRNCASECMP(buf.c_str(), "DEG", 3) == 0 || STRNCASECMP(buf.c_str(), "DEGREES", 7) == 0)
   {
      //      m_CommonParams.angles = CCU::DEGREES;
   }

   return true;
}

bool UtoTerrainImp::ReadDistUnits(const std::string& attr, const std::string& buf)
{
   if (STRNCASECMP(buf.c_str(), "FT", 2) == 0 || STRNCASECMP(buf.c_str(), "FEET", 4) == 0)
   {
      //      m_CommonParams.distance = CCU::FEET;
   }
   else if (STRNCASECMP(buf.c_str(), "MT", 1) == 0 || STRNCASECMP(buf.c_str(), "METERS", 6) == 0)
   {
      //      m_CommonParams.distance = CCU::METERS;
   }

   return true;
}

bool UtoTerrainImp::ReadLCCParam(const std::string& attr, const std::string& buf)
{
   double val;
   if (attr.compare(4, 10, "lat_origin") == 0)
   {
      if (ReadLatLon(buf, val) == 0)
      {
         //         m_LCCParams.lat = val;
      }
   }
   else if (attr.compare(4, 10, "lon_origin") == 0)
   {
      if (ReadLatLon(buf, val) == 0)
      {
         //         m_LCCParams.lon = val;
      }
   }
   else if (attr.compare(4, 8, "parallel") == 0)
   {
      if (attr[12] == '1')
      {
         if (Utok::ReadSingle(buf, val) == 0)
         {
            //            m_LCCParams.par1 = val;
         }
      }
      else if (attr[12] == '2')
      {
         if (Utok::ReadSingle(buf, val) == 0)
         {
            //            m_LCCParams.par2 = val;
         }
      }
   }

   return true;
}

bool UtoTerrainImp::ReadTMParam(const std::string& attr, const std::string& buf)
{
   double val;
   if (attr.compare(3, 16, "central_meridian") == 0)
   {
      if (Utok::ReadSingle(buf, val) == 0)
      {
         //         m_TMParams.c_meridian = val;
      }
   }
   else if (attr.compare(3, 6, "false_") == 0)
   {
      if (attr.compare(9, 7, "easting") == 0)
      {
         if (Utok::ReadSingle(buf, val) == 0)
         {
            //            m_TMParams.easting = val;
         }
      }
      else if (attr.compare(9, 8, "northing") == 0)
      {
         if (Utok::ReadSingle(buf, val) == 0)
         {
            //            m_TMParams.northing = val;
         }
      }
   }
   else if (attr.compare(3, 2, "k0") == 0)
   {
      if (Utok::ReadSingle(buf, val) == 0)
      {
         //         m_TMParams.k0 = val;
      }
   }

   return true;
}

bool UtoTerrainImp::ReadEQRParam(const std::string& attr, const std::string& buf)
{
   double val;
   if (attr.compare(4, 10, "lat_origin") == 0)
   {
      if (ReadLatLon(buf, val) == 0)
      {
         m_EQRParams.lat = val;
      }
   }
   else if (attr.compare(4, 10, "lon_origin") == 0)
   {
      if (ReadLatLon(buf, val) == 0)
      {
         m_EQRParams.lon = val;
      }
   }

   return true;
}

int UtoTerrainImp::ReadColor3(const std::string& in, UtoColor& clr)
{
   // "<red> <green> <blue>"
   std::istringstream sstr(in);
   int                n;
   sstr >> n;
   clr.SetRed((unsigned char)n);
   if (sstr.fail())
      return -1;

   sstr >> n;
   clr.SetGreen((unsigned char)n);
   if (sstr.fail())
      return -1;

   sstr >> n;
   clr.SetBlue((unsigned char)n);
   if (sstr.fail())
      return -1;

   // set the alpha value
   clr.SetAlpha(0xFF);

   return 0;
}

int UtoTerrainImp::ReadColor4(const std::string& in, UtoColor& clr)
{
   // local(s)
   int                nValue = 0;
   std::istringstream sstream(in);

   // read the red component
   sstream >> nValue;
   if (sstream.fail())
      return -1;
   clr.SetRed((unsigned char)nValue);

   // read the green component
   sstream >> nValue;
   if (sstream.fail())
      return -1;
   clr.SetGreen((unsigned char)nValue);

   // read the blue component
   sstream >> nValue;
   if (sstream.fail())
      return -1;
   clr.SetBlue((unsigned char)nValue);

   // read the alpha component
   sstream >> nValue;
   if (sstream.fail())
      return -1;
   clr.SetAlpha((unsigned char)nValue);

   return 0;
}

int UtoTerrainImp::ReadLatLon(const std::string& in, double& value)
{
   int    deg = 0, min = 0;
   char   type = 0;
   double temp;

   size_t loc = in.find_first_of(" .:NnSsWwEe");
   assert(loc != in.npos);
   if (in[loc] == '.')
   {
      value = atof(in.c_str());
      loc   = in.find_first_of("NnSsWwEe");
      if (loc != in.npos)
         type = in[loc];
   }
   else if (in[loc] == ' ' || in[loc] == ':')
   {
      deg         = atoi(in.c_str());
      size_t loc2 = in.find_first_of(" .:'NnSsWwEe", loc + 1);
      assert(loc2 != in.npos);

      if (in[loc2] == '.')
      {
         temp  = atof(&in[loc + 1]);
         value = deg + (temp / 60.0);
         loc   = in.find_first_of("NnSsWwEe", loc + 1);
         if (loc != in.npos)
            type = in[loc];
      }
      else if (in[loc2] == ' ' || in[loc2] == ':' || in[loc2] == '\'')
      {
         min  = atoi(&in[loc + 1]);
         loc  = loc2;
         loc2 = in.find_first_of("\"NnSsWwEe", loc + 1);
         assert(loc2 != in.npos);

         temp  = atof(&in[loc + 1]);
         value = deg + (double(min) / 60.0) + (temp / 3600.0);

         loc = in.find_first_of("NnSsWwEe", loc + 1);
         if (loc != in.npos)
            type = in[loc];
      }
      else
      {
         temp  = atof(&in[loc + 1]);
         value = deg + (temp / 60.0);
         type  = in[loc2];
      }
   }
   else
   {
      value = atof(in.c_str());
      type  = in[loc];
   }

   if (type == 'W' || type == 'w' || type == 'S' || type == 's')
      value *= -1.0;

   return 0;

#if 0
   // <ddd.ffffD>
   int read = sscanf(in.c_str(), "%f%c", &remainder, &type);
   if ((read == 2 && strchr("NnSsWwEe", type) != 0) || (read == 1 && type == 0))
   {
      assert(deg >= -180 && deg <= 180);
      value = remainder;
      if (type == 'W' || type == 'w' || type == 'S' || type == 's')
         value *= -1.0;
      return 0;
   }

   // <ddd mm'ss"D>
   type = 0;
   read = sscanf(in.c_str(), "%d %d'%f''%c", &deg, &min, &remainder, &type);
   if ((read == 4 && strchr("NnSsWwEe", type) != 0) || (read == 3 && type == 0))
   {
      assert(deg >= -180 && deg <= 180);
      assert(remainder <= 60.0); // seconds < 60

      value = deg + (double(min) / 60.0) + (remainder / 60000.0);
      if (type == 'W' || type == 'w' || type == 'S' || type == 's')
         value *= -1.0;
      return 0;
   }

   // <ddd mm.ffff'D>
   type = 0;
   read = sscanf(in.c_str(), "%d %f'%c", &deg, &remainder, &type);
   if ((read == 3 && strchr("NnSsWwEe", type) != 0) || (read == 2 && type == 0))
   {
      assert(deg >= -180 && deg <= 180);
      assert(remainder <= 60.0); // minutes < 60

      value = deg + (remainder / 60.0);
      if (type == 'W' || type == 'w' || type == 'S' || type == 's')
         value *= -1.0;
      return 0;
   }

   // failed to decode lat/lon input
   assert(false);

   return -1;
#endif
}

UtoMapProjection* UtoTerrainImp::MakeCME_EQRProjection()
{
   UtoMapProjection* out = new UtoMapProjection();
   out->projection       = UtoMapProjection::Equirectangular;
   out->longitudeOffset  = m_EQRParams.lon;
   return out;
}

UtoMapProjection* UtoTerrainImp::MakeCME_WCSProjection()
{
   return new UtoMapProjection();
}

bool UtoTerrainImp::ReadColorParam(const std::string& attr, const std::string& buf)
{
   // local(s)
   bool bResult = false;

   // compare the strings
   if (attr == "color3")
      bResult = ReadColor3(buf, m_oBlendColor) == 0;
   else if (attr == "color4")
      bResult = ReadColor4(buf, m_oBlendColor) == 0;

   // reset the color to white if blending has been disabled
   if (m_bDisableColorBlending)
   {
      m_oBlendColor = UtoColor(0xFF, 0xFF, 0xFF, 0xFF);
   }

   return bResult;
}

void UtoTerrainImp::SetColor(const UtoColor& rColor, unsigned int nClrMask)
{
   if (!IsColorBlendingDisabled())
   {
      m_oBlendColor.Set(rColor, nClrMask);
   }
}

bool UtoTerrainImp::ReadDisableColorBlending(const std::string& attr, const std::string& buf)
{
   // color blending has been disabled
   m_bDisableColorBlending = true;

   // the new color is white
   m_oBlendColor = UtoColor(0xFF, 0xFF, 0xFF, 0xFF);

   return true;
}

void UtoTerrainImp::SetUniform(osg::Node* node, const std::string& name, const float value)
{
   if (node != nullptr)
   {
      osg::StateSet* ss      = node->getOrCreateStateSet();
      osg::Uniform*  uniform = ss->getOrCreateUniform(name, osg::Uniform::FLOAT);
      uniform->set(value);
   }
}

void UtoTerrainImp::SetUniform(osg::Node* node, const std::string& name, const float value[4])
{
   if (node != nullptr)
   {
      osg::StateSet* ss      = node->getOrCreateStateSet();
      osg::Uniform*  uniform = ss->getOrCreateUniform(name, osg::Uniform::FLOAT_VEC4);
      osg::Vec4      vec(value[0], value[1], value[2], value[3]);
      uniform->set(vec);
   }
}

void UtoTerrainImp::SetUniformMatrix(osg::Node* node, const std::string& name, const double value[][4])
{
   if (node != nullptr)
   {
      osg::StateSet* ss      = node->getOrCreateStateSet();
      osg::Uniform*  uniform = ss->getOrCreateUniform(name, osg::Uniform::FLOAT_MAT4);
      osg::Matrixf   mymat(value[0][0],
                         value[0][1],
                         value[0][2],
                         value[0][3],
                         value[1][0],
                         value[1][1],
                         value[1][2],
                         value[1][3],
                         value[2][0],
                         value[2][1],
                         value[2][2],
                         value[2][3],
                         value[3][0],
                         value[3][1],
                         value[3][2],
                         value[3][3]);
      uniform->set(mymat);
   }
}

void UtoTerrainImp::SetUniformInverseMatrix(osg::Node* node, const std::string& name, const double value[][4])
{
   if (node != nullptr)
   {
      osg::StateSet* ss      = node->getOrCreateStateSet();
      osg::Uniform*  uniform = ss->getOrCreateUniform(name, osg::Uniform::FLOAT_MAT4);
      osg::Matrixf   mymat(value[0][0],
                         value[0][1],
                         value[0][2],
                         value[0][3],
                         value[1][0],
                         value[1][1],
                         value[1][2],
                         value[1][3],
                         value[2][0],
                         value[2][1],
                         value[2][2],
                         value[2][3],
                         value[3][0],
                         value[3][1],
                         value[3][2],
                         value[3][3]);
      uniform->set(osg::Matrixf::inverse(mymat));
   }
}

void UtoTerrainImp::SetProjection(const std::string& aProjection, const double& aOffset)
{
   m_EQRParams.lon = aOffset;
   if (aProjection == "CME_WCS")
      MakeProjection = &UtoTerrainImp::MakeCME_WCSProjection;
   else if (aProjection == "CME_EQR")
      MakeProjection = &UtoTerrainImp::MakeCME_EQRProjection;
   else
   {
      MakeProjection = &UtoTerrainImp::MakeCME_WCSProjection;
   }
}
