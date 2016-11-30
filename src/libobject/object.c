/**
 * @file object.c
 * @synopsis 
 * @author alan(a1an1in@sina.com)
 * @version 1
 * @date 2016-11-21
 */
#include <stdio.h>
#include <libdbg/debug.h>
#include <libobject/object.h>

void * object_get_func_pointer(void *class_info_addr, char *func_pointer_name)
{
	class_info_entry_t *entry = (class_info_entry_t *)class_info_addr;
	int i;

	for(i = 0; entry[i].type != ENTRY_TYPE_END; i++) {
		if(		strcmp((char *)entry[i].value_name, func_pointer_name) == 0 && 
				entry[i].type == ENTRY_TYPE_FUNC_POINTER) {
			return entry[i].value;
		}
	}	

	return 0;
}

class_info_entry_t * object_get_entry_of_parent_class(void *class_info_addr)
{
	class_info_entry_t *entry = (class_info_entry_t *)class_info_addr;
	int i;

	for(i = 0; entry[i].type != ENTRY_TYPE_END; i++) {
		if( entry[i].type == ENTRY_TYPE_OBJ) {
			return &entry[i];
		}
	}	

	return NULL;
}

int object_init_func_pointer(void *obj,void *class_info_addr)
{
	class_info_entry_t *entry = (class_info_entry_t *)class_info_addr;
	int i;
	int (*set)(void *obj, char *attrib, void *value);

 	set = object_get_func_pointer(class_info_addr,"set");
	if(set == NULL) {
		dbg_str(OBJ_WARNNING,"obj_init_func_pointer,set func is NULL");
		return -1;
	}

	for(i = 0; entry[i].type != ENTRY_TYPE_END; i++) {
        if(     entry[i].type == ENTRY_TYPE_FUNC_POINTER || 
                entry[i].type == ENTRY_TYPE_VIRTUAL_FUNC_POINTER)
        {
            /*
             *dbg_str(OBJ_DETAIL,"value_name %s, value %p",
             *        entry[i].value_name,
             *        entry[i].value);
             */

            if(entry[i].value != NULL)
                set(obj, (char *)entry[i].value_name, entry[i].value);
        }
	}	

	return 0;
}

int object_inherit_parent_methods(void *obj,void *class_info,void *parent_class_info)
{
	class_info_entry_t *entry = (class_info_entry_t *)class_info;
	class_info_entry_t *entry_parent = (class_info_entry_t *)parent_class_info;
    void *(*get)(void *obj, char *attrib);
	int (*set)(void *obj, char *attrib, void *value);
	int i;
    void *method;

    if(parent_class_info == NULL) return;

    /*
     *dbg_str(DBG_DETAIL,"current obj type name =%s, parent_class name:%s",entry->type_name,entry_parent->type_name);
     */
 	set = object_get_func_pointer(class_info,"set");
 	get = object_get_func_pointer(parent_class_info,"get");
	if(set == NULL || get == NULL) {
		dbg_str(OBJ_WARNNING,"obj_init_func_pointer,set func is NULL");
		return -1;
	}

	for(i = 0; entry[i].type != ENTRY_TYPE_END; i++) {
        if(entry[i].type == ENTRY_TYPE_FUNC_POINTER)
        {
            method = get(obj, (char *)entry[i].value_name);
            if(method != NULL)
                set(obj, (char *)entry[i].value_name, method);
        }
	}	
}

void *
object_find_reimplement_func_pointer(char *method_name,
									 char *start_type_name,
									 char *end_type_name)
{
	class_info_entry_t *entry;
	object_deamon_t *deamon;
	char *subclass_name = NULL;
	int i;

	if(strcmp(start_type_name,end_type_name) == 0) return NULL;
	if(start_type_name == NULL) {
		dbg_str(OBJ_WARNNING,"object_find_reimplement_func_pointer, start addr is NULL");
		return NULL;
	}

	deamon = object_deamon_get_global_object_deamon();
	entry  = (class_info_entry_t *)object_deamon_search_class(deamon,
															  (char *)start_type_name);
	if(entry[0].type == ENTRY_TYPE_OBJ) {
		subclass_name = entry[0].type_name;
	}
	for(i = 0; entry[i].type != ENTRY_TYPE_END; i++) {
		if( 	entry[i].type == ENTRY_TYPE_FUNC_POINTER &&
				strcmp(entry[i].value_name, method_name) == 0)
		{
			return entry[i].value;
		}

	}	

	return object_find_reimplement_func_pointer(method_name,
										        subclass_name,
										        end_type_name);
}

