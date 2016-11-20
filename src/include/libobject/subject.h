#ifndef __SUBJECT_H__
#define __SUBJECT_H__

#include <libobject/object.h>

typedef struct subject_s{
	obj_t obj;

	/*normal methods*/
	int (*construct)(struct subject_s *subject,char *init_str);
	int (*deconstruct)(struct subject_s *subject);
	int (*set)(struct subject_s *subject, char *attrib, void *value);

	/*virtual methods*/
	int (*move)(struct subject_s *subject);
	int (*show)(struct subject_s *subject);
	int (*add_x_speed)(struct subject_s *subject, float v);
	int (*add_y_speed)(struct subject_s *subject,float v);
	int (*is_touching)(struct subject_s *me,struct subject_s *subject);

	int x, y, height, width;
	float x_speed, y_speed;
	
}subject_t;


#endif
