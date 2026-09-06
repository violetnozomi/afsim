// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
/**
 *  @file DifferenceOfArrival.hpp
 *  @author John Gallagher, Booz | Allen | Hamilton \n
 *          Keith Sawmiller, Booz | Allen | Hamilton \n
 *          2241 Avionics Circle \n
 *          WPAFB, Ohio 45433-7303 \n
 *          937-656-4899 x3018 (Gallagher) \n
 *          937-656-4899 x3010 (Sawmiller) \n
 *  @brief Algorithms to find position from TDOA and Velocities from FDOA.\n
 *  Classification: UNCLASSIFIED
 */

#ifndef VCLMATH_DOA_HPP
#define VCLMATH_DOA_HPP 1

#include <vclMath/Constants.hpp>
#include <vclMath/Matrix.hpp>

/**
 *  \brief Namespace to encapsulate vclMath routines
 */
namespace vm
{
    /**
       @brief  Time Difference of Arrival: Returns the emitter location
       based on position and toa from 4 ground sensors. Sensors must be
       stationary.
       @param sensor1Pos: Matrix<double>(3,1) The position of the 1st sensor
       @param sensor2Pos: Matrix<double>(3,1) The position of the 2nd sensor
       @param sensor3Pos: Matrix<double>(3,1) The position of the 3rd sensor
       @param sensor4Pos: Matrix<double>(3,1) The position of the 4th sensor
       @parma sensor1TOA: Time of arrival at the 1st sensor in seconds
       @parma sensor2TOA: Time of arrival at the 2nd sensor in seconds
       @parma sensor3TOA: Time of arrival at the 3rd sensor in seconds
       @parma sensor4TOA: Time of arrival at the 4th sensor in seconds
       @return: Matrix<double>(3,1) The calculated emitter position
    */
    Matrix<double> CalculatePosFromTDOA(
        Matrix<double> sensor1Pos,
        Matrix<double> sensor2Pos,
        Matrix<double> sensor3Pos,
        Matrix<double> sensor4Pos,
        double sensor1TOA,
        double sensor2TOA,
        double sensor3TOA,
        double sensor4TOA
        )
    {
        // Create "A" Matrix ( 3x3 )
        vm::Matrix<double> matrixA(3,3);
        matrixA(1,1) = -2.0 * (sensor2Pos(1,1)-sensor1Pos(1,1));
        matrixA(1,2) = -2.0 * (sensor2Pos(2,1)-sensor1Pos(2,1));
        matrixA(1,3) = -2.0 * (sensor2Pos(3,1)-sensor1Pos(3,1));
        matrixA(2,1) = -2.0 * (sensor3Pos(1,1)-sensor1Pos(1,1));
        matrixA(2,2) = -2.0 * (sensor3Pos(2,1)-sensor1Pos(2,1));
        matrixA(2,3) = -2.0 * (sensor3Pos(3,1)-sensor1Pos(3,1));
        matrixA(3,1) = -2.0 * (sensor4Pos(1,1)-sensor1Pos(1,1));
        matrixA(3,2) = -2.0 * (sensor4Pos(2,1)-sensor1Pos(2,1));
        matrixA(3,3) = -2.0 * (sensor4Pos(3,1)-sensor1Pos(3,1));
        matrixA = matrixA.inverse();

        // Create "B" Matrix ( 3x1 )
        vm::Matrix<double> matrixB(3,1);
        matrixB(1,1) =
            std::pow( sensor2Pos(3,1)-sensor1Pos(3,1), 2 ) +
            std::pow( sensor2Pos(2,1)-sensor1Pos(2,1), 2 ) +
            std::pow( sensor2Pos(1,1)-sensor1Pos(1,1), 2 );
        matrixB(2,1) =
            std::pow( sensor3Pos(3,1)-sensor1Pos(3,1), 2 ) +
            std::pow( sensor3Pos(2,1)-sensor1Pos(2,1), 2 ) +
            std::pow( sensor3Pos(1,1)-sensor1Pos(1,1), 2 );
        matrixB(3,1) =
            std::pow( sensor4Pos(3,1)-sensor1Pos(3,1), 2 ) +
            std::pow( sensor4Pos(2,1)-sensor1Pos(2,1), 2 ) +
            std::pow( sensor4Pos(1,1)-sensor1Pos(1,1), 2 );

        // Calc the "D" Matrix ( 3x1 )
        // D[1] = c^2 * (TOA2^2 - TOA1^2)
        // D[2] = c^2 * (TOA3^2 - TOA1^2)
        // D[3] = c^2 * (TOA4^2 - TOA1^2)
        vm::Matrix<double> matrixD(1,3);
        matrixD(1,1) =
            std::pow(vm::V_LIGHT,2) *
            ( std::pow( sensor2TOA, 2 ) -
              std::pow( sensor1TOA, 2 ) );
        matrixD(1,2) =
            std::pow(vm::V_LIGHT,2) *
            ( std::pow( sensor3TOA, 2 ) -
              std::pow( sensor1TOA, 2 ) );
        matrixD(1,3) =
            std::pow(vm::V_LIGHT,2.0) *
            ( std::pow( sensor4TOA, 2 ) -
              std::pow( sensor1TOA, 2 ) );
        matrixD = matrixD.transpose();

        vm::Matrix<double> matrixDB(3,1);
        matrixDB = matrixD-matrixB;

        vm::Matrix<double> matrixSol(3,1);
        matrixSol = matrixA * matrixDB;
        matrixSol += sensor1Pos;

        return matrixSol;
    }

