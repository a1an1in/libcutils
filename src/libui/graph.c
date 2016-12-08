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
	} else if(strcmp(attrib, "render_clear") == 0) {
		graph->render_clear = value;
	} else if(strcmp(attrib, "render_draw_line") == 0) {
		graph->render_draw_line = value;
	} else if(strcmp(attrib, "render_fill_rect") == 0) {
		graph->render_fill_rect = value;
	} else if(strcmp(attrib, "render_draw_image") == 0) {
		graph->render_draw_image = value;
	} else if(strcmp(attrib, "render_load_image") == 0) {
		graph->render_load_image = value;
	} else if(strcmp(attrib, "render_load_text") == 0) {
		graph->render_load_text = value;
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

static int __init_window(Graph *graph, void *window)
{
	dbg_str(DBG_DETAIL,"graph init window");
}

static int __close_window(Graph *graph, void *window)
{
	dbg_str(DBG_DETAIL,"graph close_window");
}

static int __load_image(Graph *graph, void *image)
{
	dbg_str(DBG_DETAIL,"graph load image");
}

static int __update_window(Graph *graph)
{
	dbg_str(DBG_DETAIL,"graph update_window");
}

static int __draw_image(Graph *graph, void *image)
{
	dbg_str(DBG_DETAIL,"graph draw_image");
}

static int __render_create(Graph *graph)
{
	dbg_str(DBG_DETAIL,"Graph render_create");
}

static int __render_destroy(Graph *graph)
{
	dbg_str(DBG_DETAIL,"Graph render_destroy");
}

static int __render_set_color(Graph *graph, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	dbg_str(DBG_DETAIL,"Graph render_set_color");
}

static int __render_clear(Graph *graph)
{
	dbg_str(DBG_DETAIL,"Graph render_clear");
}

static int __render_draw_line(Graph *graph,int x1, int y1, int x2, int y2)
{
	dbg_str(DBG_DETAIL,"Graph render_draw_line");
}

static int __render_fill_rect(Graph *graph,int x1, int y1, int x2, int y2)
{
	dbg_str(DBG_DETAIL,"Graph render_fill_rect");
}

static int __render_load_image(Graph *graph,void *image)
{
	dbg_str(DBG_DETAIL,"Graph render_draw_image");
}

static int __render_draw_image(Graph *graph, int x, int y,void *image)
{
	dbg_str(DBG_DETAIL,"Graph render_load_image");
}

static int __render_present(Graph *graph)
{
	dbg_str(DBG_DETAIL,"Graph render_present");
}

static int __render_load_text(Graph *graph,void *text,void *font,int r, int g, int b, int a)
{
	dbg_str(DBG_DETAIL,"Graph render_load_text");

}
static int __render_write_text(Graph *graph,int x, int y, void *text)
{
	dbg_str(DBG_DETAIL,"Graph render_write_text");
}

static class_info_entry_t graph_class_info[] = {
	[0 ] = {ENTRY_TYPE_OBJ,"Obj","obj",NULL,sizeof(void *)},
	[1 ] = {ENTRY_TYPE_FUNC_POINTER,"","set",__set,sizeof(void *)},
	[2 ] = {ENTRY_TYPE_FUNC_POINTER,"","get",__get,sizeof(void *)},
	[3 ] = {ENTRY_TYPE_FUNC_POINTER,"","construct",__construct,sizeof(void *)},
	[4 ] = {ENTRY_TYPE_FUNC_POINTER,"","deconstruct",__deconstrcut,sizeof(void *)},
	[5 ] = {ENTRY_TYPE_VFUNC_POINTER,"","init_window",__init_window,sizeof(void *)},
	[6 ] = {ENTRY_TYPE_VFUNC_POINTER,"","close_window",__close_window,sizeof(void *)},
	[7 ] = {ENTRY_TYPE_VFUNC_POINTER,"","update_window",__update_window,sizeof(void *)},
	[8 ] = {ENTRY_TYPE_VFUNC_POINTER,"","draw_image",__draw_image,sizeof(void *)},
	[9 ] = {ENTRY_TYPE_VFUNC_POINTER,"","render_create",__render_create,sizeof(void *)},
	[10] = {ENTRY_TYPE_VFUNC_POINTER,"","render_destroy",__render_destroy,sizeof(void *)},
	[11] = {ENTRY_TYPE_VFUNC_POINTER,"","render_set_color",__render_set_color,sizeof(void *)},
	[12] = {ENTRY_TYPE_VFUNC_POINTER,"","render_clear",__render_clear,sizeof(void *)},
	[13] = {ENTRY_TYPE_VFUNC_POINTER,"","render_draw_line",__render_draw_line,sizeof(void *)},
	[14] = {ENTRY_TYPE_VFUNC_POINTER,"","render_fill_rect",__render_fill_rect,sizeof(void *)},
	[15] = {ENTRY_TYPE_VFUNC_POINTER,"","render_draw_image",__render_draw_image,sizeof(void *)},
	[16] = {ENTRY_TYPE_VFUNC_POINTER,"","render_load_image",__render_load_image,sizeof(void *)},
	[17] = {ENTRY_TYPE_VFUNC_POINTER,"","render_load_text",__render_load_text,sizeof(void *)},
	[18] = {ENTRY_TYPE_VFUNC_POINTER,"","render_write_text",__render_write_text,sizeof(void *)},
	[19] = {ENTRY_TYPE_VFUNC_POINTER,"","render_present",__render_present,sizeof(void *)},
	[20] = {ENTRY_TYPE_STRING,"char","name",NULL,0},
	[21] = {ENTRY_TYPE_END},

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


