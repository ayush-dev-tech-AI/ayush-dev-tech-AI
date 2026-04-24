#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define MAX 100

// queue
char tasks[MAX][100];
int front = 0, rear = -1;

// enqueue
void addTask(char *task) {
    if (rear < MAX - 1) {
        rear++;
        strcpy(tasks[rear], task);
    }
}

// dequeue
char* processTask() {
    if (front <= rear) {
        return tasks[front++];
    }
    return "Queue empty";
}

// list tasks
void listTasks(char *response) {
    strcpy(response, "");
    for (int i = front; i <= rear; i++) {
        strcat(response, tasks[i]);
        strcat(response, "\\n");
    }
}

// handle request
void handleRequest(int client_socket, char *buffer) {
    char response[2048];

    if (strstr(buffer, "GET /add?task=")) {
        char *start = strstr(buffer, "task=") + 5;
        char task[100];

        int i = 0;
        while (start[i] != ' ' && start[i] != '\0') {
            task[i] = start[i];
            i++;
        }
        task[i] = '\0';

        addTask(task);
        sprintf(response, "Task added: %s", task);
    }

    else if (strstr(buffer, "GET /process")) {
        char *t = processTask();
        sprintf(response, "Processed: %s", t);
    }

    else if (strstr(buffer, "GET /list")) {
        listTasks(response);
    }

    else {
        sprintf(response, "Server running");
    }

    char http_response[4096];
    sprintf(http_response,
        "HTTP/1.1 200 OK\\r\\n"
        "Content-Type: text/plain\\r\\n"
        "Access-Control-Allow-Origin: *\\r\\n"
        "\\r\\n"
        "%s",
        response
    );

    send(client_socket, http_response, strlen(http_response), 0);
}

// main server
int main() {
    int server_fd, client_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[3000] = {0};

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);

    printf("🚀 C Server running on http://localhost:%d\n", PORT);

    while (1) {
        client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);

        read(client_socket, buffer, 3000);
        handleRequest(client_socket, buffer);

        close(client_socket);
        memset(buffer, 0, sizeof(buffer));
    }

    return 0;
}