    /**
       @brief Frequency Difference of Arrival: Returns the emitter velocities
       based on position and foa from 4 ground sensors. Sensors must be
       stationary.
       @param emitterPos: Matrix<double>(3,1) The position of the emitter
       @param sensor1Pos: Matrix<double>(3,1) The position of the 1st sensor
       @param sensor2Pos: Matrix<double>(3,1) The position of the 2nd sensor
       @param sensor3Pos: Matrix<double>(3,1) The position of the 3rd sensor
       @param sensor4Pos: Matrix<double>(3,1) The position of the 4th sensor
       @parma sensor1FOA: Freq of arrival at the 1st sensor in seconds
       @parma sensor2FOA: Freq of arrival at the 2nd sensor in seconds
       @parma sensor3FOA: Freq of arrival at the 3rd sensor in seconds
       @parma sensor4FOA: Freq of arrival at the 4th sensor in seconds
       @return: Matrix<double>(3,1) The calculated emitter velocity
    */
    Matrix<double> CalculateVelFromFDOA(
        Matrix<double> emitterPos,
        Matrix<double> sensor1Pos,
        Matrix<double> sensor2Pos,
        Matrix<double> sensor3Pos,
        Matrix<double> sensor4Pos,
        double sensor1FOA,
        double sensor2FOA,
        double sensor3FOA,
        double sensor4FOA
        )
    {
        Matrix<double> matrixE1(3,1);
        matrixE1 = emitterPos-sensor1Pos;
        matrixE1 = matrixE1 / sqrt((matrixE1 * matrixE1.transpose())(1,1));
        Matrix<double> matrixE2(3,1);
        matrixE2 = emitterPos-sensor2Pos;
        matrixE2 = matrixE2 / sqrt((matrixE2 * matrixE2.transpose())(1,1));
        Matrix<double> matrixE3(3,1);
        matrixE3 = emitterPos-sensor3Pos;
        matrixE3 = matrixE3 / sqrt((matrixE3 * matrixE3.transpose())(1,1));
        Matrix<double> matrixE4(3,1);
        matrixE4 = emitterPos-sensor4Pos;
        matrixE4 = matrixE4 / sqrt((matrixE4 * matrixE4.transpose())(1,1));

        // Create the X, Y, and Z matrices
        Matrix<double> X(1,4);
        X(1,1) = matrixE1(1,1);
        X(1,2) = matrixE2(1,1);
        X(1,3) = matrixE3(1,1);
        X(1,4) = matrixE4(1,1);
        Matrix<double> Y(1,4);
        Y(1,1) = matrixE1(2,1);
        Y(1,2) = matrixE2(2,1);
        Y(1,3) = matrixE3(2,1);
        Y(1,4) = matrixE4(2,1);
        Matrix<double> Z(1,4);
        Z(1,1) = matrixE1(3,1);
        Z(1,2) = matrixE2(3,1);
        Z(1,3) = matrixE3(3,1);
        Z(1,4) = matrixE4(3,1);

        Matrix<double> aRetVal(3,1);

        // This is going to get ugly...Good luck trying to decipher this.
        // Calculate Vx
        aRetVal(1,1) =
            -( ( Y(1,1)*(
                     Z(1,3)-Z(1,2))-
                 Y(1,2)*Z(1,3)+Z(1,2)*Y(1,3)+Z(1,1)*(
                     Y(1,2)-Y(1,3))
                   )*
               vm::V_LIGHT*sensor4FOA+vm::V_LIGHT*
               (
                   Y(1,4)*(
                       Z(1,1)*(sensor3FOA-sensor2FOA)-Z(1,2)*sensor3FOA+
                       Z(1,3)*sensor2FOA)+
                   Y(1,1)*(Z(1,2)*sensor3FOA-Z(1,3)*sensor2FOA)+
                   Z(1,4)*(Y(1,2)*sensor3FOA+Y(1,1)*(sensor2FOA-sensor3FOA)-
                           Y(1,3)*sensor2FOA)+
                   Z(1,1)*(Y(1,3)*sensor2FOA-Y(1,2)*sensor3FOA))+
               ((Y(1,3)-Y(1,2))*
                Z(1,4)+(Z(1,2)-Z(1,3))*Y(1,4)+Y(1,2)*
                Z(1,3)-Z(1,2)*Y(1,3))*vm::V_LIGHT*sensor1FOA)
            /
            ( ( X(1,1)*( Z(1,2)*Y(1,3)-Y(1,2)*Z(1,3) )+
                Y(1,1)*( X(1,2)*Z(1,3)-Z(1,2)*X(1,3) )+
                Z(1,1)*( Y(1,2)*X(1,3)-X(1,2)*Y(1,3) ) )*sensor4FOA+X(1,4)*
              ( Y(1,1)*(Z(1,2)*sensor3FOA-Z(1,3)*sensor2FOA)+
                Z(1,1)*(Y(1,3)*sensor2FOA-Y(1,2)*sensor3FOA) ) +
              Y(1,4)*(X(1,1)*(Z(1,3)*sensor2FOA-Z(1,2)*sensor3FOA)+
                      Z(1,1)*(X(1,2)*sensor3FOA-X(1,3)*sensor2FOA))+
              Z(1,4)*(X(1,1)*(Y(1,2)*sensor3FOA-Y(1,3)*sensor2FOA)+
                      Y(1,1)*(X(1,3)*sensor2FOA-X(1,2)*sensor3FOA))+
              ((X(1,2)*Y(1,3)-Y(1,2)*X(1,3))*
               Z(1,4)+(Z(1,2)*X(1,3)-X(1,2)*Z(1,3))*
               Y(1,4)+(Y(1,2)*Z(1,3)-Z(1,2)*Y(1,3))*
               X(1,4))*sensor1FOA);

        // Calculate Vy
        aRetVal(2,1) =
            ( ( X(1,1)*( Z(1,3)-Z(1,2) )-X(1,2)*Z(1,3)+Z(1,2)*X(1,3)+
                Z(1,1)*( X(1,2)-X(1,3) ) )*vm::V_LIGHT*sensor4FOA+
              vm::V_LIGHT*( X(1,4)*(
                                Z(1,1)*(sensor3FOA-sensor2FOA)-
                                Z(1,2)*sensor3FOA+Z(1,3)*sensor2FOA)+
                            X(1,1)*( Z(1,2)*sensor3FOA-Z(1,3)*sensor2FOA)+
                            Z(1,4)*( X(1,2)*sensor3FOA+X(1,1)*
                                     (sensor2FOA-sensor3FOA)-X(1,3)*sensor2FOA)+
                            Z(1,1)*( X(1,3)*sensor2FOA-X(1,2)*sensor3FOA) )+
              ( (X(1,3)-X(1,2))*Z(1,4)+(Z(1,2)-Z(1,3))*
                X(1,4)+X(1,2)*Z(1,3)-Z(1,2)*X(1,3))*vm::V_LIGHT*sensor1FOA )
            /
            ( ( X(1,1)*( Z(1,2)*Y(1,3)-Y(1,2)*Z(1,3) )+
                Y(1,1)*( X(1,2)*Z(1,3)-Z(1,2)*X(1,3) )+
                Z(1,1)*( Y(1,2)*X(1,3)-X(1,2)*Y(1,3) ) )*sensor4FOA+
              X(1,4)*( Y(1,1)*( Z(1,2)*sensor3FOA-Z(1,3)*sensor2FOA )+
                       Z(1,1)*( Y(1,3)*sensor2FOA-Y(1,2)*sensor3FOA ) )+
              Y(1,4)*( X(1,1)*( Z(1,3)*sensor2FOA-Z(1,2)*sensor3FOA )+
                       Z(1,1)*( X(1,2)*sensor3FOA-X(1,3)*sensor2FOA ) )+
              Z(1,4)*( X(1,1)*( Y(1,2)*sensor3FOA-Y(1,3)*sensor2FOA )+
                       Y(1,1)*( X(1,3)*sensor2FOA-X(1,2)*sensor3FOA ) )+
              ( ( X(1,2)*Y(1,3)-Y(1,2)*X(1,3) )*
                Z(1,4)+( Z(1,2)*X(1,3)-X(1,2)*Z(1,3) )*
                Y(1,4)+( Y(1,2)*Z(1,3)-Z(1,2)*Y(1,3) )*
                X(1,4) )*sensor1FOA );

        aRetVal(3,1) =
            -( ( X(1,1)*(Y(1,3)-Y(1,2))-X(1,2)*Y(1,3)+Y(1,2)*X(1,3)+
                 Y(1,1)*(X(1,2)-X(1,3)) )*vm::V_LIGHT*sensor4FOA+
               vm::V_LIGHT*( X(1,4)*( Y(1,1)*(sensor3FOA-sensor2FOA)-
                                      Y(1,2)*sensor3FOA+Y(1,3)*sensor2FOA)+
                             X(1,1)*( Y(1,2)*sensor3FOA-Y(1,3)*sensor2FOA)+
                             Y(1,4)*( X(1,2)*sensor3FOA+X(1,1)*
                                      (sensor2FOA-sensor3FOA)-
                                      X(1,3)*sensor2FOA)+
                             Y(1,1)*( X(1,3)*sensor2FOA-X(1,2)*sensor3FOA) )+
               ( (X(1,3)-X(1,2))*
                 Y(1,4)+(Y(1,2)-Y(1,3))*X(1,4)+X(1,2)*Y(1,3)-Y(1,2)*X(1,3) )*
               vm::V_LIGHT*sensor1FOA)
            /
            ( ( X(1,1)*( Z(1,2)*Y(1,3)-Y(1,2)*Z(1,3) )+
                Y(1,1)*( X(1,2)*Z(1,3)-Z(1,2)*X(1,3) )+
                Z(1,1)*( Y(1,2)*X(1,3)-X(1,2)*Y(1,3) ) )*sensor4FOA+
              X(1,4)*( Y(1,1)*( Z(1,2)*sensor3FOA-Z(1,3)*sensor2FOA )+
                       Z(1,1)*( Y(1,3)*sensor2FOA-Y(1,2)*sensor3FOA ) )+
              Y(1,4)*( X(1,1)*( Z(1,3)*sensor2FOA-Z(1,2)*sensor3FOA )+
                       Z(1,1)*( X(1,2)*sensor3FOA-X(1,3)*sensor2FOA ) )+
              Z(1,4)*( X(1,1)*( Y(1,2)*sensor3FOA-Y(1,3)*sensor2FOA )+
                       Y(1,1)*( X(1,3)*sensor2FOA-X(1,2)*sensor3FOA ) )+
              ( ( X(1,2)*Y(1,3)-Y(1,2)*X(1,3) )*
                Z(1,4)+( Z(1,2)*X(1,3)-X(1,2)*Z(1,3) )*
                Y(1,4)+( Y(1,2)*Z(1,3)-Z(1,2)*Y(1,3) )*
                X(1,4))*sensor1FOA);

        return aRetVal;
    }
}

#endif // VCLMATH_DOA_HPP
