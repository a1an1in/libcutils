#include <stdio.h>
#include <inttypes.h>

#include <libdbg/debug.h>
#include "libblob/msgblob.h"

static const char *indent_str = "\t\t\t\t\t\t\t\t\t\t\t\t\t";

#define indent_printf(indent, ...) do { \
	if (indent > 0) \
		fwrite(indent_str, indent, 1, stderr); \
	fprintf(stderr, __VA_ARGS__); \
} while(0)

static void
dump_table(struct blob_attr *head, int len, int indent, bool array);

static void dump_attr_data(struct blob_attr *data, int indent, int next_indent)
{
	int type = msgblob_type(data);
	switch(type) {
	case MSGBLOB_TYPE_STRING:
		indent_printf(indent, "%s\n", msgblob_get_string(data));
		break;
	case MSGBLOB_TYPE_INT8:
		indent_printf(indent, "%d\n", msgblob_get_u8(data));
		break;
	case MSGBLOB_TYPE_INT16:
		indent_printf(indent, "%d\n", msgblob_get_u16(data));
		break;
	case MSGBLOB_TYPE_INT32:
		indent_printf(indent, "%d\n", msgblob_get_u32(data));
		break;
	case MSGBLOB_TYPE_INT64:
		indent_printf(indent, "%"PRIu64"\n", msgblob_get_u64(data));
		break;
	case MSGBLOB_TYPE_TABLE:
	case MSGBLOB_TYPE_ARRAY:
        /*
         *if (!indent)
		 *    indent_printf(indent, "\n");
         */
		dump_table(msgblob_data(data), msgblob_data_len(data),
			   next_indent, type == MSGBLOB_TYPE_ARRAY);
		break;
	}
}
static void
dump_table(struct blob_attr *head, int len, int indent, bool array)
{
	struct blob_attr *attr;
	struct msgblob_hdr *hdr;

	indent_printf(0, "{\n");
	__blob_for_each_attr(attr, head, len) {
		hdr = blob_data(attr);
        if (!array)
			indent_printf(indent + 1, "%s:", hdr->name);
        else
            fwrite(indent_str, indent + 1, 1, stderr); \
		dump_attr_data(attr, 0, indent + 1);
	}
	indent_printf(indent, "}\n");
}


enum {
	FOO_MESSAGE,
	FOO_LIST,
	FOO_TESTDATA
};

static const struct msgblob_policy pol[] = {
	[FOO_MESSAGE] = {
		.name = "message",
		.type = MSGBLOB_TYPE_STRING,
	},
	[FOO_LIST] = {
		.name = "list",
		.type = MSGBLOB_TYPE_ARRAY,
	},
	[FOO_TESTDATA] = {
		.name = "testdata",
		.type = MSGBLOB_TYPE_TABLE,
	},
};

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

static void dump_message(struct blob_buf *buf)
{
	struct blob_attr *tb[ARRAY_SIZE(pol)];

	if (msgblob_parse(pol, ARRAY_SIZE(pol), tb, blob_data(buf->head), blob_len(buf->head)) != 0) {
		fprintf(stderr, "Parse failed\n");
		return;
	}
	if (tb[FOO_MESSAGE])
		fprintf(stderr, "Message: %s\n", (char *) msgblob_data(tb[FOO_MESSAGE]));

	if (tb[FOO_LIST]) {
		fprintf(stderr, "List: ");
		dump_table(msgblob_data(tb[FOO_LIST]), msgblob_data_len(tb[FOO_LIST]), 0, true);
	}
	if (tb[FOO_TESTDATA]) {
		fprintf(stderr, "Testdata: ");
		dump_table(msgblob_data(tb[FOO_TESTDATA]), msgblob_data_len(tb[FOO_TESTDATA]), 0, false);
	}
}

static void
fill_message(struct blob_buf *buf)
{
	void *tbl,*tbl2;

	msgblob_add_string(buf, "message", "Hello, world!");

	tbl = msgblob_open_table(buf, "testdata");
    {
        tbl2 = msgblob_open_array(buf, "inner array");
        msgblob_add_u32(buf, NULL, 0);
        msgblob_add_u32(buf, NULL, 1);
        msgblob_add_u32(buf, NULL, 2);
        msgblob_close_table(buf, tbl2);
    }
	msgblob_add_u32(buf, "count", 1);
	msgblob_add_string(buf, "test", "abcdef");

	msgblob_close_table(buf, tbl);

	tbl = msgblob_open_array(buf, "list");
	msgblob_add_u32(buf, NULL, 0);
	msgblob_add_u32(buf, NULL, 1);
	msgblob_add_u32(buf, NULL, 2);
	msgblob_close_table(buf, tbl);
}

int test_msgblob()
{
	static struct blob_buf blob_buf;

	msgblob_buf_init(&blob_buf);
	fill_message(&blob_buf);
	dump_message(&blob_buf);

	if (blob_buf.buf)
		free(blob_buf.buf);

	return 0;
}
