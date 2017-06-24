#pragma once

#include <stdlib.h>
#include <sstream>
#include <string>
#include <ostream>
#include <assert.h>
#include <vector>

using namespace std;

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

	Iterator it = v.begin;
	advance(it, k);

	return it;
};

template<class I>
extern inline string to_string(I val) 
{
	ostringstream buffer;
	buffer << val;
	return buffer.str();
};

template<typename T>
extern ostream& operator<<(ostream &os, const vector<T> v) 
{
	for (T i : v) {
		os << i << " ";
	}

	return os;
};

extern vector<string> ToStringList(string s) 
{
	vector<string> s_list;

	for (int i = 0, j = 0; i < s.size(); i++) {
		if (s[i] != ' ') {
			if (!s_list.size()) {
				s_list.push_back(string());
			}
			s_list[s_list.size() - 1] += s[i];
		}
		else {
			if (s_list.size()) {
				s_list.push_back(string());
			}
		}
	}

	return s_list;
}

extern int *Shuffle(int &size)
{
	int* randSequence = new int[size];

	for (int i = 0; i < size; i++) {
		randSequence[i] = i;
	}

	for (int i = 0; i < size; i++) {
		int randomIndex = (int)(i + (rand() % ((size - 1) - i + 1)));
		int temp = randSequence[randomIndex];
		randSequence[randomIndex] = randSequence[i];
		randSequence[i] = temp;
	}

	return randSequence;
}


