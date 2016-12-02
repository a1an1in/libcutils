/**
 * @file graph.c
 * @synopsis 
 * @author alan(a1an1in@sina.com)
 * @version 1
 * @date 2016-11-21
 */
#include <libui/graph.h>

static int __construct(Graph *graph,char *init_str)
{
	dbg_str(DBG_SUC,"graph construct, graph addr:%p",graph);

	return 0;
}

static int __deconstrcut(Graph *graph)
{
	dbg_str(DBG_SUC,"graph deconstruct,graph addr:%p",graph);

	return 0;
}

static int __set(Graph *graph, char *attrib, void *value)
{
	if(strcmp(attrib, "set") == 0) {
		graph->set = value;
    } else if(strcmp(attrib, "get") == 0) {
		graph->get = value;
	} else if(strcmp(attrib, "construct") == 0) {
		graph->construct = value;
	} else if(strcmp(attrib, "deconstruct") == 0) {
		graph->deconstruct = value;
	} else if(strcmp(attrib, "move") == 0) {
		graph->move = value;
	} else if(strcmp(attrib, "name") == 0) {
        strncpy(graph->name,value,strlen(value));
	} else {
		dbg_str(DBG_DETAIL,"graph set, not support %s setting",attrib);
	}

	return 0;
}

static void *__get(Graph *obj, char *attrib)
{
    if(strcmp(attrib, "name") == 0) {
        return obj->name;
    } else {
        dbg_str(DBG_WARNNING,"graph get, \"%s\" getting attrib is not supported",attrib);
        return NULL;
    }
    return NULL;
}

static class_info_entry_t graph_class_info[] = {
	[0] = {ENTRY_TYPE_OBJ,"Obj","obj",NULL,sizeof(void *)},
	[1] = {ENTRY_TYPE_FUNC_POINTER,"","set",__set,sizeof(void *)},
	[2] = {ENTRY_TYPE_FUNC_POINTER,"","get",__get,sizeof(void *)},
	[3] = {ENTRY_TYPE_FUNC_POINTER,"","construct",__construct,sizeof(void *)},
	[4] = {ENTRY_TYPE_FUNC_POINTER,"","deconstruct",__deconstrcut,sizeof(void *)},
	[5] = {ENTRY_TYPE_STRING,"char","name",NULL,0},
	[6] = {ENTRY_TYPE_END},

};
REGISTER_CLASS("Graph",graph_class_info);

void test_ui_graph()
{
    Graph *graph;
	allocator_t *allocator = allocator_get_default_alloc();
    char *set_str;
    cjson_t *root, *e, *s;
    char buf[2048];

    root = cjson_create_object();{
        cjson_add_item_to_object(root, "Graph", e = cjson_create_object());{
            cjson_add_string_to_object(e, "name", "alan");
        }
    }

    set_str = cjson_print(root);

    graph = OBJECT_NEW(allocator, Graph,set_str);

    object_dump(graph, "Graph", buf, 2048);
    dbg_str(DBG_DETAIL,"Graph dump: %s",buf);

    object_destroy(graph);

    free(set_str);

}


