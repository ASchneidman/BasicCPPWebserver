#pragma once

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

#define MAX_INT_LENGTH 10
#define TYPE_LEN 10

char *header = "HTTP/1.1 200 OK\nContent-Type: %s\nContent-Length: %u\n\n";
char *type_str = "text/plain";

namespace Http {

enum ContentType {TEXT};

class HttpResponse {
    public:
        HttpResponse( size_t max_length ) {
            this->max_length = max_length + TYPE_LEN + MAX_INT_LENGTH;
            this->buffer = new char[this->max_length];
            this->max_message_length = this->max_length - TYPE_LEN - MAX_INT_LENGTH;
            this->message_length = 0;
        }
        ~HttpResponse() {
            delete[] this->buffer;
        }
        void setStatus( ContentType type ) {
            this->type = type;
        }
        void setContent( char *content, size_t length ) {
            if (length >= this->max_message_length) {
                return;
            }
            memset(this->buffer, 0, this->max_length);
            int written = std::snprintf(this->buffer, this->max_length, header, type_str, length);
            // TODO: DEAL WITH WRITING MORE THAN BUFFER 
            memcpy(&this->buffer[written], content, length);
            this->message_length = written + length;
        }
        void send( int socket ) {
            write(socket , this->buffer , this->message_length);
        }
    private:
        ContentType type;
        char *buffer;
        size_t max_length;
        size_t max_message_length;
        size_t message_length;
};

}