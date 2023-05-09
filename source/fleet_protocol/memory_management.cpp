#include <memory_management.h>

#include <cstdlib>

void clear_buffer(struct buffer* buffer_pointer) {
	free(buffer_pointer->data);
	buffer_pointer->data = nullptr;
	buffer_pointer->size_in_bytes = 0;
}