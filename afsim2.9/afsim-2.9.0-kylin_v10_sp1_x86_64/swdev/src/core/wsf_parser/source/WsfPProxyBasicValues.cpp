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

#include "WsfPProxyBasicValues.hpp"

#include <sstream>

#include "UtEllipsoidalEarth.hpp"
#include "UtLexicalCast.hpp"
#include "WsfParseNode.hpp"
#include "WsfParseRule.hpp"

namespace
{
const std::map<std::string, bool> gBoolStrings = {{"true", true},
                                                  {"yes", true},
                                                  {"on", true},
                                                  {"enable", true},
                                                  {"false", false},
                                                  {"no", false},
                                                  {"off", false},
                                                  {"disabled", false}};
}

void WsfProxy::Bool::SetFromString(const std::string& aText)
{
   std::string txt = aText;
   UtStringUtil::ToLower(txt);
   std::map<std::string, bool>::const_iterator iter = gBoolStrings.find(txt);
   if (iter != gBoolStrings.end())
   {
      mValue = iter->second;
      ClearUnset();
   }
}

namespace
{
template<class T>
std::pair<T, int> UtPosFromString(const std::string& posStr)
{
   auto pos = ut::lexical_cast<T>(posStr);
   return std::make_pair(pos, pos.GetFormat());
}
} // namespace

void WsfProxy::Position::Read(WsfParseNode* aNodePtr)
{
   WsfParseNode* end = aNodePtr->Next(false);
   for (auto* n = aNodePtr; n != end; n = n->Next())
   {
      if (n->mRulePtr->mUserId == 1)
      {
         if (!(n->Down() && n->Down()->Down()))
         {
            return;
         }
         WsfParseNode* latNode = n->Down()->Down();
         WsfParseNode* lonNode = latNode->Right();
         if (lonNode)
         {
            try
            {
               std::tie(mLatitude, mLatFormat)  = UtPosFromString<UtLatPos>(WsfParseNode::GetNodeString(latNode));
               std::tie(mLongitude, mLonFormat) = UtPosFromString<UtLonPos>(WsfParseNode::GetNodeString(lonNode));
               mFormat                          = WsfProxy::Position::cLAT_LON;
               ClearUnset();
            }
            catch (ut::bad_lexical_cast&)
            {
               SetUnset();
            }
         }
         break;
      }
      else if (n->mRulePtr->mUserId == 2)
      {
         std::string mgrs = WsfParseNode::GetNodeString(n);
         if (mgrs.size() < 16)
         {
            SetMGRS(mgrs);
            ClearUnset();
         }
         else
         {
            SetUnset();
         }
         break;
      }
   }
}

void WsfProxy::Position::SetMGRS(const std::string& aMGRS)
{
   mFormat = cMGRS;
   for (size_t i = 0; i < 16; ++i)
   {
      if (i < aMGRS.size())
      {
         mMGRS[i] = aMGRS[i];
      }
      else
      {
         mMGRS[i] = '\0';
      }
   }
}

UtLatPos WsfProxy::Position::GetLatitude() const
{
   if (mFormat == Format::cLAT_LON)
   {
      return UtLatPos(mLatitude, mLatFormat);
   }
   else
   {
      double lat, lon;
      UtEllipsoidalEarth::ConvertMGRSToLL(mMGRS, lat, lon);
      return UtLatPos(lat, mLatFormat);
   }
}

UtLonPos WsfProxy::Position::GetLongitude() const
{
   if (mFormat == Format::cLAT_LON)
   {
      return UtLonPos(mLongitude, mLonFormat);
   }
   else
   {
      double lat, lon;
      UtEllipsoidalEarth::ConvertMGRSToLL(mMGRS, lat, lon);
      return UtLonPos(lon, mLonFormat);
   }
}

