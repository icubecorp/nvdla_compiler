/*
 * task_list_parser.cpp
 *
 *  Created on: Sep 18, 2018
 *      Author: jiqianxiang
 */

#include "task_list_parser.h"

namespace nvdla {

static NvU32 task_id = 0;

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

void TaskListParser::taskTypeParser(Layer* layer, Layer* pre_layer, NvU32* typecount, NvU32* tasktype){
	if(!layer || !pre_layer || !typecount || !tasktype){
		return ;
	}
	
	if((pre_layer->nvdla_type == NvConv) ||  (pre_layer->nvdla_type == NvSDP) || (pre_layer->nvdla_type == NvPDP)){
		if((layer->nvdla_type == NvConv) ||  (layer->nvdla_type == NvSDP) || (layer->nvdla_type == NvPDP)){
            //task 0   dla_task
			*tasktype = 0; 
			//count
			*typecount = 1;
		}else if(layer->nvdla_type == NvSoftmax){
		    //task 1   emu_task
		    *tasktype = 1;
			//start
			*typecount = 0;
		}
	}else if(pre_layer->nvdla_type == NvSoftmax){
	    if((layer->nvdla_type == NvConv) ||  (layer->nvdla_type == NvSDP) || (layer->nvdla_type == NvPDP)){
			//task 0 dla_task
			*tasktype = 0;
			//start
			*typecount = 0;
	    }else if (layer->nvdla_type == NvSoftmax){
	    	//task 1 emu_task
	    	*tasktype = 1;
			*typecount = 1;
	    }
	}else if(pre_layer->nvdla_type == NvInput){
	    if((layer->nvdla_type == NvConv) ||  (layer->nvdla_type == NvSDP) || (layer->nvdla_type == NvPDP)){
			//task 0 dla_task
			*tasktype = 0;
			//start
			*typecount = 0;
	    }else if (layer->nvdla_type == NvSoftmax){
	    	//task 1 emu_task
	    	*tasktype = 1;
			//start
			*typecount = 0;
	    }
	}
	return ;
}

void TaskListParser::buildList() {

	if (!mList.empty()) {
		printf("Warning: list only build for once\n");
		return;
	}

	std::vector<Layer*> layers = mNetParserPtr->getLayers();
	Layer* layer = NULL;
	Layer* pre_layer = NULL;
	NvU32 task_type = 0;
	NvU32 type_count = 0;
	
	//task start and end index
	NvU32 task_dla_start_index = 0;
	NvU32 task_emu_start_index = 0;
	NvU32 task_end_index = 0;
	
	//task start and end flag
	NvU32 task_dla_start_flag = 0;
	NvU32 task_emu_start_flag = 0;
	
	ILoadable::TaskListEntry task;
	debug_info("%s, %d, layers.size = %d\n", __FUNCTION__, __LINE__, layers.size());
	for (NvU32 i=0; i<layers.size(); i++) {

		layer = layers[i];
		if(i>0){
			pre_layer = layers[i-1];
		}

		if (layer->nvdla_type == NvInput)
			continue;

		taskTypeParser(layer, pre_layer, &type_count, &task_type);
		debug_info("%s, %d, type_count = %d, task_type = %d\n", __FUNCTION__, __LINE__, type_count, task_type);
		if(task_type == 0){
			//dla task 
			if(0 == type_count){
				//start
				task_dla_start_flag = 1;
				task_dla_start_index = i;

			}else if(1 == type_count){
				task_end_index++;
			}
		}else if(task_type == 1){
			//emu task
			if(0 == type_count){
				//start
				task_emu_start_index = i;
				task_emu_start_flag = 1;

			}else if(1 == type_count){
				task_end_index++;
			}
		}
		debug_info("%s, %d, task_dla_start_flag = %d, task_emu_start_flag = %d\n", __FUNCTION__, __LINE__, task_dla_start_flag, task_emu_start_flag);
		if((task_emu_start_flag == 1)){
			if(pre_layer->nvdla_type != NvSoftmax){
				//dla task end
				task.interface= ILoadable::Interface_DLA1;
				task.id = task_id;
				task.instance = -1;
				//push start index
				while(task.preactions.size()){
					task.preactions.pop_back();
				}
				task.preactions.push_back(task_dla_start_index);
				//push end index
				while(task.postactions.size()){
					task.postactions.pop_back();
				}
				task.postactions.push_back(task_dla_start_index + task_end_index);
				debug_info("%s, %d, task_dla_start_index = %d, task_end_index = %d\n", __FUNCTION__, __LINE__, task_dla_start_index, task_end_index);
				//flag and index clear
				task_end_index = 0;
				task_dla_start_flag = 0;
				
				mList.push_back(task);
				//task_id
				task_id++;
			}
			
			//the last softmax layer
			if(i == ((layers.size() - 1))){
				if(layer->nvdla_type == NvSoftmax){
					//emu task end
					task.interface = ILoadable::Interface_EMU1;
					task.id = task_id;
					task.instance = -1;
					//push start index
					while(task.preactions.size()){
						task.preactions.pop_back();
					}
					task.preactions.push_back(task_emu_start_index);
					//push end index
					while(task.postactions.size()){
						task.postactions.pop_back();
					}
					task.postactions.push_back(task_emu_start_index + task_end_index);
					debug_info("%s, %d, task_start_index = %d, task_end_index = %d\n", __FUNCTION__, __LINE__, task_emu_start_index, task_end_index);
					//flag and index clear
					task_end_index = 0;
					task_emu_start_flag = 0;
					
					mList.push_back(task);
					//task_id
					task_id++;			
				}
			}
		}else if((task_dla_start_flag == 1)){
			if(layer->nvdla_type == NvSoftmax){
				//emu task end
				task.interface = ILoadable::Interface_EMU1;
				task.id = task_id;
				task.instance = -1;
				//push start index
				while(task.preactions.size()){
					task.preactions.pop_back();
				}
				task.preactions.push_back(task_emu_start_index);
				//push end index
				while(task.preactions.size()){
					task.preactions.pop_back();
				}
				task.postactions.push_back(task_emu_start_index + task_end_index);
				debug_info("%s, %d, task_start_index = %d, task_end_index = %d\n", __FUNCTION__, __LINE__, task_emu_start_index, task_end_index);
				//flag and index clear
				task_end_index = 0;
				task_emu_start_flag = 0;
				
				mList.push_back(task);
				//task_id
				task_id++;			
			}
		}

	}
	debug_info("%s, %d, mList.size = %d\n", __FUNCTION__, __LINE__, mList.size());
	/*
	for (int i=0; i<layers.size(); i++) {
		ILoadable::TaskListEntry task;

		layer = layers[i];

		if (layer->nvdla_type == NvInput)
			continue;

		
		if (layer->nvdla_type == NvConv ||
			layer->nvdla_type == NvSDP  ||
			layer->nvdla_type == NvPDP) {

			if (current_type == ILoadable::Interface_NONE)
				current_type = ILoadable::Interface_DLA1;

		}
		

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
	*/
}

void TaskListParser::fillAddressList()
{ 
	//TODO
}

void TaskListParser::dumpList(){
	ILoadable::TaskListEntry tle;
	NvU32 index=0;
	NvU32 i=0;
	
	if(mList.size() ==  0){
		printf("%s, %d, task vector mTaskList is NULL!\n", __FUNCTION__, __LINE__);
		return ;
	}
	debug_info("\n");
	debug_info("------------%s------------------\n", __FUNCTION__);
	for(index=0; index<mList.size(); index++){
		tle = mList[index];
		debug_info("--------index = %d---------\n", index);
		debug_info("tle.id = %d, tle.interface = %d, tle.instance = %d\n", tle.id, tle.interface, tle.instance);
		for(i=0; i<tle.preactions.size(); i++){
			debug_info("tle.preactions[%d] = %d\n", i, tle.preactions[i]);
		}
		for(i=0; i<tle.postactions.size(); i++){
			debug_info("tle.postactions[%d] = %d\n", i, tle.postactions[i]);
		}
		for(i=0; i<tle.address_list.size(); i++){
			debug_info("tle.address_list[%d] = %d\n", i, tle.address_list[i]);
		}
		debug_info("-------------------------\n");
	}
	return ;
	
}

} /* namespace nvdla */
