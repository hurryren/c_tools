#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <math.h>
#include <errno.h>
#include <string.h>

#include "leptjson.h"

#ifndef LEPT_PARSE_STACK_INIT_SIZE
#define LEPT_PARSE_STACK_INIT_SIZE 256
#endif

#define EXPECT(c, ch) do {assert(*c->json == (ch)); c->json++;}while(0)
#define ISDIGIT(ch)         ((ch) >= '0' && (ch) <= '9')
#define ISDIGIT1TO9(ch)     ((ch) >= '1' && (ch) <= '9')
#define PUTC(c, ch)  do { *(char*)lept_context_push(c,sizeof(char)) = (ch);}while(0)

typedef struct {
    const char *json;
    char *stack;
    size_t size;
    size_t top;
} lept_context;

static void *lept_context_push(lept_context *c, size_t size) {
    void *ret;
    assert(size > 0);
    if (c->top + size >= c->size) {
        if (c->size == 0)
            c->size = LEPT_PARSE_STACK_INIT_SIZE;
        while (c->top + size >= c->size) {
            c->size += c->size >> 1; /* c->size * 1.5 */
        }
        c->stack = (char *) realloc(c->stack, c->size);
    }
    ret = c->stack + c->top;
    c->top += size;
    return ret;
}

static void *lept_context_pop(lept_context *c, size_t size) {
    assert(c->top >= size);
    return c->stack + (c->top -= size);
}


static void lept_parse_whitespace(lept_context *c) {
    const char *p = c->json;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
        p++;
    c->json = p;
}

static int lept_parse_literal(lept_context *c, lept_value *v, const char *literal, lept_type type) {
    size_t i;
    EXPECT(c, literal[0]);

    for (i = 0; literal[i + 1]; i++) {
        if (c->json[i] != literal[i + 1])
            return LEPT_PARSE_INVALID_VALUE;
    }
    c->json += i;
    v->type = type;
    return LEPT_PARSE_OK;
}

static int lept_parse_number(lept_context *c, lept_value *v) {
    const char *p = c->json;
/*
    if (ISDIGIT(*p) == 0 && *p != '-')
        return LEPT_PARSE_INVALID_VALUE;
    else {
        if (*p == '0' && (*(p + 1)!='\0' && *(p + 1) != '.' && *(p + 1) != 'e' && *(p + 1) != 'E')) {
            return LEPT_PARSE_ROOT_NOT_SINGULAR;
        }

        for(*p;*(p+1)!='\0';p++){
        }
        if (*p == '.')
            return LEPT_PARSE_INVALID_VALUE;

        p++;
    }
*/
    if (*p == '-') p++;
    if (*p == '0') p++;
    else {
        if (!ISDIGIT1TO9(*p)) return LEPT_PARSE_INVALID_VALUE;
        for (p++; ISDIGIT(*p); p++);
    }
    if (*p == '.') {
        p++;
        if (!ISDIGIT(*p)) return LEPT_PARSE_INVALID_VALUE;
        for (p++; ISDIGIT(*p); p++);
    }
    if (*p == 'e' || *p == 'E') {
        p++;
        if (*p == '+' || *p == '-') p++;
        if (!ISDIGIT(*p)) return LEPT_PARSE_INVALID_VALUE;
        for (p++; ISDIGIT(*p); p++);
    }
    errno = 0;
    v->u.n = strtod(c->json, NULL);
    if (errno == ERANGE && (v->u.n == HUGE_VAL || v->u.n == -HUGE_VAL))
        return LEPT_PARSE_NUMBER_TOO_BIG;
    c->json = p;
    v->type = LEPT_NUMBER;
    return LEPT_PARSE_OK;
}

static int lept_parse_string(lept_context *c, lept_value *v) {
    size_t head = c->top;
    size_t len;
    const char *p;

    EXPECT(c, '\"');
    p = c->json;
    for (;;) {
        char ch = *p++;
        switch (ch) {
            case '\"':
                len = c->top - head;
                lept_set_string(v, (const char *) lept_context_pop(c, len), len);
                c->json = p;
                return LEPT_PARSE_OK;
            case '\0':
                c->top = head;
                return LEPT_PARSE_MISS_QUOTATION_MARK;
            default:
                PUTC(c, ch);
        }
    }
}

static int lept_parse_value(lept_context *c, lept_value *v) {
    switch (*c->json) {
        case 'n' :
            return lept_parse_literal(c, v, "null", LEPT_NULL);
        case 't' :
            return lept_parse_literal(c, v, "true", LEPT_TRUE);
        case 'f' :
            return lept_parse_literal(c, v, "false", LEPT_FALSE);
        case '\0':
            return LEPT_PARSE_EXPECT_VALUE;
        case '"':
            return lept_parse_string(c, v);
        default:
            return lept_parse_number(c, v);
    }
}

int lept_parse(lept_value *v, const char *json) {
    lept_context c;
    int ret;

    assert(v != NULL);

    c.json = json;
    c.stack = NULL;
    c.size = c.top = 0;

    lept_init(v);
    lept_parse_whitespace(&c);
    if ((ret = lept_parse_value(&c, v)) == LEPT_PARSE_OK) {
        lept_parse_whitespace(&c);
        if (*c.json != '\0') {
            v->type = LEPT_NULL;
            ret = LEPT_PARSE_ROOT_NOT_SINGULAR;
        }
    }
    assert(c.top == 0);
    free(c.stack);
    return ret;
}

void lept_free(lept_value *v) {
    assert(v != NULL);

    if (v->type == LEPT_STRING)
        free(v->u.s.s);
    v->type = LEPT_NULL;
}


lept_type lept_get_type(const lept_value *v) {
    assert(v != NULL);
    return v->type;
}


int lept_get_boolean(const lept_value *v) {
    assert(v != NULL && (v->type == LEPT_FALSE || v->type == LEPT_TRUE));

    if (v->type == LEPT_FALSE)
        return 0;
    if (v->type == LEPT_TRUE)
        return 1;

    return 0;
}

void lept_set_boolean(lept_value *v, int b) {
    assert(v != NULL && (b==0 || b==1));

    if(b==0)
        v->type == LEPT_FALSE;
    if(b==1)
        v->type == LEPT_TRUE;
}


double lept_get_number(const lept_value *v) {
    assert(v != NULL && v->type == LEPT_NUMBER);
    return v->u.n;
}

void lept_set_number(lept_value *v, double n) {
    assert(v != NULL);
    v->type = LEPT_NUMBER;
    v->u.n = n;
}

const char *lept_get_string(const lept_value *v) {
    assert(v != NULL && v->type == LEPT_STRING);
    return v->u.s.s;
}

size_t lept_get_string_length(const lept_value *v) {
    assert(v != NULL && v->type == LEPT_STRING);
    return v->u.s.len;
}

void lept_set_string(lept_value *v, const char *s, size_t len) {
    assert(v != NULL && (s != NULL || len == 0));
    lept_free(v);
    v->u.s.s = (char *) malloc(len + 1);
    memcpy(v->u.s.s, s, len);
    v->u.s.s[len] = '\0';
    v->u.s.len = len;
    v->type = LEPT_STRING;
}

