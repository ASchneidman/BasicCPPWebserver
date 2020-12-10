#pragma once

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <iostream>

#define MAX_INT_LENGTH 10
#define TYPE_LEN 10
#define MAX_BUFFER_LENGTH 3000

char *header = "HTTP/1.1 200 OK\nContent-Type: %s\nContent-Length: %u\n\n";
char *type_str = "text/plain";

namespace Http {

enum ContentType {TEXT};

enum Method {GET};

class HttpRequest {


    public:
        enum ReadStatus {SUCCESS, FAIL};

        ReadStatus accept( int socket ) {
            char *buffer = new char[MAX_BUFFER_LENGTH];
            this->request_length = read(socket, buffer, MAX_BUFFER_LENGTH);
            this->request = std::string(buffer);
            return readRequestData();
        }

        const std::string &toString() const {
            return this->request;
        }

        const Method &getMethod() const {
            return this->method;
        }

        const std::string &getLoc() const {
            return this->loc;
        }

    friend std::ostream& operator<<(std::ostream& os, const HttpRequest& req);

    private:
        int request_length;
        Method method;
        std::string loc;
        std::string request;

        ReadStatus readRequestData() {
            size_t pos = this->request.find(' ');
            if (pos == std::string::npos) {
                return FAIL;
            }
            std::string m = this->request.substr(0, pos);

            if (m == "GET") {
                method = GET;
            } else {
                return FAIL;
            }

            size_t next_pos = this->request.find(' ', pos+1);
            if (next_pos == std::string::npos) {
                return FAIL;
            }
            loc = this->request.substr(pos+1, next_pos - (pos+1));
        }
};

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


std::ostream& operator<<(std::ostream& os, Http::HttpRequest& req) {
    os << req.toString();
    return os;
}