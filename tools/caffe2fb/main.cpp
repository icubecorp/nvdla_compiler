#include "net.h"

#include "task_list_parser.h"

int main()
{
    nvdla::Net lenet;

    lenet.load_param("lenet.param");
    lenet.load_model("lenet.bin");	

    //test
    nvdla::TaskListParser task_parser;
    task_parser.buildList();
    task_parser.getList();    

    return 0;
}
