#include <cstdlib>

#ifndef Arrays_H
#define Arrays_H


//  these functions can be included in base class as protected to be used by derived class OR as public
//  atindex has to be same as provided in first argument [0][0] which is default
//  or by pointer arithmatic passing p+3, p+4 etc
//  else example [1][2] has to be mentioned in atindex by passing 1*hmax+2 value
//  upto variable is default 0 means Do ALL ELEMENTS FROM reference passed

	int reset_array(char *ptr, size_t size, unsigned int upto=0, char resetwith='#');
	int reset_array(int *ptr, size_t size, unsigned int upto=0, int resetwith=0);
	int reset_array(unsigned int *ptr, size_t size, unsigned int upto=0, unsigned int resetwith=0);
	int reset_array(short int *ptr, size_t size, unsigned int upto=0, short int resetwith=0);
	int reset_array(unsigned short int *ptr, size_t size, unsigned int upto=0, unsigned short int resetwith=0);
	
	int show_array(const char *ptr, size_t size, unsigned int hmax=0, unsigned int upto=0, unsigned int atindex=0);
	int show_array(const int *ptr, size_t size, unsigned int hmax=0, unsigned int upto=0, unsigned int atindex=0);
	int show_array(const unsigned int *ptr, size_t size, unsigned int hmax=0, unsigned int upto=0, unsigned int atindex=0);
	int show_array(const short int *ptr, size_t size, unsigned int hmax=0, unsigned int upto=0, unsigned int atindex=0);
	int show_array(const unsigned short int *ptr, size_t size, unsigned int hmax=0, unsigned int upto=0, unsigned int atindex=0);

#endif
