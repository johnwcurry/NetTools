/*
 * CidrConvert.h
 *
 *  Created on: Oct 10, 2016
 *      Author: johnwcurry
 */

#pragma once
#include  <cstddef>
#include <cstdint>
#include <string>
class CidrConvert
{
public:
	CidrConvert();
	virtual ~CidrConvert();
	uint32_t CidrFromString(std::string netMask);
};


