#include "Plist.hpp"
#include <algorithm>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

static void createMessage(map<string, std::any>& dict)
{
	Plist::Date date;
	date.setTimeFromAppleEpoch(338610664);

	dict["testDate"] = date;
	dict["testInt"] = int(-3455);
	dict["testInt32"] = int32_t(-3455);
	dict["testInt64"] = int64_t(-3455);
	dict["testShort"] = short(3455);
	dict["testLong"] = long(-3455);
	dict["testDouble"] = 1.34223;
	dict["testFloat"] = 1.34223f;
	dict["testBoolTrue"] = true;
	dict["testBoolFalse"] = false;
	std::ifstream stream("./testImage.png", std::ios::binary);
	if(!stream)
		throw std::runtime_error("Can't open file: testImage.png");

	int start = stream.tellg();
	stream.seekg(0, std::ifstream::end);
	int size = ((int)stream.tellg()) - start;
	std::vector<char> actualData(size);
	if(size > 0)
	{
		stream.seekg(0, std::ifstream::beg);
		stream.read((char*)&actualData[0], size);
	}
	else
	{
		throw std::runtime_error("Can't read zero length data");
	}

	dict["testImage"] = actualData;
	vector<std::any> array(2);
	array[0] = 34;
	array[1] = string("string item in array");
	dict["testArray"] = array;
	dict["testString"] = string("hello there");
	dict["testEmptyString"] = string("");

	map<string, std::any> innerDict;
	innerDict["test string"] = string("inner dict item");
	dict["testDict"] = innerDict;

	innerDict.clear();
	array.resize(256);

	for(int i = 0; i < 256; ++i)
	{
		stringstream ss;
		if(i < 10)
			ss << "00";
		if((i >= 10) && (i < 100))
			ss << "0";
		ss << i;
		array[i] = i;
		innerDict[ss.str()] = i;
	}
	dict["testArrayLarge"] = array;
	dict["testDictLarge"] = innerDict;
}

bool vectorsEqual(const std::vector<char>& a, const std::vector<char>& b)
{
	const auto& res = std::mismatch(a.begin(), a.end(), b.begin(), b.end());

	return res.first == a.end() && res.second == b.end();
}

static void checkDictionary(const map<string, std::any>& dict)
{
	string actualString = "hello there";
	string actualEmptyString = "";
	double actualDouble = 1.34223;
	int actualInt = -3455;

	// checking byte array
	std::ifstream stream("./testImage.png", std::ios_base::in | std::ios::binary);
	if(!stream.is_open())
	{
		throw std::runtime_error("Can't open file: testImage.png");
	}

	int start = stream.tellg();
	stream.seekg(0, std::ifstream::end);
	int size = ((int)stream.tellg()) - start;
	std::vector<char> actualData(size);
	if(size > 0)
	{
		stream.seekg(0, std::ifstream::beg);
		stream.read((char*)&actualData[0], size);
	}
	else
	{
		throw std::runtime_error("Can't read zero length data");
	}

	const vector<char>& plistData = std::any_cast<const vector<char>&>(dict.find("testImage")->second);

	// length
	REQUIRE(actualData.size() == plistData.size());

	REQUIRE(vectorsEqual(actualData, plistData));

	REQUIRE_THAT(actualDouble,
	             Catch::Matchers::WithinRel(std::any_cast<const double&>(dict.find("testDouble")->second), 1E-5));

	REQUIRE_THAT(actualDouble,
	             Catch::Matchers::WithinRel(std::any_cast<const double&>(dict.find("testFloat")->second), 1E-5));

	REQUIRE(actualInt == std::any_cast<const int64_t&>(dict.find("testInt")->second));

	REQUIRE(actualString == std::any_cast<const string&>(dict.find("testString")->second));

	REQUIRE(actualEmptyString == std::any_cast<const string&>(dict.find("testEmptyString")->second));

	const vector<std::any>& plistArray = std::any_cast<const vector<std::any>&>(dict.find("testArray")->second);
	int actualArrayItem0 = 34;
	string actualArrayItem1 = "string item in array";
	REQUIRE(actualArrayItem0 == std::any_cast<const int64_t&>(plistArray[0]));
	REQUIRE(actualArrayItem1 == std::any_cast<const string&>(plistArray[1]).c_str());

	// checking long array (need to do this because there is different logic if
	// the length of the array is >= 15 elements
	const vector<std::any>& plistArrayLarge =
	std::any_cast<const vector<std::any>&>(dict.find("testArrayLarge")->second);
	int i = 0;
	for(vector<std::any>::const_iterator it = plistArrayLarge.begin(); i < 256; ++it, ++i)
	{
		REQUIRE(i == std::any_cast<const int64_t&>(*it));
	}

	// checking long dict (need to do this because there is different logic if the length
	// of the dict is >= 15 elements
	const map<string, std::any>& plistDictLarge =
	std::any_cast<const map<string, std::any>&>(dict.find("testDictLarge")->second);
	i = 0;
	for(map<string, std::any>::const_iterator it = plistDictLarge.begin(); i < 256; ++it, ++i)
	{
		REQUIRE(i == std::any_cast<const int64_t&>(it->second));
	}

	int actualDate = 338610664;
	REQUIRE(actualDate ==
	        static_cast<int>(std::any_cast<const Plist::Date&>(dict.find("testDate")->second).timeAsAppleEpoch()));

	REQUIRE(std::any_cast<const bool&>(dict.find("testBoolTrue")->second));
	REQUIRE_FALSE(std::any_cast<const bool&>(dict.find("testBoolFalse")->second));
}

