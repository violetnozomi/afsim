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

#ifndef OPTIONSWE_HPP
#define OPTIONSWE_HPP

#include <QWidget>

#include "OptionWidget.hpp"
#include "ProxyInterface.hpp"

namespace Ui
{
class OptionsWE;
}

namespace PostProcessor
{
class OptionsWE : public OptionWidget
{
      Q_OBJECT
   public:
      OptionsWE();
      ~OptionsWE() override;

      void WriteData(std::ostream& aStream) override;
      bool CheckValidity() override;
      void AddWeapons(const QVector<ProxyInterface::PlatformPart>& aWeapons);
      void RemoveWeapons(const QVector<ProxyInterface::PlatformPart>& aWeapons);

   private:
      Ui::OptionsWE* mUIPtr;

};
}
#endif // !OPTIONSWE_HPP
