/*
 * task_list_parser.cpp
 *
 *  Created on: Sep 18, 2018
 *      Author: jiqianxiang
 */

#include "task_list_parser.h"

namespace nvdla {

TaskListParser::TaskListParser() {
	// TODO Auto-generated constructor stub

}

TaskListParser::~TaskListParser() {
	// TODO Auto-generated destructor stub
}

void* TaskListParser::getList() const {
	return (void*)&mList;
}

void TaskListParser::buildList() {

	int layer_num = 0;

	for (int i=0; i<layer_num; i++) {

		ILoadable::TaskListEntry task;

		task.id = 0;
		task.interface = ILoadable::Interface_DLA1;
		task.instance = -1;

		// pre-action
		for(;;) {
			task.preactions.push_back(0);
		}

		// post-action
		for(;;) {
			task.postactions.push_back(0);
		}

		//task.address_list = ?

		mList.push_back(task);
	}
}

} /* namespace nvdla */
