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
#include "DtedTmsTileSource.hpp"

#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>
#include <osgEarth/FileUtils>
#include <osgEarth/ImageUtils>

namespace osgEarth
{
namespace Drivers
{
namespace TileMapService
{

namespace
{
const auto LC = "[DtedTmsTileSource] ";
}

DtedTmsTileSource::DtedTmsTileSource(const TileSourceOptions& options)
   : TileSource(options)
   , _options(options)
{
}

osgEarth::Status DtedTmsTileSource::initialize(const osgDB::Options* dbOptions)
{
   _dbOptions = Registry::instance()->cloneOrCreateOptions(dbOptions);

   auto uri = _options.url().value();
   if (uri.empty())
      return Status::Error("Fail: DTED TMS driver requires a valid \"url\" property");

   _tileMap = osgEarth::Util::TMS::TileMapReaderWriter::read(uri.full(), _dbOptions.get());
   if (!_tileMap.valid())
      return Status::Error(Stringify{} << "Failed to read tilemap from " << uri.full());

   OE_INFO << LC << "DTED TMS tile map datestamp = " << DateTime{_tileMap->getTimeStamp()}.asRFC1123() << std::endl;

   auto profile = _tileMap->createProfile();
   setProfile(profile);

   if (!profile)
      return Status::Error(Stringify{} << "Failed to establish a profile for " << uri.full());

   if (_tileMap->getTileSets().size() > 0)
   {
      OE_DEBUG << LC << "TileMap min/max " << _tileMap->getMinLevel() << " / " << _tileMap->getMaxLevel() << std::endl;

      if (_tileMap->getDataExtents().size() > 0)
      {
         auto& tileMapDataExtents = _tileMap->getDataExtents();
         auto& dataExtents        = getDataExtents();
         dataExtents.insert(dataExtents.end(), tileMapDataExtents.begin(), tileMapDataExtents.end());
      }
      else
      {
         getDataExtents().push_back(DataExtent{profile->getExtent(), 0, _tileMap->getMaxLevel()});
      }
   }

   return STATUS_OK;
}

osg::Image* DtedTmsTileSource::createImage(const TileKey& key, ProgressCallback* progress)
{
   if (_tileMap.valid() && key.getLevelOfDetail() <= _tileMap->getMaxLevel())
   {
      auto image_url = _tileMap->getURL(key, false);

      auto image = osg::ref_ptr<osg::Image>{};
      if (!image_url.empty())
         image = URI{image_url}.readImage(_dbOptions.get(), progress).getImage();

      if (!image.valid())
      {
         if (image_url.empty() || !_tileMap->intersectsKey(key))
         {
            if (key.getLevelOfDetail() <= _tileMap->getMaxLevel())
            {
               OE_DEBUG << LC << "Returning empty image" << std::endl;
               return ImageUtils::createEmptyImage();
            }
         }
      }

      if (image.valid())
      {
         image->setInternalTextureFormat(GL_R32F);
         ImageUtils::markAsUnNormalized(image, true);
      }

      return image.release();
   }

   return nullptr;
}

osgEarth::CachePolicy DtedTmsTileSource::getCachePolicyHint(const Profile* targetProfile) const
{
   if (targetProfile && targetProfile->isEquivalentTo(getProfile()))
      return CachePolicy::NO_CACHE;

   return CachePolicy::DEFAULT;
}

std::string DtedTmsTileSource::getExtension() const
{
   return _tileMap->getFormat().getExtension();
}

osgEarth::TimeStamp DtedTmsTileSource::getLastModifiedTime() const
{
   if (_tileMap.valid())
      return _tileMap->getTimeStamp();

   return TileSource::getLastModifiedTime();
}

int DtedTmsTileSource::getPixelsPerTile() const
{
   return _tileMap->getFormat().getWidth();
}

} // namespace TileMapService
} // namespace Drivers
} // namespace osgEarth
