#ifndef HIGHLOAD_SERVER_FILEREADER_H
#define HIGHLOAD_SERVER_FILEREADER_H

#include <sys/mman.h>
#include <sys/stat.h>

// пришлось сделать обертку над си, может и с обычными потоками даже быстрее будет)

class FileReader {
public:
    explicit FileReader(const std::string& filepath) {
        file = fopen(filepath.data(), "r");
        if (!file) {
            return;
        }
        need_close = true;
        int descr = fileno(file);

        struct stat st;
        if (fstat(descr, &st) != 0) {
            return;
        }

        size_of_file = st.st_size;
        file_ptr = static_cast<char*>(mmap(nullptr, st.st_size, PROT_READ, MAP_PRIVATE, descr, 0));
        if (file_ptr == MAP_FAILED) {
            return;
        }

        need_munmap = true;
    }

    bool IsInit() const {
        return need_close && need_munmap;
    }

    size_t GetSize() const {
        return size_of_file;
    }

    void ShiftPtr(size_t offset) {
        // если оффсет больше, чем нужно, задвигаем в конец
        if (m_offset + offset >= size_of_file) {
            file_ptr = file_ptr - m_offset + size_of_file - 1;
            return;
        }

        file_ptr = file_ptr + m_offset + offset;
    }

    const char* GetPtr() const {
        return file_ptr;
    }

    bool IsOpen() {
        return need_close;
    }

    bool IsMap() {
        return need_munmap;
    }

    ~FileReader() {
        if (need_munmap) {
            munmap(file_ptr, size_of_file);
        }

        if (need_close) {
            fclose(file);
        }
    }

private:
    bool need_close = false;
    bool need_munmap = false;
    size_t m_offset = 0;
    size_t size_of_file;

    FILE* file;
    char* file_ptr;
};

#endif //HIGHLOAD_SERVER_FILEREADER_H
