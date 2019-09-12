#pragma once

// If true I should be using raw pointers instead of shared_ptr. In modern C++ we should be using
// shared_ptr or unique_ptr but I wanna learn how to manage memory by myself.
#define MANUAL_MEM_MANAGEMENT true

// Trace all function calls
#define TRACE true

#define TRACE_BYPASS_QUEUE true

// cout when an object is destroyed
#define PRINT_DESTRUCTORS true
