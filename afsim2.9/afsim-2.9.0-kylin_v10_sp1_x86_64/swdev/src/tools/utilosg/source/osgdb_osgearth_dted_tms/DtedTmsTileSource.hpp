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
#pragma once

#include <osg/Notify>
#include <osgEarth/Registry>
#include <osgEarth/TileSource>
#include <osgEarthUtil/TMS>

#include "DtedTmsOptions.hpp"

namespace osgEarth
{
namespace Drivers
{
namespace TileMapService
{
class DtedTmsTileSource : public TileSource
{
public:
   explicit DtedTmsTileSource(const TileSourceOptions& options);

public:
   osgEarth::Status initialize(const osgDB::Options* dbOptions) override;

   osg::Image* createImage(const TileKey& key, ProgressCallback* progress) override;

   CachePolicy         getCachePolicyHint(const Profile* targetProfile) const override;
   virtual std::string getExtension() const;
   TimeStamp           getLastModifiedTime() const override;
   int                 getPixelsPerTile() const override;

private:
   osg::ref_ptr<osgEarth::Util::TMS::TileMap> _tileMap;
   osg::ref_ptr<osgDB::Options>               _dbOptions;
   const DtedTmsOptions                       _options;
};
} // namespace TileMapService
} // namespace Drivers
} // namespace osgEarth
