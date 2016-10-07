// NumberString.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "main.hpp"


int main()
{
	
	NumberString<NSBase::bin> C, D;
	C = "10001011101101010010101";
	D = "1000010000011100011";
	cout << C + D + 0b10011 << endl;


	NumberString<NSBase::dec> A, B;
	A = "12344444446785767442345234544445345000000003453453435616549864560234523450345";
	B = "45666666234545656898666666345634345345575634456345662456345753";
	cout << A + B - 2 << endl;

	NumberString<NSBase::dec> E, F;
	E = "1000000000000000000000000000000000000000000000000000000000000000";
	F = "615168";
	cout << E - F << endl;

	cout << _NS10(-381038192830248912430912312341223423421434123449) \
		  + _NS10(2341234123432532141243123412341) << endl;

	cout << _NS10(3241232412341234134634000075805786753452322126455) \
		  * _NS10(-123234123412412343453344541233) << endl;

	cout << _NS10(322222) - _NS10(322222) << endl;

	
	cout << _NS10(644445) % 322222 << endl;

	getchar();
    return 0;
}