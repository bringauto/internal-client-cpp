#include <fleet_protocol/common_headers/memory_management.h>
#include <fleet_protocol/common_headers/general_error_codes.h>

#include <cstdlib>



int allocate(struct buffer *buffer_pointer, size_t size_in_bytes) {
	if ((buffer_pointer->data = malloc(size_in_bytes)) == nullptr) {
		return NOT_OK;
	}
	buffer_pointer->size_in_bytes = size_in_bytes;
	return OK;

}

void deallocate(struct buffer* buffer_pointer) {
	free(buffer_pointer->data);
	buffer_pointer->data = nullptr;
	buffer_pointer->size_in_bytes = 0;
}