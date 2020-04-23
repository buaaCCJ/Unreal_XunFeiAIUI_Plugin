/*
 * AIUIType.h
 *
 *  Created on: 2017年2月17日
 *      Author: hj
 */

#ifndef AIUITYPE_INTERNAL_H_
#define AIUITYPE_INTERNAL_H_

#include "aiui/AIUICommon.h"

#include <string>
#include <map>
#include <stdint.h>
#include <sys/types.h>

using namespace std;


namespace aiui {


/*
 数据捆绑对象，支持int、long、string和char*类型数据捆绑传输。
 */

class Buffer;
class DataBundle : public IDataBundle
{
private:
	enum DataType {
		_integer = 1,
		_long,
		_string,
		_binary
	};

	void createMap();

private:
	map<string, Buffer*>* mDataMapPtr;

	int getDataType(Buffer* dataBuffer);

	void putDataType(Buffer* dataBuffer, int dataType);

	Buffer* createDataBuffer(DataType dataType, int dataLen);

	bool putData(Buffer* dataBuffer, const char* data, int dataLen);

	const char* getCharString(const string& key, const string& defaultVal);

	Buffer* getBinary(const string& key);

public:
	DataBundle();

	DataBundle(map<string, Buffer*>* dataMap);

	virtual ~DataBundle();

	map<string, Buffer*>* getDataMap();

	void release();

	bool remove(const string& key);

	bool putInt(const string& key, int val, bool replace = false);

	int getInt(const string& key, int defaultVal);

	bool putLong(const string& key, long val, bool replace = false);

	long getLong(const string& key, long defaultVal);

	bool putString(const string& key, const string& val, bool replace = false);

	string getString(const string& key, const string& defaultVal);

	bool putBinary(const string& key, const char* data, int dataLen, bool replace = false);

	const char* getBinary(const string& key, int* dataLen);

public:
	/* interface implementation */
	void destroy() { release(); delete this; }
	
	bool remove(const char* key)  { 
		if (key == NULL) return false;
		return remove(string(key));
	}

	bool putInt(const char* key, int val, bool replace = false) {
		if (key == NULL) return false;
		return putInt(string(key), val, replace);
	}

	int getInt(const char* key, int defaultVal){
		if (key == NULL) return defaultVal;
		return getInt(string(key), defaultVal);
	}

	bool putLong(const char* key, long val, bool replace = false) {
		if (key == NULL) return false;
		return putLong(string(key), val, replace);
	}

	long getLong(const char* key, long defaultVal){
		if (key == NULL) return defaultVal;
		return getLong(string(key), defaultVal);
	}

	bool putString(const char* key, const char* val, bool replace = false) {
		if (key == NULL) return false;
		return putString(string(key), string(val), replace);
	}

	const char* getString(const char* key, const char* defaultVal) {
		if (key == NULL) return defaultVal;
		return getCharString(string(key), string(defaultVal));
	}

	bool putBinary(const char* key, const char* data, int dataLen, bool replace = false) {
		if (key == NULL || data == NULL)
			return false;
		return putBinary(string(key), data, dataLen, replace);
	}

	const char* getBinary(const char* key, int* dataLen) {
		if (key == NULL) return NULL;
		return getBinary(string(key), dataLen);
	}
};

// ---------------------------------------------------------------------------

};

#endif /* AIUITYPE_H_ */
