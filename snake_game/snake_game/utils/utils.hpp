#pragma once

#include <stdlib.h>
#include <algorithm>


template<typename T>
extern inline T	random(T min = 0, T max = 0) 
{
	return (max - min + 1) * (rand() / (RAND_MAX + 1.0)) + min; 
}

template<typename T>
extern inline int rounded(T val) 
{ 
	int integral = (int)val; return (val - integral < 0.5) ? integral : integral + 1;
};

template<typename T>
extern inline void	clamp(T &val, T a, T b)
{
	if (val < a) {
		val = a;
		return;
	}
	
	if (val > b)
		val = b;
}

template<class Container, class Iterator>
extern Iterator	random_from(Container &v) 
{
	assert(v.size() > 0);

	const unsigned long divisor = (RAND_MAX + 1) / v.size();
	unsigned long k;
	do { k = rand() / divisor; } while (k >= v.size());

	Iterator iter = v.begin;
	std::advance(iter, k);

	return iter;
};


