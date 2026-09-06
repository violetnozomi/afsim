// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef VCLMATH_RECFILTER_HPP
#define VCLMATH_RECFILTER_HPP 1

#include "vclMath/Matrix.hpp"
#include <iostream>
/*!
 \class  recFilter
 \brief  n dimension 2d Order Recursive Least Sqares Filter Class\n

         Provides filtered estimates for postion, velocity, and acceleration  \n
         using input meaurements. \n

	 Reference:  Zarchan and Musoff, Fundamentals of Kalman Filtering:\n
		     A Practical Approach, 2d Edition, pp 112-115 \n

 \author Randall Wantz, Booz Allen Hamilton, 2008 \n
         Modeling, Simulation, Wargaming, and Analysis

**  Classification: UNCLASSIFIED
*/
namespace vm {

class recFilter {

private:

	double k;					/*! 	Iteration number 		*/
	double Ts;					/*!	Current time (sec)		*/
	double Tlast;					/*! 	last update time (sec)		*/
	Matrix<double> Hats;				/*!	Defualt matrix of estimates	*/
	unsigned int dims;				/*!	dimensions (e.g. x,y,z)		*/
	unsigned int order;				/*!	estimator order (max 2)		*/

public:

/*!
	\brief Construct an object of type recfilter
	\param d -- Number of Dimensions
*/
	recFilter(unsigned int d, unsigned int ord) {

        	if(ord < 0 || ord > 2){
                	std::cout << "recFilter Error: Order must be either 0, 1, or 2!\n";
                	exit(-1);
        	}
        	dims = d;
        	Hats.resize(d,4);                       // 4 colums for Hat/dotHat/dotdotHat/Res
        	k = 0;
        	Ts = 0.0;
        	Tlast = 0.0;
        	this->order = ord;
	}

/*!
	\brief Destructor
*/
  	~recFilter() { }

/*!
	\brief Update current filter estimates
	\param meas 	-- Vector of current measurements
	\param timex	-- Current time in seconds
*/
	void update(Matrix<double> meas, double timex) {
		k++;								// Update Iterator
		Ts = timex - Tlast;						// Setup up time & last time
		Tlast = timex;

		double Bot;				// core of all gain denominators
		double K1;				// compute gains
		double K2;
		double K3;

		if(this->order == 2) {
			//Bot = ((k*k*k)+(3*(k*k))+(2*k));	// core of all gain denominators
			Bot = this->k * (this->k + 1.0) * (this->k + 2.0);
			K1 = (9.0*this->k*this->k - 9.0*this->k + 6.0)/Bot;		// compute gains
			K2 = (36.0*this->k - 18.0)/(Bot*Ts);
			K3 = 60.0/(Bot*(Ts*Ts));
		}
		else if(this->order == 1) {
			K1 = 2.0 * (2.0 * this->k - 1.0) / ( this->k * (this->k + 1) );
			K2 = 6.0 / ( this->k * (this-> k + 1.0) * Ts);
			K3 = 0.0;
		}
		else {
			/* Oth order filter, no velocity expected*/
			K1 = 1.0 / this->k;
			K2 = 0.0;
			K3 = 0.0;
		}

		//  Compute Residual and estimates (Hat variables) for each dimension

		for (unsigned int i = 1; i <= dims; i++) {
			double Res = meas(1,i) - Hats(i,1) - Hats(i,2)*Ts - 0.5*Hats(i,3)*Ts*Ts;
			double Hat = Hats(i,1) + Hats(i,2)*Ts + 0.5*Hats(i,3)*Ts*Ts + K1*Res;
			double dotHat = Hats(i,2) + Hats(i,3)*Ts + K2*Res;
			double dot2Hat = Hats(i,3) + K3*Res;

			Hats(i,1) = Hat;		// copy temp estimates to matrix
			Hats(i,2) = dotHat;
			Hats(i,3) = dot2Hat;
			Hats(i,4) = Res;
		}

	} // update

/*!
        \brief Get current position estimates
        \return Matrix column containing position estimates for each dimension
*/
	Matrix<double> getHat() 	{ return Hats.getColumn(1); }

/*!
        \brief Get current velocity estimates
        \return Matrix column containing velocity estimates for each dimension
*/
	Matrix<double> getdotHat() 	{ return Hats.getColumn(2); }

/*!
        \brief Get current acceleration estimates
        \return Matrix column containing acceleration estimates for each dimension
*/
	Matrix<double> getdotdotHat() 	{ return Hats.getColumn(3); }

/*!
        \brief Get current residuals
        \return Matrix column containing residuals for each dimension
*/
	Matrix<double> getRes() 	{ return Hats.getColumn(4); }

};  // class recFilter

}  // namespace
#endif
