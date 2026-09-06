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

#include <osgEarth/Common>
#include <osgEarth/TileSource>
#include <osgEarth/URI>

namespace osgEarth
{
namespace Drivers
{

class DtedTmsOptions : public TileSourceOptions
{
public:
   osgEarth::optional<URI>&       url() { return _url; }
   const osgEarth::optional<URI>& url() const { return _url; }

   osgEarth::optional<std::string>&       tmsType() { return _tmsType; }
   const osgEarth::optional<std::string>& tmsType() const { return _tmsType; }

   osgEarth::optional<std::string>&       format() { return _format; }
   const osgEarth::optional<std::string>& format() const { return _format; }

   DtedTmsOptions(const TileSourceOptions& opt = TileSourceOptions{})
      : TileSourceOptions{opt}
   {
      setDriver("dted_tms");
      fromConfig(_conf);
   }

   DtedTmsOptions(const std::string& inUrl)
      : DtedTmsOptions{}
   {
      url() = inUrl;
   }

   ~DtedTmsOptions() override {}

   Config getConfig() const override
   {
      auto conf = TileSourceOptions::getConfig();
      conf.set("url", _url);
      conf.set("tms_type", _tmsType);
      conf.set("format", _format);
      return conf;
   }

protected:
   void mergeConfig(const Config& conf) override
   {
      TileSourceOptions::mergeConfig(conf);
      fromConfig(conf);
   }

private:
   void fromConfig(const Config& conf)
   {
      conf.get("url", _url);
      conf.get("format", _format);
      conf.get("tms_type", _tmsType);
   }

   osgEarth::optional<URI>         _url;
   osgEarth::optional<std::string> _tmsType;
   osgEarth::optional<std::string> _format;
};
} // namespace Drivers
} // namespace osgEarth
