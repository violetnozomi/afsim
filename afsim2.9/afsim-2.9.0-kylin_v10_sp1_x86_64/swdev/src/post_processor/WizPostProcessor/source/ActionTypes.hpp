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

#ifndef ACTIONTYPES_HPP
#define ACTIONTYPES_HPP

// Qt
#include <QWidget>

#include "ReportWidget.hpp"

#include "ui_ActionTypes.h"

// Action Types Widget for Engagement Report
namespace PostProcessor
{
class ActionTypes : public ReportWidget
{
      Q_OBJECT
   public:
      ActionTypes();
      ~ActionTypes() override = default;

      void WriteData(std::ostream& aStream) override;
      bool CheckValidity() override;

   private:
      void RestoreDefaults() override;
      Ui::ActionTypes mUI;
};
}
#endif
