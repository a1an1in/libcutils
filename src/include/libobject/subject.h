#ifndef __SUBJECT_H__
#define __SUBJECT_H__

#include <libobject/object.h>

typedef struct subject_s Subject;
struct subject_s{
	Obj obj;

	/*normal methods*/
	int (*construct)(Subject *subject,char *init_str);
	int (*deconstruct)(Subject *subject);
	int (*set)(Subject *subject, char *attrib, void *value);

	/*virtual methods*/
	int (*move)(Subject *subject);
	int (*show)(Subject *subject);
	int (*add_x_speed)(Subject *subject, float v);
	int (*add_y_speed)(Subject *subject,float v);
	int (*is_touching)(Subject *me,Subject *subject);

	int x, y, height, width;
	float x_speed, y_speed;
};

#endif
