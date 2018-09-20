/*
 * submit_list_parser.h
 *
 *  Created on: Sep 18, 2018
 *      Author: jiqianxiang
 */

#ifndef SUBMIT_LIST_PARSER_H_
#define SUBMIT_LIST_PARSER_H_

#include "list_entry_parser.h"
#include "task_list_parser.h"

namespace nvdla {

class SubmitListParser: public ListEntryParser {
public:
	SubmitListParser(TaskListParser* task_parser);
	virtual ~SubmitListParser();

	void  buildList();
	void* getList() const;

private:
	std::vector<ILoadable::SubmitListEntry> mList;
	TaskListParser* mTaskListPtr;
};

} /* namespace nvdla */

#endif /* SUBMIT_LIST_PARSER_H_ */
