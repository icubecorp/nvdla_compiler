/*
 * submit_list_parser.cpp
 *
 *  Created on: Sep 18, 2018
 *      Author: jiqianxiang
 */

#include "submit_list_parser.h"

namespace nvdla {

SubmitListParser::SubmitListParser(NetParser* net, TaskListParser* task_parser) :
	ListEntryParser(net),
	mTaskListPtr(task_parser)
{
	// TODO Auto-generated constructor stub
}

SubmitListParser::~SubmitListParser() {
	// TODO Auto-generated destructor stub
}

const void* SubmitListParser::getList() const {
	return (const void*)&mList;
}

void SubmitListParser::buildList() {

    std::vector<NvU16> task_ids = mTaskListPtr->get_task_id();
    for(unsigned int i = 0; i < task_ids.size(); i++ ){
	    ILoadable::SubmitListEntry submit;
    	submit.id = i;
    	submit.tasks.push_back(task_ids[i]);
    	mList.push_back(submit);
    }


}

void SubmitListParser::dumpList() {
    debug_info("enter SubmitListParser %s line=%d\n",__FUNCTION__,__LINE__);
    for(unsigned int i = 0; i < mList.size(); i++){
        debug_info("submit id=%d\n",mList[i].id);
        for(unsigned int j = 0; j < mList[i].tasks.size(); j++)
             debug_info("task_id id=%d %s\n",mList[i].tasks[j],__FUNCTION__);
    }

}



} /* namespace nvdla */
