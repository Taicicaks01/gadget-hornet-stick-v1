#pragma once
#include <Arduino.h>
#include <LittleFS.h>

typedef void (*DirEntryCallback)(const char* name, size_t size, bool isDir, void* ctx);

class FileExplorerClass {
public:
    bool begin();
    int listDir(const char* path, DirEntryCallback cb, void* ctx=nullptr);
    bool deleteFile(const char* path);
    bool fileInfo(const char* path, size_t& outSize, bool& outIsDir);
    // Read next line from file; returns bytes read, 0 on EOF, -1 on error
    int readLine(File& f, char* buf, size_t bufLen);
};

extern FileExplorerClass fileExplorer;