std::string WsfProxy::Position::PrintLatLon(bool aShowPositionKeyword) const
{
   std::stringstream ss;
   ss.precision(15);
   if (aShowPositionKeyword)
   {
      ss << "position ";
   }
   ss << GetLatitude() << ' ' << GetLongitude();
   return ss.str();
}

std::string WsfProxy::Position::ToString() const
{
   if (IsUnset())
   {
      return "unset";
   }
   // assert(mFormat==cLAT_LON);    // no MGRS support yet
   if (mFormat == cLAT_LON)
   {
      return PrintLatLon(true);
   }
   else if (mFormat == cMGRS)
   {
      std::stringstream ss;
      ss << "mgrs_coordinate " << mMGRS;
      return ss.str();
   }
   else
   {
      assert(mFormat == cMGRS);
   }
   return "";
}

// Initialize all data members in proxy type constructors.
// Otherwise, hashing with the type pointer isn't reliable.
WsfProxy::Position::Position()
   : mFormat(cLAT_LON)
   , mLatFormat(-1)
   , mLonFormat(-1)
   , mLatitude(0.0)
   , mLongitude(0.0)
{
}

WsfProxy::Position::Position(const UtLatPos& aLat, const UtLonPos& aLon)
{
   mFormat = cLAT_LON;
   SetLatitude(aLat);
   SetLongitude(aLon);
}

bool WsfProxy::Position::SetFromString(const std::string& aString)
{
   std::stringstream ss(aString);
   std::string       type;
   ss >> type;
   if (type == "position")
   {
      UtLatPos lat;
      UtLonPos lon;
      if (ss >> lat >> lon)
      {
         mLatitude  = lat;
         mLongitude = lon;
         mLatFormat = lat.GetFormat();
         mLonFormat = lon.GetFormat();
         mFormat    = cLAT_LON;
         return true;
      }
   }
   else if (type == "mgrs_coordinate")
   {
      std::string mgrs;
      if ((ss >> mgrs) && mgrs.size() < 16)
      {
         SetMGRS(mgrs);
         ClearUnset();
         return true;
      }
   }
   return false;
}

bool WsfProxy::Position::Less(const Position& aRhs) const
{
   if ((mBasicValueFlags & cVALUE_BITS) < (aRhs.mBasicValueFlags & cVALUE_BITS))
   {
      return true;
   }
   if ((mBasicValueFlags & cVALUE_BITS) > (aRhs.mBasicValueFlags & cVALUE_BITS))
   {
      return false;
   }
   if (mFormat < aRhs.mFormat)
   {
      return true;
   }
   if (mFormat > aRhs.mFormat)
   {
      return false;
   }
   if (mLatitude < aRhs.mLatitude)
   {
      return true;
   }
   if (mLatitude > aRhs.mLatitude)
   {
      return false;
   }
   if (mLongitude < aRhs.mLongitude)
   {
      return true;
   }
   return false;
}

bool WsfProxy::Position::Equal(const Position& aRhs) const
{
   if (mBasicValueFlags & cVALUE_UNSET & aRhs.mBasicValueFlags)
   {
      return true;
   }
   if ((mBasicValueFlags ^ aRhs.mBasicValueFlags) & cVALUE_UNSET)
   {
      return false;
   }
   return mFormat == aRhs.mFormat && mLatitude == aRhs.mLatitude && mLongitude == aRhs.mLongitude;
}

void WsfProxy::Position::CopyValue(const Position& aRhs)
{
   mBasicValueFlags = (mBasicValueFlags & ~cCOPY_BITS) | (cCOPY_BITS & aRhs.mBasicValueFlags);
   mFormat          = aRhs.mFormat;
   mLatitude        = aRhs.mLatitude;
   mLongitude       = aRhs.mLongitude;
   mLatFormat       = aRhs.mLatFormat;
   mLonFormat       = aRhs.mLonFormat;
}