TEST_CASE("Read xml", "")
{
	map<string, std::any> dict;
	Plist::readPlist("XMLExample1.plist", dict);
	checkDictionary(dict);
}

TEST_CASE("Read binary")
{
	map<string, std::any> dict;
	Plist::readPlist("binaryExample1.plist", dict);

	checkDictionary(dict);
}
TEST_CASE("Write binary")
{
	map<string, std::any> dict;
	createMessage(dict);
	Plist::writePlistBinary("binaryExampleWritten.plist", dict);
	dict.clear();
	Plist::readPlist("binaryExampleWritten.plist", dict);
	checkDictionary(dict);
}

TEST_CASE("Write XML")
{
	map<string, std::any> dict;
	createMessage(dict);
	Plist::writePlistXML("xmlExampleWritten.plist", dict);
	dict.clear();
	Plist::readPlist("xmlExampleWritten.plist", dict);
	checkDictionary(dict);
}

TEST_CASE("Write binary to byte array")
{
	vector<char> data;
	map<string, std::any> dict;
	createMessage(dict);
	Plist::writePlistBinary(data, dict);
	map<string, std::any> dictCheck;
	Plist::readPlist(&data[0], data.size(), dictCheck);
	checkDictionary(dictCheck);
}

TEST_CASE("Write XML to byte array")
{
	vector<char> data;
	map<string, std::any> dict;
	createMessage(dict);
	Plist::writePlistXML(data, dict);
	map<string, std::any> dictCheck;
	Plist::readPlist(&data[0], data.size(), dictCheck);
	checkDictionary(dictCheck);
}

TEST_CASE("Date")
{
	Plist::Date date;

	// check comparisons.

	double objectTime = date.timeAsAppleEpoch();

	Plist::Date dateGreater(date);
	dateGreater.setTimeFromAppleEpoch(objectTime + 1);
	Plist::Date dateLess(date);
	dateLess.setTimeFromAppleEpoch(objectTime - 1);

	REQUIRE(1 == Plist::Date::compare(dateGreater, dateLess));
	REQUIRE(-1 == Plist::Date::compare(dateLess, dateGreater));
	REQUIRE(0 == Plist::Date::compare(date, date));

	CHECK(dateGreater > dateLess);
	CHECK(dateLess < dateGreater);
	CHECK(date == date);

	dateGreater.setTimeFromAppleEpoch(objectTime + 100);

	time_t seconds = dateGreater.secondsSinceDate(date);

	REQUIRE(100 == seconds);
}
