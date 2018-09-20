/*
 * submit_list_parser.cpp
 *
 *  Created on: Sep 18, 2018
 *      Author: jiqianxiang
 */

#include "submit_list_parser.h"

namespace nvdla {

SubmitListParser::SubmitListParser(TaskListParser* task_parser) :
	ListEntryParser(),
	mTaskListPtr(task_parser)
{
	// TODO Auto-generated constructor stub
}

SubmitListParser::~SubmitListParser() {
	// TODO Auto-generated destructor stub
}

void* SubmitListParser::getList() const {
	return (void*)&mList;
}

void SubmitListParser::buildList() {

	//TODO always only one submit for all tasks

	ILoadable::SubmitListEntry submit;

	submit.id = 0;
	submit.tasks = mTaskListPtr->get_task_id();

	mList.push_back(submit);
}

} /* namespace nvdla */