std::string WsfProxy::Position::Write(WsfParseRule* aRulePtr)
{
   if (aRulePtr->Type() == WsfParseRule::cVALUE)
   {
      return ToString();
   }
   switch (aRulePtr->mUserId)
   {
   case 1: // lat-lon
      return PrintLatLon(false);
   default:
      return ToString();
   }
}

void WsfProxy::Length2::Read(WsfParseNode* aNodePtr)
{
   WsfParseNode* n = aNodePtr->Down();
   if (n && n->Down())
   {
      n      = n->Down();
      auto x = WsfParseNode::GetNodeString(n);
      n      = n->Right();
      if (n)
      {
         auto y = WsfParseNode::GetNodeString(n);
         n      = n->Right();
         if (n)
         {
            auto unit = WsfParseNode::GetNodeString(n);

            int unitId = UtUnitLength::ReadUnit(unit);
            if (unitId != -1)
            {
               mX      = UtStringUtil::ToDouble(x);
               mY      = UtStringUtil::ToDouble(y);
               mFormat = static_cast<unsigned char>(unitId);
               mX      = UtUnitLength::ConvertToStandard(mX, unitId);
               mY      = UtUnitLength::ConvertToStandard(mY, unitId);
               ClearUnset();
            }
         }
      }
   }
}

WsfProxy::Length2::Length2()
   : mFormat(0)
   , mX(0.0)
   , mY(0.0)
{
}

std::string WsfProxy::Length2::ToString() const
{
   if (IsUnset())
   {
      return "unset";
   }
   std::stringstream ss;
   ss.precision(15);
   ss << UtUnitLength::ConvertFromStandard(mX, static_cast<int>(mFormat)) << ' '
      << UtUnitLength::ConvertFromStandard(mY, static_cast<int>(mFormat)) << ' ' << UtUnitLength::FindUnitName(mFormat);
   return ss.str();
}

void WsfProxy::Length2::CopyValue(const Length2& aRhs)
{
   mBasicValueFlags = (mBasicValueFlags & ~cCOPY_BITS) | (cCOPY_BITS & aRhs.mBasicValueFlags);
   mFormat          = aRhs.mFormat;
   mX               = aRhs.mX;
   mY               = aRhs.mY;
}

bool WsfProxy::Length2::Equal(const Length2& aRhs) const
{
   if (mBasicValueFlags & cVALUE_UNSET & aRhs.mBasicValueFlags)
   {
      return true;
   }
   if ((mBasicValueFlags ^ aRhs.mBasicValueFlags) & cVALUE_UNSET)
   {
      return false;
   }
   return mFormat == aRhs.mFormat && mX == aRhs.mX && mY == aRhs.mY;
}

bool WsfProxy::Length2::Less(const Length2& aRhs) const
{
   if ((mBasicValueFlags & cVALUE_BITS) < (aRhs.mBasicValueFlags & cVALUE_BITS))
   {
      return true;
   }
   if ((mBasicValueFlags & cVALUE_BITS) > (aRhs.mBasicValueFlags & cVALUE_BITS))
   {
      return false;
   }
   if (mFormat < aRhs.mFormat)
   {
      return true;
   }
   if (mFormat > aRhs.mFormat)
   {
      return false;
   }
   if (mX < aRhs.mX)
   {
      return true;
   }
   if (mX > aRhs.mX)
   {
      return false;
   }
   if (mY < aRhs.mY)
   {
      return true;
   }
   return false;
}

bool WsfProxy::Length2::SetFromString(const std::string& aString)
{
   std::stringstream ss(aString);
   double            x, y;
   std::string       unit;
   if (ss >> x >> y >> unit)
   {
      int unitId = UtUnitLength::ReadUnit(unit);
      if (unitId != -1)
      {
         mX      = x;
         mY      = y;
         mFormat = static_cast<unsigned char>(unitId);
         return true;
      }
   }
   return false;
}


