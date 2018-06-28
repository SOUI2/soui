#include "stdafx.h"
#include "3dlib.h"

#include <math.h>

namespace IMAGE3D
{
	// storage for our lookup tables
	float cos_look[361] = {0.0}; // 1 extra element so we can store 0-360 inclusive
	float sin_look[361] = {0.0};


void Build_Sin_Cos_Tables(void)
{
	// create sin/cos lookup table
	// note the creation of one extra element; 360
	// this helps with logic in using the tables

	// generate the tables 0 - 360 inclusive
	for (int ang = 0; ang <= 360; ang++)
	{
		// convert ang to radians
		float theta = (float)ang*PI/(float)180;

		// insert next entry into table
		cos_look[ang] = cos(theta);
		sin_look[ang] = sin(theta);

	} // end for ang

} // end Build_Sin_Cos_Tables


float Fast_Sin(float theta)
{
	// this function uses the sin_look[] lookup table, but
	// has logic to handle negative angles as well as fractional
	// angles via interpolation, use this for a more robust
	// sin computation that the blind lookup, but with with
	// a slight hit in speed

	// convert angle to 0-359
	theta = fmodf(theta,360);

	// make angle positive
	if (theta < 0) theta+=360.0;

	// compute floor of theta and fractional part to interpolate
	int theta_int    = (int)theta;
	float theta_frac = theta - theta_int;

	// now compute the value of sin(angle) using the lookup tables
	// and interpolating the fractional part, note that if theta_int
	// is equal to 359 then theta_int+1=360, but this is fine since the
	// table was made with the entries 0-360 inclusive
	return(sin_look[theta_int] + 
		theta_frac*(sin_look[theta_int+1] - sin_look[theta_int]));

} // end Fast_Sin

///////////////////////////////////////////////////////////////

float Fast_Cos(float theta)
{
	// this function uses the cos_look[] lookup table, but
	// has logic to handle negative angles as well as fractional
	// angles via interpolation, use this for a more robust
	// cos computation that the blind lookup, but with with
	// a slight hit in speed

	// convert angle to 0-359
	theta = fmodf(theta,360);

	// make angle positive
	if (theta < 0) theta+=360.0;

	// compute floor of theta and fractional part to interpolate
	int theta_int    = (int)theta;
	float theta_frac = theta - theta_int;

	// now compute the value of sin(angle) using the lookup tables
	// and interpolating the fractional part, note that if theta_int
	// is equal to 359 then theta_int+1=360, but this is fine since the
	// table was made with the entries 0-360 inclusive
	return(cos_look[theta_int] + 
		theta_frac*(cos_look[theta_int+1] - cos_look[theta_int]));

} // end Fast_Cos

int Fast_Distance_2D(int x, int y)
{
	// this function computes the distance from 0,0 to x,y with 3.5% error

	// first compute the absolute value of x,y
	x = abs(x);
	y = abs(y);

	// compute the minimum of x,y
	int mn = MIN(x,y);

	// return the distance
	return(x+y-(mn>>1)-(mn>>2)+(mn>>4));

} // end Fast_Distance_2D

///////////////////////////////////////////////////////////////////////////////

float Fast_Distance_3D(float fx, float fy, float fz)
{
	// this function computes the distance from the origin to x,y,z

	int temp;  // used for swaping
	int x,y,z; // used for algorithm

	// make sure values are all positive
	x = (int)fabs(fx) * 1024;
	y = (int)fabs(fy) * 1024;
	z = (int)fabs(fz) * 1024;

	// sort values
	if (y < x) SWAP(x,y,temp)

		if (z < y) SWAP(y,z,temp)

			if (y < x) SWAP(x,y,temp)

				int dist = (z + 11 * (y >> 5) + (x >> 2) );

	// compute distance with 8% error
	return((float)(dist >> 10));

} // end Fast_Distance_3D

}