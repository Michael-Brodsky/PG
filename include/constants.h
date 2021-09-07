/*
 *	This file defines a library of scientific and engineering numerical 
 *	constants.

 *****************************************************************************/

/*	File: constants.h
 *	Date: November 11, 2015
 *	Version: 0.99
 *	Author: Michael Brodsky
 *	Email: superhotmuffin@hotmail.com
 *	Copyright (c) 2012-2015 Michael Brodsky */

/*	***************************************************************************
 *
 *  This file is part of "Pretty Good" (Pg). "Pg" is free software: 
 *	you can redistribute it and/or modify it under the terms of the 
 *	GNU General Public License as published by the Free Software Foundation, 
 *	either version 3 of the License, or (at your option) any later version.
 *
 *  This file is distributed in the hope that it will be useful, but 
 *	WITHOUT ANY WARRANTY; without even the implied warranty of 
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *	GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License 
 *	along with this file. If not, see <http://www.gnu.org/licenses/>.
 *
 *	***************************************************************************
 *
 *	Description: 
 *
 *	This file defines a library of object-like macros, suitable for #if 
 *	preprocessing, that expand to common numerical constants of upto 21 
 *	decimal places.
 *
 *	***************************************************************************/

#ifndef CONSTANTS_H__
#define CONSTANTS_H__	20150726L

/*	Numeric Constants ********************************************************/

#define M_E				2.718281828459045235360	/* Euler's number (e)        */
#define M_BETA2			0.915965594177219015055	/* Catalan's constant (B[2]) */
#define M_ZETA3			1.202056903159594285400	/* Apery's constant (Z[3])   */
#define M_GAMMA			0.577215664901532860607	/* Euler-Mascheroni constant */
#define M_GAUSS			0.834626841674073186281	/* Gauss' constant (G)       */
#define M_OMEGA			0.567143290409783873000	/* Omega constant, x=exp(-x) */
#define M_K				1.380000000000000000000e-23 /* Boltzmann's constant  */
#define M_Q				1.600000000000000000000e-19 /* Electron charge       */

/*	Logarithmic Constants ****************************************************/

#define M_LN_01			-4.6051701859881		/* ln(0.01)                  */
#define M_LNPI			1.144729885849400174143	/* ln(pi)                    */
#define M_LN2			0.693147180559945309417	/* ln(2)                     */
#define M_LN10			2.302585092994045684018	/* ln(10)                    */
#define M_LOG2E			1.442695040888963407360	/* log2(e), 1/ln(2)          */
#define M_LOG10E		0.434294481903251827651	/* log10(e)                  */
#define M_LOG10PI		0.497149872694133854351	/* log10(pi)                 */
#define M_LOG10_2		0.301029995663981195214	/* log10(2)                  */

/*	Constants involving Euler's number (e) ***********************************/

#define M_1_E			0.367879441171442321596	/* 1/e                       */
#define M_EXP2			7.389056098930650227230	/* e^2                       */
#define M_1_EXP2		0.135335283236612691894	/* 1/e^2                     */

/*	Constants involving pi ***************************************************/

#define M_2PI			6.283185307179586476925	/* 2pi,	360 deg              */
#define M_PI			3.141592653589793238463	/* pi,	180 deg              */
#define M_PI_2			1.570796326794896619231	/* pi/2, 90 deg              */
#define M_PI_3			1.047197551196597746154	/* pi/3, 60 deg              */
#define M_PI_4			0.785398163397448309616	/* pi/4, 45 deg              */
#define M_PI_5			0.628318530717958647693	/* pi/5, 36 deg              */
#define M_PI_6			0.523598775598298873077	/* pi/6, 30 deg              */
#define M_PI_7			0.448798950512827605495	/* pi/7                      */
#define M_PI_8			0.392699081698724154808	/* pi/8, 22.5 deg            */
#define M_PI_9			0.349065850398865915385	/* pi/9, 20 deg              */
#define M_PI_10			0.314159265358979323846	/* pi/10, 18 deg             */
#define M_PI_12			0.261799387799149436539	/* pi/12, 15 deg             */
#define M_PI_15			0.209439510239319549231	/* pi/15, 12 deg             */
#define M_PI_16			0.196349540849362077404	/* pi/16, 11.25 deg          */
#define M_PI_18			0.174532925199432957692	/* pi/18, 10 deg             */
#define M_PI_20			0.157079632679489661923	/* pi/20, 9 deg              */
#define M_PI_24			0.130899693899574718269	/* pi/24, 7.5 deg            */
#define M_PI_30			0.104719755119659774615	/* pi/30, 6 deg              */
#define M_PI_32			0.098174770424681038702	/* pi/32, 5.625 deg          */
#define M_1_PI			0.318309886183790671538	/* 1/pi                      */
#define M_1_2PI			0.1591549430918953		/* 1/2pi                     */
#define M_2_PI			0.636619772367581343076 /* 2/pi                      */
#define	M_PI2			9.869604401089358618834	/* pi^2                      */

