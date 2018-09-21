/*
 * caffe_to_flatbuf.h
 *
 *  Created on: Sep 20, 2018
 *      Author: jiqianxiang
 */

#ifndef CAFFE_TO_FLATBUF_H_
#define CAFFE_TO_FLATBUF_H_

#include "net_parser.h"
#include "list_entry_parser.h"
#include "priv/Loadable.h"

class LoadablePrivPair;

namespace nvdla {

class CaffeToFlatbuf {
public:
	CaffeToFlatbuf(const char * protopath, const char * modelpath);
	virtual ~CaffeToFlatbuf();

	void loadNetwork();
	void fillAllList();
	void generateFlatbuf();

private:
	const char* mProtoPath;
	const char* mModelPath;

	NetParser* mNetParserPtr;
	std::vector<ListEntryParser*> mListParsers;

	priv::LoadableFactory::LoadablePrivPair mLoadable;
};

} /* namespace nvdla */

#endif /* CAFFE_TO_FLATBUF_H_ */
