// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include <QDoubleValidator>

// QDoubleValidator allows for values outside the top/bottom of valid numbers to be entered and will return an
// intermediate state back.
//  This occurs even when the notation is set to StandardNotation.
//  The issue if for a validator with range [0,90] the number 95 can be entered which returns Intermediate.  Instead the
//  desired functionality is to return Invalid when 95 is entered.
class UtQtDoubleValidator : public QDoubleValidator
{
public:
   UtQtDoubleValidator(double bottom, double top, int decimals, QObject* parent)
      : QDoubleValidator(bottom, top, decimals, parent)
   {
   }

   QValidator::State validate(QString& s, int& i) const
   {
      if (s.isEmpty() || s == "-" || s == ".")
      {
         return QValidator::Intermediate;
      }

      QChar decimalPoint = locale().decimalPoint();
      if (s.indexOf(decimalPoint) != -1)
      {
         int charsAfterPoint = s.length() - s.indexOf(decimalPoint) - 1;

         if (charsAfterPoint > decimals())
         {
            return QValidator::Invalid;
         }
      }

      bool   ok;
      double d = locale().toDouble(s, &ok);
      if (ok && d >= bottom() && d <= top())
      {
         return QValidator::Acceptable;
      }
      else
      {
         return QValidator::Invalid;
      }
   }
};