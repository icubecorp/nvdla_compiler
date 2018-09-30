

#include "caffe_to_flatbuf.h"

#include "task_list_parser.h"
#include "submit_list_parser.h"
#include "memory_list_parser.h"
#include "address_list_parser.h"
#include "event_list_parser.h"
#include "tensor_desc_list_parser.h"
#include "symbol_list_parser.h"
#include "reloc_list_parser.h"

using namespace nvdla;
int main()
{
    #if 1
    nvdla::CaffeToFlatbuf * ctf = new CaffeToFlatbuf("lenet.param","lenet.bin");
    ctf->loadNetwork();
    ctf->fillAllList();
    ctf->generateFlatbuf();
    delete (ctf);
    return 0;
    #else
    nvdla::NetParser lenet;

    lenet.load_caffe_net("lenet.param","lenet.bin");
    lenet.build_nvdla_net();
    TaskListParser* tlp = new TaskListParser(&lenet);
    //tlp->buildList();
    
    RelocListParser* reloclist = new RelocListParser(&lenet);
    reloclist->buildList();

    EventListParser* eventlist = new EventListParser(&lenet);
    eventlist->buildList();
 
    MemoryListParser* memlist = new MemoryListParser(&lenet, tlp);
    memlist->buildList();
    tlp->buildList();
    memlist->fillTaskAddrList();
    tlp->debugTaskList();    
    memlist->debugMemList();
    AddressListParser *addresslist = new AddressListParser(&lenet, memlist);
    addresslist->buildList();
    addresslist->dumplist();
    SubmitListParser *submitlist = new SubmitListParser(&lenet, tlp);
    submitlist->buildList();
    TensorDescListParser *tensorlist = new TensorDescListParser(&lenet, memlist);
    tensorlist->buildList();
    tensorlist->dump_tensor_info();
    SymbolListParser* symbollist = new SymbolListParser(&lenet,memlist,tlp);
    symbollist->buildList();
    symbollist->dump_blobs_info();
    return 0;
    #endif
}
