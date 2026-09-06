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

#ifndef GENERALFORMAT_HPP
#define GENERALFORMAT_HPP

// Qt
#include <QWidget>

#include "ReportWidget.hpp"

namespace Ui
{
class GeneralFormat;
}

namespace PostProcessor
{
// Format Options Widget
class GeneralFormat : public ReportWidget
{
      Q_OBJECT

   public:
      GeneralFormat();
      ~GeneralFormat() override;

      void WriteData(std::ostream& aStream) override;

      void SetDelimiter(QString aDelimiter);

   private:
      void RestoreDefaults() override;
      Ui::GeneralFormat* mUIPtr;

      QString mSelectedDelimiter;
      QString mSelectedLengthUnits;
      QString mSelectedAngleUnits;
      QString mSelectedNumFormat;
};
}
#endif
