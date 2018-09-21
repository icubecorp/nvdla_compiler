/*
 * event_list_parser.h
 *
 *  Created on: Sep 20, 2018
 *      Author: jiqianxiang
 */

#ifndef EVENT_LIST_PARSER_H_
#define EVENT_LIST_PARSER_H_

#include "list_entry_parser.h"

namespace nvdla {

class EventListParser: public ListEntryParser {
public:
	EventListParser(NetParser* net);
	virtual ~EventListParser();

	void  buildList();
	const void* getList() const;

private:
	std::vector<ILoadable::EventListEntry> mList;
};

} /* namespace nvdla */

#endif /* EVENT_LIST_PARSER_H_ */