/*	Constants involving roots ************************************************/

#define M_SQRTE			1.648721270700128146849	/* sqrt(e)                   */
#define M_SQRTPI		1.772453850905516027298	/* sqrt(pi)                  */
#define M_SQRT2PI		2.506628274631000502416	/* sqrt(2pi)                 */
#define M_2SQRTPI_PI	1.128379167095512573896	/* 2*sqrt(pi)/pi             */
#define M_SQRT2			1.414213562373095048802	/* sqrt(2)                   */
#define M_SQRT2_2		0.707106781186547524401	/* sqrt(2)/2                 */
#define M_SQRT3			1.732050807568877293527	/* sqrt(3)                   */
#define M_SQRT3_2		0.866025403784438646764	/* sqrt(3)/2                 */
#define M_SQRT3_3		0.577350269189625764509	/* sqrt(3)/3                 */
#define M_SQRT5			2.236067977499789696409	/* sqrt(5)                   */
#define M_3ROOT2		1.259921049894873164767	/* 2^(1/3)                   */
#define M_12ROOT2		1.059463094359295264562	/* 2^(1/12)                  */

/*	Factorial Constants ******************************************************/

#define	M_FACT1			1						/* 1!                        */
#define	M_FACT2			2						/* 2!                        */
#define	M_FACT3			6						/* 3!                        */
#define	M_FACT4			24						/* 4!                        */
#define	M_FACT5			120						/* 5!                        */
#define	M_FACT6			720						/* 6!                        */
#define	M_FACT7			5040					/* 7!                        */
#define	M_FACT8			40320					/* 8!                        */
#define	M_FACT9			362880					/* 9!                        */
#define	M_FACT10		3628800					/* 10!                       */
#define	M_FACT11		39916800				/* 11!                       */
#define	M_FACT12		479001600				/* 12!                       */
#define	M_FACT13		6227020800				/* 13!                       */
#define	M_FACT14		87178291200				/* 14!                       */
#define	M_FACT15		1307674368000			/* 15!                       */
#define	M_FACT16		20922789888000			/* 16!                       */

/*	Power of 2 Constants *****************************************************/

#define M_POW2_64		18446744073709551616	/* 2^64                      */
#define M_POW2_32		4294967296				/* 2^32                      */
#define M_POW2_16		65536					/* 2^16                      */
#define M_POW2_15		32768					/* 2^15                      */
#define M_POW2_14		16384					/* 2^14                      */
#define M_POW2_13		8192					/* 2^13                      */
#define M_POW2_12		4096					/* 2^12                      */
#define M_POW2_11		2048					/* 2^11                      */
#define M_POW2_10		1024					/* 2^10                      */
#define M_POW2_9		512						/* 2^9                       */
#define M_POW2_8		256						/* 2^8                       */
#define M_POW2_7		128						/* 2^7                       */
#define M_POW2_6		64						/* 2^6                       */
#define M_POW2_5		32						/* 2^5                       */
#define M_POW2_4		16						/* 2^4                       */
#define M_POW2_3		8						/* 2^3                       */
#define M_POW2_2		4						/* 2^2                       */
#define M_POW2_1		2						/* 2^1                       */
#define M_POW2_0		1						/* 2^0                       */

