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

#ifndef REPORTWIDGET_HPP
#define REPORTWIDGET_HPP

#include <QWidget>
#include <QXmlStreamWriter>

#include "UtCallbackHolder.hpp"

namespace PostProcessor
{
// base class for Widgets in the Report Dialog
class ReportWidget : public QWidget
{
      Q_OBJECT
   public:
      ReportWidget()
         : QWidget()
         , mIsOption(false)
      {
      }
      ~ReportWidget() override = default;
      bool IsOptionWidget() { return mIsOption; }

      virtual void WriteData(std::ostream& aStream) {}
      virtual bool CheckValidity() { return true; }
      virtual void RestoreDefaults() {}

   protected:
      bool             mIsOption;
      UtCallbackHolder mUtCallbacks;

};
}
#endif

