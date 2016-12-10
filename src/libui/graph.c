/**
 * @file graph.c
 * @synopsis 
 * @author alan(a1an1in@sina.com)
 * @version 1
 * @date 2016-11-21
 */
#include <libui/graph.h>
#include <libui/component.h>

static int __construct(Graph *graph,char *init_str)
{
	dbg_str(DBG_DETAIL,"graph construct, graph addr:%p",graph);

	return 0;
}

static int __deconstrcut(Graph *graph)
{
	dbg_str(DBG_DETAIL,"graph deconstruct,graph addr:%p",graph);

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
	} else if(strcmp(attrib, "init_window") == 0) {
		graph->init_window = value;
	} else if(strcmp(attrib, "close_window") == 0) {
		graph->close_window = value;
	} else if(strcmp(attrib, "update_window") == 0) {
		graph->update_window = value;
	} else if(strcmp(attrib, "draw_image") == 0) {
		graph->draw_image = value;
	} else if(strcmp(attrib, "render_create") == 0) {
		graph->render_create = value;
	} else if(strcmp(attrib, "render_destroy") == 0) {
		graph->render_destroy = value;
	} else if(strcmp(attrib, "render_set_color") == 0) {
		graph->render_set_color = value;
	} else if(strcmp(attrib, "render_set_font") == 0) {
		graph->render_set_font = value;
	} else if(strcmp(attrib, "render_clear") == 0) {
		graph->render_clear = value;
	} else if(strcmp(attrib, "render_draw_line") == 0) {
		graph->render_draw_line = value;
	} else if(strcmp(attrib, "render_draw_rect") == 0) {
		graph->render_draw_rect = value;
	} else if(strcmp(attrib, "render_fill_rect") == 0) {
		graph->render_fill_rect = value;
	} else if(strcmp(attrib, "render_draw_image") == 0) {
		graph->render_draw_image = value;
	} else if(strcmp(attrib, "render_load_image") == 0) {
		graph->render_load_image = value;
	} else if(strcmp(attrib, "render_load_text") == 0) {
		graph->render_load_text = value;
	} else if(strcmp(attrib, "render_unload_text") == 0) {
		graph->render_unload_text = value;
	} else if(strcmp(attrib, "render_write_text") == 0) {
		graph->render_write_text = value;
	} else if(strcmp(attrib, "render_present") == 0) {
		graph->render_present = value;

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
	[0 ] = {ENTRY_TYPE_OBJ,"Obj","obj",NULL,sizeof(void *)},
	[1 ] = {ENTRY_TYPE_FUNC_POINTER,"","set",__set,sizeof(void *)},
	[2 ] = {ENTRY_TYPE_FUNC_POINTER,"","get",__get,sizeof(void *)},
	[3 ] = {ENTRY_TYPE_FUNC_POINTER,"","construct",__construct,sizeof(void *)},
	[4 ] = {ENTRY_TYPE_FUNC_POINTER,"","deconstruct",__deconstrcut,sizeof(void *)},
	[5 ] = {ENTRY_TYPE_VFUNC_POINTER,"","init_window",NULL,sizeof(void *)},
	[6 ] = {ENTRY_TYPE_VFUNC_POINTER,"","close_window",NULL,sizeof(void *)},
	[7 ] = {ENTRY_TYPE_VFUNC_POINTER,"","update_window",NULL,sizeof(void *)},
	[8 ] = {ENTRY_TYPE_VFUNC_POINTER,"","draw_image",NULL,sizeof(void *)},
	[9 ] = {ENTRY_TYPE_VFUNC_POINTER,"","render_create",NULL,sizeof(void *)},
	[10] = {ENTRY_TYPE_VFUNC_POINTER,"","render_destroy",NULL,sizeof(void *)},
	[11] = {ENTRY_TYPE_VFUNC_POINTER,"","render_set_color",NULL,sizeof(void *)},
	[12] = {ENTRY_TYPE_VFUNC_POINTER,"","render_set_font",NULL,sizeof(void *)},
	[13] = {ENTRY_TYPE_VFUNC_POINTER,"","render_clear",NULL,sizeof(void *)},
	[14] = {ENTRY_TYPE_VFUNC_POINTER,"","render_draw_line",NULL,sizeof(void *)},
	[15] = {ENTRY_TYPE_VFUNC_POINTER,"","render_draw_rect",NULL,sizeof(void *)},
	[16] = {ENTRY_TYPE_VFUNC_POINTER,"","render_fill_rect",NULL,sizeof(void *)},
	[17] = {ENTRY_TYPE_VFUNC_POINTER,"","render_draw_image",NULL,sizeof(void *)},
	[18] = {ENTRY_TYPE_VFUNC_POINTER,"","render_load_image",NULL,sizeof(void *)},
	[19] = {ENTRY_TYPE_VFUNC_POINTER,"","render_load_text",NULL,sizeof(void *)},
	[20] = {ENTRY_TYPE_VFUNC_POINTER,"","render_unload_text",NULL,sizeof(void *)},
	[21] = {ENTRY_TYPE_VFUNC_POINTER,"","render_write_text",NULL,sizeof(void *)},
	[22] = {ENTRY_TYPE_VFUNC_POINTER,"","render_present",NULL,sizeof(void *)},
	[23] = {ENTRY_TYPE_STRING,"char","name",NULL,0},
	[24] = {ENTRY_TYPE_END},
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