int object_cover_vitual_func_pointer(void *obj,
									 char *cur_type_name,
									 char *type_name)
{
	class_info_entry_t *entry;
	object_deamon_t *deamon;
	int i;
	int (*set)(void *obj, char *attrib, void *value);
	void *reimplement_func;

	if(strcmp(cur_type_name,type_name) == 0) return 0;

	deamon = object_deamon_get_global_object_deamon();
	entry  = (class_info_entry_t *)
             object_deamon_search_class(deamon, (char *)cur_type_name);

 	set    = object_get_func_pointer(entry,"set");
	if(set == NULL) {
		dbg_str(OBJ_WARNNING,"obj_init_func_pointer,set func is NULL");
		return -1;
	}

	for(i = 0; entry[i].type != ENTRY_TYPE_END; i++) {
		if(entry[i].type == ENTRY_TYPE_VIRTUAL_FUNC_POINTER){
			reimplement_func = object_find_reimplement_func_pointer(entry[i].value_name,
																	type_name,
																	cur_type_name);
            if(reimplement_func != NULL)
                set(obj, (char *)entry[i].value_name, reimplement_func);
		}
	}	

	return 0;
}

static int __object_set(void *obj,
                        cjson_t *c,
	                    int (*set)(void *obj, char *attrib, void *value)) 
{
	object_deamon_t *deamon;
	void *class_info_addr;
    cjson_t *next;
    cjson_t *object;
    int (*sub_set)(void *obj, char *attrib, void *value); 

    while (c) {
        if(c->type & CJSON_OBJECT) {
            object = c;
            if(object->string) {
                dbg_str(OBJ_DETAIL,"object name:%s",object->string);
                deamon          = object_deamon_get_global_object_deamon();
                class_info_addr = object_deamon_search_class(deamon,object->string);
                sub_set         = object_get_func_pointer(class_info_addr,"set");
            }

            if (c->child) {
                __object_set(obj,c->child, sub_set);
            }
        } else {
            if(set) {
                /*
                 *dbg_str(OBJ_DETAIL,"object name %s,set %s",object->string, c->string);
                 */
                if(c->type & CJSON_NUMBER) {
                    set(obj,c->string,&(c->valueint));
                } else if(c->type & CJSON_STRING) {
                    set(obj,c->string,c->valuestring);
                }
            }
        }

        c = c->next;
    }

}
int object_set(void *obj, char *type_name, char *set_str) 
{
    cjson_t *root;

    dbg_str(OBJ_DETAIL,"%s",set_str);

    root = cjson_parse(set_str);
    __object_set(obj, root,NULL);
    cjson_delete(root);

    return 0;
}

