#define ARRAY(type, type_lower) \
ARRAY_STRUCT(type) \
ARRAY_FUNCS(type, type_lower)
#define ARRAY_STRUCT(type) \
typedef struct type##Arr { \
	type* buf; \
	int len; \
	int cap; \
} type##Arr;
#define ARRAY_FUNCS(type, type_lower) \
type##Arr new_##type_lower##_arr (int cap) { \
	return (type##Arr){ .buf = malloc(cap * sizeof(type)), .len = 0, .cap = cap }; \
} \
void try_resize_##type_lower##_arr (type##Arr* arr) { \
	if (arr->len >= arr->cap) { \
		arr->cap *= 2; \
		arr->buf = realloc(arr->buf, sizeof(type) * arr->cap); \
	} \
} \
type* push_##type_lower##_arr (type##Arr* arr, type item) { \
	arr->len++; \
	try_resize_##type_lower##_arr(arr); \
	arr->buf[arr->len - 1] = item; \
	return arr->buf + arr->len - 1; \
} \
void push_front_##type_lower##_arr (type##Arr* arr, type item) { \
	arr->len++; \
	try_resize_##type_lower##_arr(arr); \
	memmove(arr->buf + 1, arr->buf, (arr->len - 1) * sizeof(type)); \
	arr->buf[0] = item; \
} \
type pop_##type_lower##_arr (type##Arr* arr) { \
	if (arr->len == 0) { \
		printf("Can't pop from empty array!"); \
		exit(1); \
	} \
	arr->len--; \
	return arr->buf[arr->len]; \
}
