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

#ifndef NETWORKS_HPP
#define NETWORKS_HPP

// Qt
#include <QWidget>

#include "ReportWidget.hpp"

namespace Ui
{
class Networks;
}

namespace PostProcessor
{
// Networks Widget for Communication Report
class Networks : public ReportWidget
{
      Q_OBJECT
   public:
      Networks();
      ~Networks() override;

      void WriteData(std::ostream& aStream) override;
      bool CheckValidity() override;

   private:
      void RestoreDefaults() override;
      Ui::Networks* mUIPtr;
};
}
#endif // !NETWORKS_HPP