int __object_dump(void *obj, char *type_name, cjson_t *object) 
{
	object_deamon_t *deamon;
	class_info_entry_t *entry;
    void *(*get)(void *obj, char *attrib);
    int len;
    int i;
    cjson_t *item;
    void *value;
    char *name;

	deamon = object_deamon_get_global_object_deamon();
	entry  = (class_info_entry_t *)object_deamon_search_class(deamon,(char *)type_name);
    get    = object_get_func_pointer(entry,(char *)"get");
    if(get == NULL) {
        dbg_str(OBJ_WARNNING,"get func pointer is NULL");
        return -1;
    }

	for(i = 0; entry[i].type != ENTRY_TYPE_END; i++) {
		if(entry[i].type == ENTRY_TYPE_OBJ){
            item = cjson_create_object();
            cjson_add_item_to_object(object, entry[i].type_name, item);
            __object_dump(obj, entry[i].type_name, item);
        } else if(entry[i].type == ENTRY_TYPE_FUNC_POINTER || 
                  entry[i].type == ENTRY_TYPE_VIRTUAL_FUNC_POINTER) 
        {
		} else {
            value = get(obj,entry[i].value_name);
            if(value == NULL) continue;
            name = entry[i].value_name;
            if(entry[i].type == ENTRY_TYPE_INT8_T || entry[i].type == ENTRY_TYPE_UINT8_T){
                cjson_add_number_to_object(object, name, *((char *)value));
            } else if(entry[i].type == ENTRY_TYPE_INT16_T || entry[i].type == ENTRY_TYPE_UINT16_T) {
                cjson_add_number_to_object(object, name, *((short *)value));
            } else if(entry[i].type == ENTRY_TYPE_INT32_T || entry[i].type == ENTRY_TYPE_UINT32_T) {
                cjson_add_number_to_object(object, name, *((int *)value));
            } else if(entry[i].type == ENTRY_TYPE_INT64_T || entry[i].type == ENTRY_TYPE_UINT64_T) {
            } else if(entry[i].type == ENTRY_TYPE_STRING) {
                cjson_add_string_to_object(object, name, (char *)value);
            } else if(entry[i].type == ENTRY_TYPE_NORMAL_POINTER ||
                    entry[i].type == ENTRY_TYPE_FUNC_POINTER || 
                    entry[i].type == ENTRY_TYPE_VIRTUAL_FUNC_POINTER ||
                    entry[i].type == ENTRY_TYPE_OBJ_POINTER) 
            {
                unsigned long long d = (unsigned long long) value;
                cjson_add_number_to_object(object, name,d);
            } else {
                dbg_str(OBJ_WARNNING,"type error,please check");
            }
        }
	}	
}

int object_dump(void *obj, char *type_name, char *buf, int max_len) 
{
    cjson_t *root;
    cjson_t *item;
    char *out;
    int len;

    root = cjson_create_object();
    item = cjson_create_object();
    cjson_add_item_to_object(root, type_name, item);

    __object_dump(obj, type_name, item);

    out = cjson_print(root);
    len = strlen(out);
    len = len > max_len ? max_len: len; 
    strncpy(buf,out,len);

    strncpy(buf,out,max_len);
    cjson_delete(root);
    free(out);
}

int __object_init(void *obj, char *cur_type_name, char *type_name) 
{
	object_deamon_t *deamon;
	void *class_info, *parent_class_info;
 	class_info_entry_t * entry_of_parent_class;
	int (*construct)(void *obj,char *init_str);

	dbg_str(OBJ_DETAIL,"current obj type name =%s",cur_type_name);

	deamon                = object_deamon_get_global_object_deamon();
	class_info            = object_deamon_search_class(deamon,(char *)cur_type_name);
	construct             = object_get_func_pointer(class_info,"construct");
 	entry_of_parent_class = object_get_entry_of_parent_class(class_info);

	dbg_str(OBJ_DETAIL,"obj_class addr:%p",class_info);
 	if(entry_of_parent_class != NULL) {
        /*
		 *dbg_str(OBJ_DETAIL,"init subclass");
         */
		__object_init(obj, entry_of_parent_class->type_name, type_name);
 	} else {
		dbg_str(OBJ_DETAIL,"obj has not subclass");
	}

    /*
	 *dbg_str(DBG_DETAIL,"current obj type name =%s",cur_type_name);
     */
    /*
     *if(entry_of_parent_class != NULL){
     *    dbg_str(DBG_DETAIL,"current obj type name =%s, parent_class name:%s",cur_type_name,entry_of_parent_class->type_name);
     *} else {
     *    dbg_str(DBG_DETAIL,"current obj type name =%s, parent_class name:%p",cur_type_name,entry_of_parent_class);
     *}
     */

    /*
     *if(entry_of_parent_class != NULL) {
     *    parent_class_info = object_deamon_search_class(deamon,entry_of_parent_class->type_name);
     *    object_inherit_parent_methods(obj,class_info,parent_class_info);
     *}
     */
	object_init_func_pointer(obj,class_info);
	object_cover_vitual_func_pointer(obj, cur_type_name, type_name);

	dbg_str(OBJ_DETAIL,"obj addr:%p",obj);
	construct(obj,NULL);

	return 0;
}

int object_init(void *obj, char *type_name) 
{
	__object_init(obj, type_name, type_name);
}

