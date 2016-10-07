#pragma once

#include <algorithm>
#include <string>
#include <deque>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <bitset>

using namespace std;
typedef __int32 int32;
typedef unsigned __int32 uint32;
typedef __int64 int64;
typedef unsigned __int64 uint64;

enum NSSign :char { plus = '+', minus = '-' }; //符号
inline NSSign operator*(NSSign lhs, NSSign rhs)
{
	if (lhs == rhs)return NSSign::plus;
	else return NSSign::minus;
}
enum NSBase:size_t { bin = 2, oct = 8, dec = 10, hex = 16 }; //进制
const char CommonCharSet[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
enum NSCompareResult :char { equ = 1, greater, less }; //当为类函数时，大于小于是以调用对象为左值，被调用者为右值来说的

template<NSBase base>
struct NumberString_SpecificSettings
{
public:
	static const uint32 UnitMax; //单元最大值
	static const size_t UnitStringLen; //单元所对应字符串的长度
private:
	NumberString_SpecificSettings();//不允许实例化此类
};

const uint32 NumberString_SpecificSettings<NSBase::bin>::UnitMax = (uint32)(-1);
const uint32 NumberString_SpecificSettings<NSBase::oct>::UnitMax = 07777777777;
const uint32 NumberString_SpecificSettings<NSBase::dec>::UnitMax = 999999999;
const uint32 NumberString_SpecificSettings<NSBase::hex>::UnitMax = (uint32)(-1);

const size_t NumberString_SpecificSettings<NSBase::bin>::UnitStringLen = 32;
const size_t NumberString_SpecificSettings<NSBase::oct>::UnitStringLen = 10;
const size_t NumberString_SpecificSettings<NSBase::dec>::UnitStringLen = 9;
const size_t NumberString_SpecificSettings<NSBase::hex>::UnitStringLen = 8;

template<NSBase base>
uint32 UnitStringToInt(const string& String); //对于一个单元的数字与字符串的转换
template<> uint32 UnitStringToInt<NSBase::dec>(const string& String)
{
	stringstream Stream(String);
	uint32 Int = 0;
	Stream << std::dec;
	Stream >> Int;
	return Int;
}
template<> uint32 UnitStringToInt<NSBase::oct>(const string& String)
{
	stringstream Stream(String);
	uint32 Int = 0;
	Stream << std::oct;
	Stream >> Int;
	return Int;
}
template<> uint32 UnitStringToInt<NSBase::hex>(const string& String)
{
	stringstream Stream(String);
	uint32 Int = 0;
	Stream << std::hex;
	Stream >> Int;
	return Int;
}
template<> uint32 UnitStringToInt<NSBase::bin>(const string& String)
{
	bitset<32> Stream(String);
	uint32 Int = Stream.to_ulong();
	return Int;
}

template<NSBase base>
string UnitIntToString(const uint32& Int); //对于一个单元的数字与字符串的转换
template<> string UnitIntToString<NSBase::dec>(const uint32& Int)
{
	stringstream Stream;
	Stream << std::dec << Int;
	return Stream.str();
}
template<> string UnitIntToString<NSBase::oct>(const uint32& Int)
{
	stringstream Stream;
	Stream << Int; Stream << std::oct << Int; 
	return Stream.str();
}
template<> string UnitIntToString<NSBase::hex>(const uint32& Int)
{
	stringstream Stream;
	Stream << std::hex << Int;
	return Stream.str();
}
template<> string UnitIntToString<NSBase::bin>(const uint32& Int)
{
	bitset<32> Stream(Int);
	string tmpString = Stream.to_string();
	return tmpString.substr(tmpString.find('1'));
}

#define NSBase_static_assert() \
static_assert(base == NSBase::bin || base == NSBase::oct || base == NSBase::dec || base == NSBase::hex,\
 "NumberString:Construct with wrong base!")

template<NSBase base> //base为进制基数
class NumberString //目前仅需完成高精度整数的通用流设计
{
private:
	typedef NumberString_SpecificSettings<base> SpecificSettings;
	typedef deque<uint32>::iterator iterator;

	NSSign sign;
	deque<uint32> number;

	void upShift(size_t bit) //在此数的最低位补uint32(0)
	{
		for (size_t i = 0; i < bit; ++i) number.emplace_front(0);
	}
	void trim() //将此数所有最高位0去除
	{
		while (number.size() > 1 && number.back() == 0)
		{
			number.pop_back();
		}
	}


public:
	NumberString() :sign(NSSign::plus), number(1, (uint32)(0))  //值为0的数，而非NaN
	{
		NSBase_static_assert();
	}
	NumberString(const NumberString& source):sign(source.sign), number(source.number)
	{}
	NumberString(const NumberString&& source) :sign(source.sign), number(source.number)
	{}
	NumberString(uint64 Number, NSSign Sign = NSSign::plus) : sign(Sign)
	{
		NSBase_static_assert();
		*this = Number;
	}
	NumberString(const string& Number)
	{
		NSBase_static_assert();
		*this = Number;
	}
	

	NumberString& operator=(const NumberString& source)
	{
		sign = source.sign;
		number = source.number;
		return *this;
	}
	NumberString& operator=(const NumberString&& source)
	{
		sign = source.sign;
		number = source.number;
		return *this;
	}
	NumberString& operator=(uint64 Number)
	{
		number.clear();
		uint64 tmpUnit = Number;
		while (tmpUnit > uint64(SpecificSettings::UnitMax))
		{
			number.push_back(tmpUnit % (uint64(SpecificSettings::UnitMax) + 1));
			tmpUnit /= uint64(SpecificSettings::UnitMax) + 1;
		}
		number.push_back(tmpUnit);

		return *this;
	}
	NumberString& operator=(const string& Number)
	{
		number.clear();

		bool existSign = false;
		size_t StringLength = 0;
		size_t iterator = Number.size();


		if (!Number.empty())
		{
			if (*Number.begin() == NSSign::plus || *Number.begin() == NSSign::minus)
			{
				existSign = true;
				sign = (NSSign)*Number.begin();
			}
			else sign = NSSign::plus;
			while ((existSign ? iterator - 1 : iterator) > SpecificSettings::UnitStringLen)
			{
				iterator -= SpecificSettings::UnitStringLen;
				number.push_back(UnitStringToInt<base>(Number.substr(iterator, SpecificSettings::UnitStringLen)));
			}
			if (existSign)
				number.push_back(UnitStringToInt<base>(Number.substr(1, iterator - 1)));
			else number.push_back(UnitStringToInt<base>(Number.substr(0, iterator)));
		}
		return *this;
	}

	NumberString operator+()const
	{
		return *this;
	}
	NumberString operator-()const
	{
		NumberString copy(*this);
		copy.reverseSign();
		return copy;
	}
	NumberString operator++() //前缀自加
	{
		return *this += 1;
	}
	NumberString operator--() //前缀自减
	{
		return *this -= 1;
	}
	NumberString operator++(int) //后缀自加,不推荐
	{
		NumberString temp = *this;
		*this += 1;
		return temp;
	}
	NumberString operator--(int) //后缀自减,不推荐
	{
		NumberString temp = *this;
		*this -= 1;
		return temp;
	}
	NumberString operator+(const NumberString& another) const
	{
		if (sign == another.sign)
		{
			NumberString result;
			bool isCarryBit = false; //是否进位
			if (number.size() >= another.number.size()) 
			{
				result = *this;
				for (size_t i = 0; i < result.number.size(); ++i)
				{
					if (i < another.number.size())
					{
						result.number[i] += (isCarryBit ? 1 : 0) + another.number[i];
						isCarryBit = result.number[i] > SpecificSettings::UnitMax || result.number[i] < another.number[i];
						if (isCarryBit) result.number[i] -= SpecificSettings::UnitMax + 1;
					}
					else 
					{ 
						result.number[i] += (isCarryBit ? 1 : 0);
						isCarryBit = result.number[i] > SpecificSettings::UnitMax;
						if (isCarryBit) result.number[i] -= SpecificSettings::UnitMax + 1;
						else break;
					}
				}
				if (isCarryBit)result.number.push_back(1);
			}
			else
			{
				result = another;
				for (size_t i = 0; i < result.number.size(); ++i)
				{
					if (i < number.size())
					{
						result.number[i] += (isCarryBit ? 1 : 0) + number[i];
						isCarryBit = result.number[i] > SpecificSettings::UnitMax || result.number[i] < number[i];
						if (isCarryBit) result.number[i] -= SpecificSettings::UnitMax + 1;
					}
					else
					{
						result.number[i] += (isCarryBit ? 1 : 0);
						isCarryBit = result.number[i] > SpecificSettings::UnitMax;
						if (isCarryBit) result.number[i] -= SpecificSettings::UnitMax + 1;
						else break;
					}
				}
				if (isCarryBit)result.number.push_back(1);
			}
			return result;
		}
		else return *this - (-another);
	}
	NumberString operator-(const NumberString& another) const 
	{
		if (sign == another.sign)
		{
			NumberString result;
			bool isBorrowBit = false; //是否借位

			switch (absCompare(another))
			{
			case NSCompareResult::equ:
				//result = 0;
				break;
			case NSCompareResult::greater:
				result = *this;
				for (size_t i = 0; i < result.number.size(); ++i)
				{
					if (i < another.number.size())
					{
						if (result.number[i] > another.number[i])
						{
							result.number[i] -= another.number[i] + (isBorrowBit ? 1 : 0);
							isBorrowBit = false;
						}
						else
						{
							result.number[i] = SpecificSettings::UnitMax + 1 - (another.number[i] - result.number[i]) - (isBorrowBit ? 1 : 0);
							isBorrowBit = true;
						}
					}
					else 
					{
						if (isBorrowBit)
						{
							if (result.number[i] > 0)
							{
								result.number[i] -= 1;
								isBorrowBit = false;
							}
							else
							{
								result.number[i] = SpecificSettings::UnitMax;
								isBorrowBit = true;
							}
						}
						else
						{
							break;
						}
					}
				}
				break;
			case NSCompareResult::less:
				result = another;
				result.reverseSign();
				for (size_t i = 0; i < result.number.size(); ++i)
				{
					if (i < number.size())
					{
						if (result.number[i] > number[i])
						{
							result.number[i] -= number[i] + (isBorrowBit ? 1 : 0);
							isBorrowBit = false;
						}
						else
						{
							result.number[i] = SpecificSettings::UnitMax + 1 - (number[i] - result.number[i]) - (isBorrowBit ? 1 : 0);
							isBorrowBit = true;
						}
					}
					else
					{
						if (isBorrowBit)
						{
							if (result.number[i] > 0)
							{
								result.number[i] -= 1;
								isBorrowBit = false;
							}
							else
							{
								result.number[i] = SpecificSettings::UnitMax;
								isBorrowBit = true;
							}
						}
						else
						{
							break;
						}
					}
				}
				break;
			}
			result.trim();
			return result;
		}
		else return *this + (-another);
	}
	NumberString operator*(const NumberString& another) const 
	{
		NumberString result;
		NumberString temp = 0;
		deque<NumberString> additions;
		

		if (number.size() < another.number.size())
		{
			additions.resize(number.size());
			for (size_t i = 0; i < number.size(); ++i)
			{
				for (size_t j = 0; j < another.number.size(); ++j)
				{
					temp = int64(number[i])*int64(another.number[j]);
					temp.upShift(i + j);
					additions[i] += temp;
				}
			}
		}
		else
		{
			additions.resize(another.number.size());
			for (size_t i = 0; i < another.number.size(); ++i)
			{
				for (size_t j = 0; j < number.size(); ++j)
				{
					temp = int64(another.number[i])*int64(number[j]);
					temp.upShift(i + j);
					additions[i] += temp;
				}
			}
		}
		for (size_t i = 0; i < additions.size(); ++i)
		{
			result += additions[i];
		}

		result.sign = sign*another.sign;

		return result;
	}
	NumberString operator/(const NumberString& another) const 
	{
		//本方法系他人代码改写，（我不会告诉你我要玩游戏没时间写），以后重写
		
		uint32 norm = (SpecificSettings::UnitMax + 1) / (another.number.back() + 1);
		NumberString X = (*this)*norm;
		X.sign = NSSign::plus;
		NumberString Y = another*norm;
		X.sign = NSSign::plus;

		NumberString quotient = 0,remainder = 0;
		quotient.number.resize(X.number.size());
		for (int64 i = X.number.size() - 1; i >= 0; --i)
		{
			remainder.upShift(1);
			remainder += X.number[i];
			uint32 s1 = remainder.number.size() <= Y.number.size() ? 0 : remainder.number[Y.number.size()];
			uint32 s2 = remainder.number.size() <= Y.number.size() - 1 ? 0 : remainder.number[Y.number.size() - 1];
			uint32 d = (((uint64)SpecificSettings::UnitMax + 1)*s1 + s2) / Y.number.back();
			remainder -= Y*d;
			while (remainder < 0)
			{
				remainder += Y, --d;
			}
			quotient.number[i] = d;
		}
		quotient.sign = sign*another.sign;
		//remainder.sign = sign;
		quotient.trim();
		//remainder.trim();
		//remainder = remainder / norm;

		return quotient;
	}
	NumberString operator%(const NumberString& another) const 
	{
		//本方法系他人代码改写，（我不会告诉你我要玩游戏没时间写），以后重写

		uint32 norm = (SpecificSettings::UnitMax + 1) / (another.number.back() + 1);
		NumberString X = (*this)*norm;
		X.sign = NSSign::plus;
		NumberString Y = another*norm;
		X.sign = NSSign::plus;

		NumberString quotient = 0, remainder = 0;
		quotient.number.resize(X.number.size());
		for (int64 i = X.number.size() - 1; i >= 0; --i)
		{
			remainder.upShift(1);
			remainder += X.number[i];
			uint32 s1 = remainder.number.size() <= Y.number.size() ? 0 : remainder.number[Y.number.size()];
			uint32 s2 = remainder.number.size() <= Y.number.size() - 1 ? 0 : remainder.number[Y.number.size() - 1];
			uint32 d = (((uint64)SpecificSettings::UnitMax + 1)*s1 + s2) / Y.number.back();
			remainder -= Y*d;
			while (remainder < 0)
			{
				remainder += Y, --d;
			}
			quotient.number[i] = d;
		}
		//quotient.sign = sign*another.sign;
		remainder.sign = sign;
		//quotient.trim();
		remainder.trim();
		//remainder = remainder / norm;

		return remainder / norm;
	}
	
	NumberString operator+=(const NumberString& another)
	{
		return *this = *this + another;
	}
	NumberString operator-=(const NumberString& another)
	{
		return *this = *this - another;
	}
	NumberString operator*=(const NumberString& another)
	{
		return *this = *this * another;
	}
	NumberString operator/=(const NumberString& another)
	{
		return *this = *this / another;
	}
	NumberString operator%=(const NumberString& another)
	{
		return *this = *this % another;
	}

	NumberString operator+(int64 another) const 
	{
		return *this + NumberString(abs(another), (another >= 0) ? NSSign::plus : NSSign::minus);
	}
	NumberString operator-(int64 another) const
	{
		return *this - NumberString(abs(another), (another >= 0) ? NSSign::plus : NSSign::minus);
	}
	NumberString operator*(int64 another) const
	{
		return *this * NumberString(abs(another), (another >= 0) ? NSSign::plus : NSSign::minus);
	}
	NumberString operator/(int64 another) const
	{
		//本方法系他人代码改写，（我不会告诉你我要玩游戏没时间写），以后重写

		NumberString result;
		result.number.resize(number.size());
		result.sign = sign;
		if (another < 0) 
		{ 
			result.reverseSign();
			another = -another; 
		}
		for (int64 i = number.size() - 1, rem = 0; i >= 0; --i) 
		{
			int64 a = number[i] + rem*(int64)(SpecificSettings::UnitMax + 1);
			result.number[i] = (int32)(a / another);
			rem = a%another;
		}
		result.trim();
		return result;
	}
	int64 operator%(int64 another) const
	{
		//本方法系他人代码改写，（我不会告诉你我要玩游戏没时间写），以后重写

		if (another<0) another = -another;
		int result = 0;
		for (int64 i = number.size() - 1; i >= 0; --i)
			result = (number[i] + result*(int64)(SpecificSettings::UnitMax + 1)) % another;
		return result*((sign == NSSign::plus) ? 1 : -1);
	}

	bool operator==(const NumberString& another) const
	{
		if ((sign == another.sign && absCompare(another) == NSCompareResult::equ) || (isZero() && another.isZero()))
			return true;
		else return false;
	}
	bool operator<(const NumberString& another) const
	{
		if (sign != another.sign)
		{
			if (isZero() && another.isZero()) return false;
			else if (sign == '+') return false;
			else return true;
		}
		else //sign == another.sign
		{
			if (sign == '+') 
			{
				if (absCompare(another) == NSCompareResult::less) return true;
				else return false;
			}
			else
			{
				if (absCompare(another) == NSCompareResult::less) return false;
				else return true;
			}
		}
	}
	bool operator>(const NumberString& another) const
	{
		if (sign != another.sign)
		{
			if (isZero() && another.isZero()) return false;
			else if (sign == '+') return true;
			else return false;
		}
		else //sign == another.sign
		{
			if (sign == '+')
			{
				if (absCompare(another) == NSCompareResult::greater) return true;
				else return false;
			}
			else
			{
				if (absCompare(another) == NSCompareResult::greater) return false;
				else return true;
			}
		}
	}
	bool operator<=(const NumberString& another) const
	{
		return !(*this > another);
	}
	bool operator>=(const NumberString& another) const
	{
		return !(*this < another);
	}

	friend inline istream& operator>>(istream &stream, NumberString &Number)
	{
		string tmpString;
		stream >> tmpString;
		Number = tmpString;
		return stream;
	}
	friend inline ostream& operator<<(ostream &stream,const NumberString &Number)
	{
		stream << Number.toString();

		return stream;
	}

	string toString() const
	{
		stringstream stream;

		if (isNaN())
			stream << "NaN";
		else if (isZero())
			stream << 0;
		else
		{
			if (sign == NSSign::minus)
				stream << (char)sign;
			stream << UnitIntToString<base>(number.back());
			if (number.size() > 1) 
			{
				for (size_t i = number.size() - 2; i != 0; --i)
				{
					stream << setw(SpecificSettings::UnitStringLen) << setfill('0') << UnitIntToString<base>(number[i]);
				}
				stream << setw(SpecificSettings::UnitStringLen) << setfill('0') << UnitIntToString<base>(number[0]);
			}
		}

		return stream.str();
	}
	inline NSSign getSign() const 
	{
		return sign;
	}
	inline void setSign(NSSign inSign)
	{
		sign = inSign;
	}
	inline void reverseSign()
	{
		if (sign == NSSign::plus)
			sign = NSSign::minus;
		else sign = NSSign::plus;
	}
	inline bool isZero() const
	{
		if (number.size() == 1 && number[0] == 0)
			return true;
		else return false;
	}
	inline bool isNaN() const //NaN,Not a Number
	{
		if (number.empty())return true;
		else return false;
	}
	inline size_t getSize() const
	{
		return number.size();
	}

	NSCompareResult absCompare(const NumberString& another) const
	{
		if (number.size() > another.number.size())
			return NSCompareResult::greater;
		else if (number.size() < another.number.size())
			return NSCompareResult::less;
		else //number.size() == another.number.size()
		{
			for (int32 i = number.size() - 1; i >= 0; --i)
			{
				if (number[i] > another.number[i])
					return NSCompareResult::greater;
				else if (number[i] < another.number[i])
					return NSCompareResult::less;
			}
			return NSCompareResult::equ;
		}
	}
	static bool divmod(const NumberString& dividend, const NumberString& divisor, NumberString& quotient/*商*/, NumberString& remainder/*余数*/)//返回是否可得到结果
	{
		//本方法系他人代码改写，（我不会告诉你我要玩游戏没时间写），以后重写

		uint32 norm = (SpecificSettings::UnitMax + 1) / (divisor.number.back() + 1);
		NumberString X = dividend*norm;
		X.sign = NSSign::plus;
		NumberString Y = divisor*norm;
		X.sign = NSSign::plus;

		quotient = 0; remainder = 0;
		quotient.number.resize(X.number.size());
		for (int64 i = X.number.size() - 1; i >= 0; --i)
		{
			remainder.upShift(1);
			remainder += X.number[i];
			uint32 s1 = remainder.number.size() <= Y.number.size() ? 0 : remainder.number[Y.number.size()];
			uint32 s2 = remainder.number.size() <= Y.number.size() - 1 ? 0 : remainder.number[Y.number.size() - 1];
			uint32 d = (((uint64)SpecificSettings::UnitMax + 1)*s1 + s2) / Y.number.back();
			remainder -= Y*d;
			while (remainder < 0)
			{
				remainder += Y, --d;
			}
			quotient.number[i] = d;
		}
		quotient.sign = dividend.sign*divisor.sign;
		remainder.sign = dividend.sign;
		quotient.trim();
		remainder.trim();
		remainder = remainder / norm;

		return true;
	}

	~NumberString()
	{}
};

#undef NSBase_static_assert()

#define _NS2(str) NumberString<NSBase::bin>(#str)
#define _NS8(str) NumberString<NSBase::oct>(#str)
#define _NS10(str) NumberString<NSBase::dec>(#str)
#define _NS16(str) NumberString<NSBase::hex>(#str)
