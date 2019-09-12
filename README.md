# WARNING

This is a buggy RAT, not completed, not stable. It still a "work in progress" application.

# Introduction

Project created to kill some of my free time, It is not actively maintained.
There are a lot of things to improve/fix, and it will take time to reach a stability on this project.

# Features

- Reverse Shell
- List Process
- Desktop streaming
- FileSystem
- Download File

# Understanding the project

IThreadChannels are means to communicate two threads in an synchronous manner.
I use them as the atomic object shared by two threads for communication, in this app
I want two way communication so I need two channels, this is I created the IDoubleThreadChannel.
Synchronous means they have to be called by the communicating threads to receive events as needed.
Example, the UI thread will call getFromApp() and block there to receive App events.

Communicators in the other hand they manage their own threading and trigger callbacks asynchronously.
You don't call them, they call you.

# Guidelines

- Packet objects should be deleted by NetServerService
- Client pointers should be deleted by Application.

# Macros

- SHOW_CONSOLE If a console should be shown for debugging purposes.
- MANUAL_MEMORY_MANAGEMENT If true I try to manage the memory I allocate(for learning), If false, then
  I will be using shared_ptr instead to make memory management a lot more easy.

# TODOS

- Desktop streaming feature not finished(break out of infinite loop).
- Get ride of errors mostly conversion issues
- Packet header must send 8 bytes for the packet length not int32_t, so change it to uint64_t
- Use Read/Write Locks https://docs.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-acquiresrwlockexclusive
- The dataLength field in Buffer class is incorrect, should be improved
- The issue of Event objects having a void* which should not be deleted through delete void*
  can be solved either as I did, the consumer of the event, takes care of casting it to the expected value
  and deleting it, or, using class templates such as new AppEvent<string> then this could be delete object\*
- Buffer class should throw exceptions on illegal operations.
- Encryption
- Camera Streaming
- Error Handling .... everywhere.
