// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "UtScriptColor.hpp"

#include <memory>
#include <sstream>

#include "UtColor.hpp"
#include "UtMemory.hpp"
#include "UtScriptMethodDefine.hpp"
#include "UtScriptTypes.hpp"

namespace
{
UtScriptColor* sScriptColorClassPtr = nullptr;
}

// =================================================================================================
UtScriptColor::UtScriptColor(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   UT_SCRIPT_WRAP_CLASS(UtColor);
   sScriptColorClassPtr = this;
   mConstructible       = true;
   mCloneable           = true;
   mEqualityComparable  = true;

   UT_SCRIPT_STATIC_METHOD(std::unique_ptr<UtColor>, Construct, (int aRed, int aGreen, int aBlue))
   {
      return ut::make_unique<UtColor>(aRed / 255.F, aGreen / 255.F, aBlue / 255.F);
   };

   UT_SCRIPT_STATIC_METHOD(std::unique_ptr<UtColor>, Construct, (int aRed, int aGreen, int aBlue, int aAlpha))
   {
      return ut::make_unique<UtColor>(aRed / 255.F, aGreen / 255.F, aBlue / 255.F, aAlpha / 255.F);
   };

   UT_SCRIPT_STATIC_METHOD(std::unique_ptr<UtColor>, Construct, (const std::string& aHexString))
   {
      return ut::make_unique<UtColor>(aHexString);
   };

   UT_SCRIPT_STATIC_METHOD(const UtColor&, Black, ()) { return UtColor::GetColorMapping("black"); };
   UT_SCRIPT_STATIC_METHOD(const UtColor&, Blue, ()) { return UtColor::GetColorMapping("blue"); };
   UT_SCRIPT_STATIC_METHOD(const UtColor&, Brown, ()) { return UtColor::GetColorMapping("brown"); };
   UT_SCRIPT_STATIC_METHOD(const UtColor&, DarkBlue, ()) { return UtColor::GetColorMapping("dark_blue"); };
   UT_SCRIPT_STATIC_METHOD(const UtColor&, DarkGreen, ()) { return UtColor::GetColorMapping("dark_green"); };
   UT_SCRIPT_STATIC_METHOD(const UtColor&, DarkPurple, ()) { return UtColor::GetColorMapping("dark_purple"); };
   UT_SCRIPT_STATIC_METHOD(const UtColor&, DarkRed, ()) { return UtColor::GetColorMapping("dark_red"); };
   UT_SCRIPT_STATIC_METHOD(const UtColor&, DarkYellow, ()) { return UtColor::GetColorMapping("dark_yellow"); };
   UT_SCRIPT_STATIC_METHOD(const UtColor&, Green, ()) { return UtColor::GetColorMapping("green"); };
   UT_SCRIPT_STATIC_METHOD(const UtColor&, Indigo, ()) { return UtColor::GetColorMapping("indigo"); };
   UT_SCRIPT_STATIC_METHOD(const UtColor&, LightBlue, ()) { return UtColor::GetColorMapping("light_blue"); };
   UT_SCRIPT_STATIC_METHOD(const UtColor&, LightGreen, ()) { return UtColor::GetColorMapping("light_green"); };
   UT_SCRIPT_STATIC_METHOD(const UtColor&, LightPurple, ()) { return UtColor::GetColorMapping("light_purple"); };
   UT_SCRIPT_STATIC_METHOD(const UtColor&, LightRed, ()) { return UtColor::GetColorMapping("light_red"); };
   UT_SCRIPT_STATIC_METHOD(const UtColor&, LightYellow, ()) { return UtColor::GetColorMapping("light_yellow"); };
   UT_SCRIPT_STATIC_METHOD(const UtColor&, Magenta, ()) { return UtColor::GetColorMapping("magenta"); };
   UT_SCRIPT_STATIC_METHOD(const UtColor&, Orange, ()) { return UtColor::GetColorMapping("orange"); };
   UT_SCRIPT_STATIC_METHOD(const UtColor&, Pink, ()) { return UtColor::GetColorMapping("pink"); };
   UT_SCRIPT_STATIC_METHOD(const UtColor&, Purple, ()) { return UtColor::GetColorMapping("purple"); };
   UT_SCRIPT_STATIC_METHOD(const UtColor&, Red, ()) { return UtColor::GetColorMapping("red"); };
   UT_SCRIPT_STATIC_METHOD(const UtColor&, Tan, ()) { return UtColor::GetColorMapping("tan"); };
   UT_SCRIPT_STATIC_METHOD(const UtColor&, Violet, ()) { return UtColor::GetColorMapping("violet"); };
   UT_SCRIPT_STATIC_METHOD(const UtColor&, White, ()) { return UtColor::GetColorMapping("white"); };
   UT_SCRIPT_STATIC_METHOD(const UtColor&, Yellow, ()) { return UtColor::GetColorMapping("yellow"); };

   UT_SCRIPT_METHOD(int, GetRed, ())
   {
      float rgb[4];
      self.Get(rgb);
      return static_cast<int>(rgb[0] * 255);
   };

   UT_SCRIPT_METHOD(int, GetGreen, ())
   {
      float rgb[4];
      self.Get(rgb);
      return static_cast<int>(rgb[1] * 255);
   };

   UT_SCRIPT_METHOD(int, GetBlue, ())
   {
      float rgb[4];
      self.Get(rgb);
      return static_cast<int>(rgb[2] * 255);
   };

   UT_SCRIPT_METHOD(int, GetAlpha, ())
   {
      float rgb[4];
      self.Get(rgb);
      return static_cast<int>(rgb[3] * 255);
   };

   // =================================================================================================
   UT_SCRIPT_METHOD(std::string, GetHexString, ())
   {
      self.SetFormat(UtColor::FmtHexAlpha);
      std::ostringstream oss;
      oss << self;
      return oss.str();
   };

   // =================================================================================================
   UT_SCRIPT_METHOD(std::string, GetRGBA_String, ())
   {
      float rgb[4];
      self.Get(rgb);
      std::ostringstream oss;
      oss << rgb[0] * 255 << ' ' << rgb[1] * 255 << ' ' << rgb[2] * 255 << ' ' << rgb[3] * 255;
      return oss.str();
   };

   // =================================================================================================
   UT_SCRIPT_METHOD(void, Set, (int aRed, int aGreen, int aBlue))
   {
      self.Set(aRed / 255.0F, aGreen / 255.0F, aBlue / 255.0F);
   };

   UT_SCRIPT_METHOD(void, Set, (const UtColor& aColor)) { self.Set(aColor); };

   UT_SCRIPT_METHOD(void, SetAlpha, (int aAlpha))
   {
      float rgb[3];
      float alpha;
      self.Get(rgb, alpha);
      self.Set(rgb, static_cast<float>(aAlpha) / 255.0F);
   };
}

//! Creates a new script reference of a new UtColor object
UtScriptRef* UtScriptColor::Create(const UtColor& aColor)
{
   return new UtScriptRef(new UtColor(aColor), sScriptColorClassPtr, UtScriptRef::cMANAGE);
}

// =================================================================================================
//! virtual
void* UtScriptColor::Create(const UtScriptContext& aInstance)
{
   return new UtColor();
}

// =================================================================================================
//! virtual
void* UtScriptColor::Clone(void* aObjectPtr)
{
   return new UtColor(*static_cast<UtColor*>(aObjectPtr));
}

// =================================================================================================
//! virtual
void UtScriptColor::Destroy(void* aObjectPtr)
{
   delete static_cast<UtColor*>(aObjectPtr);
}

bool UtScriptColor::EqualTo(void* aLHS, void* aRHS)
{
   return *static_cast<UtColor*>(aLHS) == *static_cast<UtColor*>(aRHS);
}