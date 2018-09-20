/*
 * task_list_parser.cpp
 *
 *  Created on: Sep 18, 2018
 *      Author: jiqianxiang
 */

#include "task_list_parser.h"

namespace nvdla {

TaskListParser::TaskListParser(NetParser* net) :
    ListEntryParser(net)
{

}

TaskListParser::~TaskListParser()
{

}

const void* TaskListParser::getList() const {
	return (const void*)&mList;
}

void TaskListParser::buildList() {

	if (!mList.empty()) {
		printf("Warning: list only build for once\n");
		return;
	}

	std::vector<Layer*> layers = mNetParserPtr->getLayers();
	NvU16 task_id = 0;
	ILoadable::Interface current_type = ILoadable::Interface_NONE;
	for (int i=0; i<layers.size(); i++) {
		ILoadable::TaskListEntry task;

		Layer* layer = layers[i];

		if (layer->nvdla_type == NvInput)
			continue;

		/*
		if (layer->nvdla_type == NvConv ||
			layer->nvdla_type == NvSDP  ||
			layer->nvdla_type == NvPDP) {

			if (current_type == ILoadable::Interface_NONE)
				current_type = ILoadable::Interface_DLA1;

		}
		*/

		if (layer->nvdla_type == NvSoftmax) {
			task.id = task_id++;
			task.interface = ILoadable::Interface_DLA1;
			task.instance = -1;
			mList.push_back(task);

			task.id = task_id++;
			task.interface = ILoadable::Interface_EMU1;
			task.instance = -1;
			mList.push_back(task);
		}
	}
}

void TaskListParser::fillAddressList()
{
	//TODO
}

} /* namespace nvdla */