#define M_1_POW2_0		1.0						/* 2^(-0)                    */
#define M_1_POW2_1		0.5						/* 2^(-1)                    */
#define M_1_POW2_2		0.25					/* 2^(-2)                    */
#define M_1_POW2_3		0.125					/* 2^(-3)                    */
#define M_1_POW2_4		0.0625					/* 2^(-4)                    */
#define M_1_POW2_5		0.03125					/* 2^(-5)                    */
#define M_1_POW2_6		0.015625				/* 2^(-6)                    */
#define M_1_POW2_7		0.0078125				/* 2^(-7)                    */
#define M_1_POW2_8		0.00390625				/* 2^(-8)                    */
#define M_1_POW2_9		0.001953125				/* 2^(-9)                    */
#define M_1_POW2_10		0.0009765625			/* 2^(-10)                   */
#define M_1_POW2_11		0.00048828125			/* 2^(-11)                   */
#define M_1_POW2_12		0.000244140625			/* 2^(-12)                   */
#define M_1_POW2_13		0.0001220703125			/* 2^(-13)                   */
#define M_1_POW2_14		0.00006103515625		/* 2^(-14)                   */
#define M_1_POW2_15		0.000030517578125		/* 2^(-15)                   */
#define M_1_POW2_16		0.0000152587890625		/* 2^(-16)                   */
#define M_1_POW2_17		0.00000762939453125		/* 2^(-17)                   */
#define M_1_POW2_18		0.000003814697265625	/* 2^(-18)                   */
#define M_1_POW2_19		0.0000019073486328125	/* 2^(-19)                   */
#define M_1_POW2_20		0.00000095367431640625	/* 2^(-20)                   */

/*	Triginometric Constants **************************************************/

#define M_SIN30			0.5						/* sin(30)                   */
#define M_COS30			M_SQRT3_2				/* cos(30)                   */
#define M_TAN30			M_SQRT3_3				/* tan(30)                   */
#define M_SIN45			M_SQRT2_2				/* sin(45)                   */
#define M_COS45			M_SQRT2_2				/* cos(45)                   */
#define M_TAN45			1.0						/* tan(45)                   */
#define M_SIN60			M_SQRT3_2				/* sin(60)                   */
#define M_COS60			0.5						/* cos(60)                   */
#define M_TAN60			M_SQRT3					/* tan(60)                   */

#define M_DEG_RAD		57.29577951308232087680	/* Degrees per radian        */

/* Constants involving decibel ratios ****************************************/

#define M_AP2			3.01029995663981195214	/* 2:1 power ratio (dB)      */
#define M_AV2			6.02059991327962390427	/* 2:1 amplitude ratio (dB)  */

#define M_DBA_PLUS_20	10.0					/* +20 dB (amplitude ratio)  */
#define M_DBA_PLUS_10	3.16227766016837933200	/* +10 dB (amplitude ratio)  */
#define M_DBA_PLUS_6	1.99526231496887960135	/* +6 dB (amplitude ratio)   */
#define M_DBA_PLUS_3	1.41253754462275430216	/* +3 dB (amplitude ratio)   */
#define M_DBA_PLUS_1	1.12201845430196343559	/* +1 dB (amplitude ratio)   */
#define M_DBA_MINUS_1	0.89125093813374552995	/* -1 dB (amplitude ratio)   */
#define M_DBA_MINUS_3	0.70794578438413791080	/* -3 dB (amplitude ratio)   */
#define M_DBA_MINUS_6	0.50118723362727228500	/* -6 dB (amplitude ratio)   */
#define M_DBA_MINUS_10	0.31622776601683793320	/* -10 dB (amplitude ratio)  */
#define M_DBA_MINUS_20	0.1						/* -20 dB (amplitude ratio)  */

#define M_DBP_PLUS_10	10.0					/* +10 dB (power ratio)      */
#define M_DBP_PLUS_6	3.98107170553497250770	/* +6 dB (power ratio)       */
#define M_DBP_PLUS_3	1.99526231496887960135	/* +3 dB (power ratio)       */
#define M_DBP_PLUS_1	1.25892541179416721042	/* +1 dB (power ratio)       */
#define M_DBP_MINUS_1	0.79432823472428150207	/* -1 dB (power ratio)       */
#define M_DBP_MINUS_3	0.50118723362727228500	/* -3 dB (power ratio)       */
#define M_DBP_MINUS_6	0.25118864315095801111	/* -6 dB (power ratio)       */
#define M_DBP_MINUS_10	0.1						/* -10 dB (power ratio)      */

/* Miscellaneous constants. **************************************************/

#define M_TIME_CONST	0.63212055882855767840 /* Time constant, i.e. 63.2%. */

#endif	/* !CONSTANTS_H__ */