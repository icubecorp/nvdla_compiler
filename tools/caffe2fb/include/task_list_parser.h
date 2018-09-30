/*
 * task_list_parser.h
 *
 *  Created on: Sep 18, 2018
 *      Author: jiqianxiang
 */

#ifndef TASK_LIST_PARSER_H_
#define TASK_LIST_PARSER_H_

#include "list_entry_parser.h"
#include "debug.h"

namespace nvdla {

class TaskListParser: public ListEntryParser {
public:
	TaskListParser(NetParser* net);
	virtual ~TaskListParser();

	void  buildList();
	const void* getList() const;

	const std::vector<ILoadable::Interface> get_tasks_type() const {
		std::vector<ILoadable::Interface> types;
		for (const ILoadable::TaskListEntry& task : mList) {
			types.push_back((ILoadable::Interface)task.interface);
		}
		return types;
	}

	const std::vector<NvU16> get_task_id() const {
		std::vector<NvU16> task_ids;
		for (const ILoadable::TaskListEntry& task : mList)
			task_ids.push_back(task.id);
		return task_ids;
	}

	void fillAddressList();
	void taskTypeParser(Layer* layer, Layer* pre_layer, NvU32* typecount, NvU32* tasktype);
	void dumpList();

private:
	std::vector<ILoadable::TaskListEntry> mList;

};

} /* namespace nvdla */

#endif /* TASK_LIST_PARSER_H_ */
