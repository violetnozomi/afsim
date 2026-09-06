// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include <osg/Notify>
#include <osgDB/FileNameUtils>
#include <osgDB/Registry>
#include <osgEarth/Registry>
#include <osgEarth/TileSource>
#include <osgEarthUtil/TMS>

#include "DtedTmsExports.hpp"
#include "DtedTmsOptions.hpp"
#include "DtedTmsTileSource.hpp"

namespace osgEarth
{
namespace Drivers
{
namespace TileMapService
{
class DTED_TMS_EXPORT DtedTmsDriver : public TileSourceDriver
{
private:
   using TileMapCache = std::map<std::string, osg::ref_ptr<osgEarth::Util::TMS::TileMap>>;
   TileMapCache _tileMapCache;

public:
   DtedTmsDriver() { supportsExtension("osgearth_dted_tms", "DTED Tile Map Service Driver"); }

   virtual const char* className() { return "DTED Tile Map Service Driver"; }

   ReadResult readObject(const std::string& file_name, const Options* options) const override
   {
      if (!acceptsExtension(osgDB::getLowerCaseFileExtension(file_name)))
         return ReadResult::FILE_NOT_HANDLED;

      return new DtedTmsTileSource{getTileSourceOptions(options)};
   }
};

REGISTER_OSGPLUGIN(osgearth_dted_tms, DtedTmsDriver)
} // namespace TileMapService
} // namespace Drivers
} // namespace osgEarth
