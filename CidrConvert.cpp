/*
 * CidrConvert.cpp
 *
 *  Created on: Oct 10, 2016
 *      Author: johnwcurry
 */


#include "CidrConvert.h"
#include <stdio.h>
#include <sys/types.h>
/*
1     	128.0.0.0  			80 00 00 00    	127.255.255.255      	1000 0000 0000 0000 0000 0000 0000 0000
2		192.0.0.0  			C0 00 00 00   	63.255.255.255      	1100 0000 0000 0000 0000 0000 0000 0000
3      	224.0.0.0  			E0 00 00 00    	31.255.255.255       	1110 0000 0000 0000 0000 0000 0000 0000
4      	240.0.0.0  			F0 00 00 00    	15.255.255.255       	1111 0000 0000 0000 0000 0000 0000 0000
5      	248.0.0.0  			F8 00 00 00    	7.255.255.255	      	1111 1000 0000 0000 0000 0000 0000 0000
6      	252.0.0.0  			FC 00 00 00   	3.255.255.255       	1111 1100 0000 0000 0000 0000 0000 0000
7      	254.0.0.0  			FE 00 00 00    	1.255.255.255       	1111 1110 0000 0000 0000 0000 0000 0000
8      	255.0.0.0  			FF 00 00 00    	0.255.255.255       	1111 1111 0000 0000 0000 0000 0000 0000
9      	255.128.0.0      	FF 80 00 00     0.127.255.255       	1111 1111 1000 0000 0000 0000 0000 0000
*/
CidrConvert::CidrConvert()
{
	// TODO Auto-generated constructor stub

}

CidrConvert::~CidrConvert()
{
	// TODO Auto-generated destructor stub
}
uint32_t CidrConvert::CidrFromString(std::string netMask)
{
	uint32_t vals[4];
	uint32_t cidrValue = 0;
	int iCount = sscanf(netMask.c_str(), "%u.%u.%u.%u",&vals[3],&vals[2],&vals[1],&vals[0]);
	bool moreData = true;
	for (int x = 3; x >=0; x--)
	{
		switch (vals[x])
		{
		case 0x80:
			cidrValue += 1;
			break;
		case 0xC0:
			cidrValue += 2;
			break;
		case 0xE0:
			cidrValue += 3;
			break;
		case 0xF0:
			cidrValue += 4;
			break;
		case 0xF8:
			cidrValue += 5;
			break;
		case 0xFC:
			cidrValue += 6;
			break;
		case 0xFE:
			cidrValue += 7;
			break;
		case 0xFF:
			cidrValue += 8;
			break;
		}
		if (vals[x] != 0xFF)
			break;
	}
	return cidrValue;
}

