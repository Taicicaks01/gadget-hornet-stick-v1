#include "FileExplorer.h"
#include <LittleFS.h>
#include <Arduino.h>

FileExplorerClass fileExplorer;

bool FileExplorerClass::begin() {
    if (!LittleFS.begin()) return false;
    return true;
}

int FileExplorerClass::listDir(const char* path, DirEntryCallback cb, void* ctx) {
    Dir dir = LittleFS.openDir(path);
    int count = 0;
    while (dir.next()) {
        String name = dir.fileName();
        size_t sz = dir.fileSize();
        bool isDir = dir.isDirectory();
        if (cb) cb(name.c_str(), sz, isDir, ctx);
        count++;
    }
    return count;
}

bool FileExplorerClass::deleteFile(const char* path) {
    if (!LittleFS.exists(path)) return false;
    return LittleFS.remove(path);
}

bool FileExplorerClass::fileInfo(const char* path, size_t& outSize, bool& outIsDir) {
    if (!LittleFS.exists(path)) return false;
    File f = LittleFS.open(path, "r");
    if (!f) return false;
    outSize = f.size();
    outIsDir = false;
    f.close();
    return true;
}

int FileExplorerClass::readLine(File& f, char* buf, size_t bufLen) {
    if (!f || !f.available()) return 0;
    size_t i = 0;
    while (f.available() && i < bufLen - 1) {
        int c = f.read();
        if (c < 0) break;
        buf[i++] = (char)c;
        if (c == '\n') break;
    }
    buf[i] = '\0';
    return (int)i;
}
