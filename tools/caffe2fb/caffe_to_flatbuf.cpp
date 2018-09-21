/*
 * caffe_to_flatbuf.cpp
 *
 *  Created on: Sep 20, 2018
 *      Author: jiqianxiang
 */

#include "caffe_to_flatbuf.h"

#include "task_list_parser.h"
#include "submit_list_parser.h"
#include "memory_list_parser.h"
#include "address_list_parser.h"
#include "event_list_parser.h"
#include "tensor_desc_list_parser.h"
#include "symbol_list_parser.h"
#include "reloc_list_parser.h"

#include "debug.h"

#include "priv/Loadable.h"
#include "flatbuffers/flatbuffers.h"

namespace nvdla {

CaffeToFlatbuf::CaffeToFlatbuf(const char * protopath, const char * modelpath) :
	mProtoPath(nullptr),
	mModelPath(nullptr)
{
	mNetParserPtr = new NetParser();

	/*
	 * 	enum LIST_PARSER_TYPE {
		TASK_LIST_PARSER,
		SUBMIT_LIST_PARSER,
		MEMORY_LIST_PARSER,
		ADDRESS_LIST_PARSER,
		EVENT_LIST_PARSER,
		TENSOR_DESC_LIST_PARSER,
		SYMBOL_LIST_PARSER,
		RELOC_LIST_PARSER,
		LIST_PARSER_NUM
	};
	 * */

	//should follow the LIST_PARSER_TYPE order

	ListEntryParser* parser = new TaskListParser(mNetParserPtr);
	mListParsers.push_back(parser);

	parser = new SubmitListParser(mNetParserPtr, (TaskListParser*)mListParsers[TASK_LIST_PARSER]);
	mListParsers.push_back(parser);

	parser = new MemoryListParser(mNetParserPtr, (TaskListParser*)mListParsers[TASK_LIST_PARSER]);
	mListParsers.push_back(parser);

	parser = new AddressListParser(mNetParserPtr, (MemoryListParser*)mListParsers[MEMORY_LIST_PARSER]);
	mListParsers.push_back(parser);

	parser = new EventListParser(mNetParserPtr);
	mListParsers.push_back(parser);

	parser = new TensorDescListParser(mNetParserPtr);
	mListParsers.push_back(parser);

	parser = new SymbolListParser(mNetParserPtr, (MemoryListParser*)mListParsers[MEMORY_LIST_PARSER]);
	mListParsers.push_back(parser);

	parser = new RelocListParser(mNetParserPtr);
	mListParsers.push_back(parser);


	mLoadable = priv::LoadableFactory::newLoadable();

}

CaffeToFlatbuf::~CaffeToFlatbuf()
{
	delete(mNetParserPtr);

	for (ListEntryParser* & ele : mListParsers)
		delete (ele);
	mListParsers.clear();
}

void CaffeToFlatbuf::loadNetwork()
{
	if (!mProtoPath || !mModelPath) {
		log_error("please specify .prototxt and .caffemodel path\n");
		return;
	}

	mNetParserPtr->load_caffe_net(mProtoPath, mModelPath);
	mNetParserPtr->build_nvdla_net();

}

void CaffeToFlatbuf::fillAllList()
{
	if (mListParsers.size() != LIST_PARSER_NUM) {
		log_error("size of list parser vector != LIST_PARSER_NUM, please check.\n");
		return;
	}

	for (int i=0; i<LIST_PARSER_NUM; i++) {
		log_debug("build list: %s\n", "test");
		mListParsers[i]->buildList();
	}
}

void CaffeToFlatbuf::generateFlatbuf()
{
    mLoadable.priv()->setMemoryListEntries(
    		*(std::vector<ILoadable::MemoryListEntry>*)mListParsers[MEMORY_LIST_PARSER]->getList());
    mLoadable.priv()->setEventListEntries(
    		*(std::vector<ILoadable::EventListEntry>*)mListParsers[EVENT_LIST_PARSER]->getList());
    mLoadable.priv()->setTaskListEntries(
    		*(std::vector<ILoadable::TaskListEntry>*)mListParsers[TASK_LIST_PARSER]->getList());
    mLoadable.priv()->setSubmitListEntries(
    		*(std::vector<ILoadable::SubmitListEntry>*)mListParsers[SUBMIT_LIST_PARSER]->getList());
    mLoadable.priv()->setAddressListEntries(
    		*(std::vector<ILoadable::AddressListEntry>*)mListParsers[ADDRESS_LIST_PARSER]->getList());
    mLoadable.priv()->setTensorDescListEntries(
    		*(std::vector<ILoadable::TensorDescListEntry>*)mListParsers[TENSOR_DESC_LIST_PARSER]->getList());
    mLoadable.priv()->setRelocEntries(
    		*(std::vector<ILoadable::RelocEntry>*)mListParsers[RELOC_LIST_PARSER]->getList());


    //TODO set symbol with blob
    /*
    const std::vector<priv::Loadable::Symbol>* symbols =
    		(const std::vector<priv::Loadable::Symbol>*)mListParsers[SYMBOL_LIST_PARSER]->getList();
    for (int i=0; i<symbols->size(); i++)
    	mLoadable.priv()->setSymbolContent(name, Blob, data);
	*/

    mLoadable.priv()->serialize();

    //TODO should call flatbuffers::SaveFile(), where to find this function?

    //flatbuffers::SaveFile();

}

} /* namespace nvdla */
