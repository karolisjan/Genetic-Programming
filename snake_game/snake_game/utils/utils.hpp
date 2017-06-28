#pragma once

#include <stdlib.h>

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

template<class Iterator>
Iterator random_from(Iterator begin, Iterator end)
{
	int size = end - begin;

	unsigned long divisor = (RAND_MAX + 1) / size, k = divisor;

	do { k = rand() / divisor; }
	while (k >= size);

	return begin + k;
};


