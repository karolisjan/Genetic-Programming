#pragma once

#include <Windows.h>
#include <stdio.h>
#include <regex>
#include <string>
#include <fstream>

using namespace std;

class iniReader
{
public:
	iniReader(string filename) 
	{
		ifstream configFile(filename, ios::in);
		if (!configFile.is_open()) throw runtime_error("Config.ini was not found!");

		lpFilename = new wchar_t[filename.length() + 1];
		copy(filename.begin(), filename.end(), lpFilename);
		lpFilename[filename.length()] = '\0';
	}

	~iniReader() { delete lpFilename; };

	template<typename I> I Read(string section, string key)
	{
		wstring returnedString = GetValue(section, key);

		if (returnedString == lpDefault) {
			string errMsg = section + '.' + key + " was not found!";
			throw runtime_error(errMsg);
		}
		else {
			if (regex_match(string(returnedString.begin(), returnedString.end()), real)) {
				try {
					return stoi(returnedString);
				}
				catch (exception &e) {
					string errMsg = section + '.' + key + " holds an invalid value!";
					throw runtime_error(errMsg);
				}
			}
			else {
				string errMsg = section + '.' + key + " holds an invalid value!";
				throw runtime_error(errMsg);
			}
		}
	}

	template<> double Read<double>(string section, string key)
	{
		wstring returnedString = GetValue(section, key);

		if (returnedString == lpDefault) {
			string errMsg = section + '.' + key + " was not found!";
			throw runtime_error(errMsg);
		}
		else {
			if (regex_match(string(returnedString.begin(), returnedString.end()), real)) {
				try {
					return stod(returnedString);
				}
				catch (exception &e) {
					string errMsg = section + '.' + key + " holds an invalid value!";
					throw runtime_error(errMsg);
				}
			}
			else {
				string errMsg = section + '.' + key + " holds an invalid value!";
				throw runtime_error(errMsg);
			}
		}
	}

	template<> float Read<float>(string section, string key)
	{
		wstring returnedString = GetValue(section, key);

		if (returnedString == lpDefault) {
			string errMsg = section + '.' + key + " was not found!";
			throw runtime_error(errMsg);
		}
		else {
			if (regex_match(string(returnedString.begin(), returnedString.end()), real)) {
				try {
					return static_cast<float>(stod(returnedString));
				}
				catch (exception &e) {
					string errMsg = section + '.' + key + " holds an invalid value!";
					throw runtime_error(errMsg);
				}
			}
			else {
				string errMsg = section + '.' + key + " holds an invalid value!";
				throw runtime_error(errMsg);
			}
		}
	}

	template<> bool Read<bool>(string section, string key)
	{
		wstring temp = GetValue(section, key);
		string returnedString(temp.begin(), temp.end());

		if (temp == lpDefault) {
			string errMsg = section + '.' + key + " was not found!";
			throw runtime_error(errMsg);
		}
		else {
			if (returnedString == "true" || returnedString == "1") return true;
			else if (returnedString == "false" || returnedString == "0") return false;
			else {
				string errMsg = section + '.' + key + " holds an invalid value!";
				throw runtime_error(errMsg);
			}
		}
	}

	template<> string Read<string>(string section, string key)
	{
		wstring returnedString = GetValue(section, key);

		if (returnedString == lpDefault) {
			string errMsg = section + '.' + key + " was not found!";
			throw runtime_error(errMsg);
		}
		else
			return string(returnedString.begin(), returnedString.end());
	}

private:
	inline wstring GetValue(string &section, string &key)
	{
		wchar_t *lpSection = new wchar_t[section.length() + 1];
		copy(section.begin(), section.end(), lpSection);
		lpSection[section.length()] = '\0';

		wchar_t *lpKey = new wchar_t[key.length() + 1];
		copy(key.begin(), key.end(), lpKey);
		lpKey[key.length()] = '\0';

		LPWSTR	lpReturnedString = new wchar_t[bufferSize];

		GetPrivateProfileString(lpSection, lpKey, lpDefault, lpReturnedString, bufferSize, lpFilename);

		wstring returnedString(lpReturnedString);

		delete lpSection;
		delete lpKey;
		delete lpReturnedString;

		//returnedString.find('#');

		return returnedString;
	}

	LPWSTR					lpFilename;
	const DWORD				bufferSize = 256;
	LPWSTR					lpDefault = L"NOT FOUND\0";
	regex					integer = regex("(\\+|-)?[[:d:]]+");
	regex					real = regex("((\\+|-)?[[:d:]]+)(\\.(([[:d:]]+)?))?");
};