void WsfProxy::Length3::Read(WsfParseNode* aNodePtr)
{
   WsfParseNode* n = aNodePtr->Down();
   if (n && n->Down())
   {
      n      = n->Down();
      auto x = WsfParseNode::GetNodeString(n);
      n      = n->Right();
      if (n)
      {
         auto y = WsfParseNode::GetNodeString(n);
         n      = n->Right();
         if (n)
         {
            auto z = WsfParseNode::GetNodeString(n);
            n      = n->Right();

            if (n)
            {
               auto unit = WsfParseNode::GetNodeString(n);

               int unitId = UtUnitLength::ReadUnit(unit);
               if (unitId != -1)
               {
                  mX      = UtStringUtil::ToDouble(x);
                  mY      = UtStringUtil::ToDouble(y);
                  mFormat = static_cast<unsigned char>(unitId);
                  mX      = UtUnitLength::ConvertToStandard(mX, unitId);
                  mY      = UtUnitLength::ConvertToStandard(mY, unitId);
                  ClearUnset();
               }
            }
         }
      }
   }
}

WsfProxy::Length3::Length3()
{
   mFormat = 0;
   mX      = 0.0;
   mY      = 0.0;
   mZ      = 0.0;
}

std::string WsfProxy::Length3::ToString() const
{
   if (IsUnset())
   {
      return "unset";
   }
   std::stringstream ss;
   ss.precision(15);
   ss << UtUnitLength::ConvertFromStandard(mX, static_cast<int>(mFormat)) << ' ';
   ss << UtUnitLength::ConvertFromStandard(mY, static_cast<int>(mFormat)) << ' ';
   ss << UtUnitLength::ConvertFromStandard(mZ, static_cast<int>(mFormat)) << ' ';
   ss << UtUnitLength::FindUnitName(mFormat);
   return ss.str();
}

void WsfProxy::Length3::CopyValue(const Length3& aRhs)
{
   mBasicValueFlags = (mBasicValueFlags & ~cCOPY_BITS) | (cCOPY_BITS & aRhs.mBasicValueFlags);
   mFormat          = aRhs.mFormat;
   mX               = aRhs.mX;
   mY               = aRhs.mY;
   mZ               = aRhs.mZ;
}

bool WsfProxy::Length3::Equal(const Length3& aRhs) const
{
   if (mBasicValueFlags & cVALUE_UNSET & aRhs.mBasicValueFlags)
   {
      return true;
   }
   if ((mBasicValueFlags ^ aRhs.mBasicValueFlags) & cVALUE_UNSET)
   {
      return false;
   }
   return mFormat == aRhs.mFormat && mX == aRhs.mX && mY == aRhs.mY && mZ == aRhs.mZ;
}

bool WsfProxy::Length3::Less(const Length3& aRhs) const
{
   if ((mBasicValueFlags & cVALUE_BITS) < (aRhs.mBasicValueFlags & cVALUE_BITS))
   {
      return true;
   }
   if ((mBasicValueFlags & cVALUE_BITS) > (aRhs.mBasicValueFlags & cVALUE_BITS))
   {
      return false;
   }
   if (mFormat < aRhs.mFormat)
   {
      return true;
   }
   if (mFormat > aRhs.mFormat)
   {
      return false;
   }
   if (mX < aRhs.mX)
   {
      return true;
   }
   if (mX > aRhs.mX)
   {
      return false;
   }
   if (mY < aRhs.mY)
   {
      return true;
   }
   if (mY > aRhs.mY)
   {
      return false;
   }
   if (mZ < aRhs.mZ)
   {
      return true;
   }
   return false;
}

bool WsfProxy::Length3::SetFromString(const std::string& aString)
{
   std::stringstream ss(aString);
   double            x, y, z;
   std::string       unit;
   if (ss >> x >> y >> z >> unit)
   {
      int unitId = UtUnitLength::ReadUnit(unit);
      if (unitId != -1)
      {
         mX      = x;
         mY      = y;
         mZ      = z;
         mFormat = (unsigned char)unitId;
         return true;
      }
   }
   return false;
}
