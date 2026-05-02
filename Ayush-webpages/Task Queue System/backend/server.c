/*
 * =====================================================
 *   TASK QUEUE SERVER — Written in Pure C
 *   HTTP Server | Linked List Queue | CORS Enabled
 *   Captain's Engineering, Stardate 2025
 * =====================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>

#define PORT 8080
#define BUFFER_SIZE 4096
#define MAX_TASK_LEN 512

/* ── Queue Node ─────────────────────────────────── */
typedef struct Node {
    char task[MAX_TASK_LEN];
    char id[32];
    char timestamp[64];
    int  priority;
    struct Node* next;
} Node;

/* ── Queue State ────────────────────────────────── */
typedef struct {
    Node* head;
    Node* tail;
    int   size;
    int   total_processed;
} Queue;

Queue q = {NULL, NULL, 0, 0};
int   task_counter = 1;

/* ── Helpers ────────────────────────────────────── */

void get_timestamp(char* buf) {
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    strftime(buf, 64, "%Y-%m-%d %H:%M:%S", t);
}

/* URL decode: converts %20 → space, + → space, etc. */
void url_decode(const char* src, char* dst, int dst_len) {
    int i = 0, j = 0;
    while (src[i] && j < dst_len - 1) {
        if (src[i] == '%' && src[i+1] && src[i+2]) {
            char hex[3] = {src[i+1], src[i+2], 0};
            dst[j++] = (char)strtol(hex, NULL, 16);
            i += 3;
        } else if (src[i] == '+') {
            dst[j++] = ' ';
            i++;
        } else {
            dst[j++] = src[i++];
        }
    }
    dst[j] = '\0';
}

/* Extract query param value from URL */
int get_param(const char* url, const char* key, char* out, int out_len) {
    char search[64];
    snprintf(search, sizeof(search), "%s=", key);
    const char* pos = strstr(url, search);
    if (!pos) return 0;
    pos += strlen(search);
    int i = 0;
    while (pos[i] && pos[i] != '&' && pos[i] != ' ' && pos[i] != '\r' && pos[i] != '\n' && i < out_len - 1) {
        out[i] = pos[i];
        i++;
    }
    out[i] = '\0';
    return 1;
}

/* ── Queue Operations ───────────────────────────── */

void enqueue(const char* task_raw, int priority) {
    char task[MAX_TASK_LEN];
    url_decode(task_raw, task, MAX_TASK_LEN);

    Node* node = (Node*)malloc(sizeof(Node));
    if (!node) return;

    strncpy(node->task, task, MAX_TASK_LEN - 1);
    node->task[MAX_TASK_LEN - 1] = '\0';
    node->priority = priority;
    snprintf(node->id, sizeof(node->id), "T%04d", task_counter++);
    get_timestamp(node->timestamp);
    node->next = NULL;

    /* Priority insertion: higher priority goes before lower */
    if (!q.head || priority > q.head->priority) {
        node->next = q.head;
        q.head = node;
        if (!q.tail) q.tail = node;
    } else {
        Node* cur = q.head;
        while (cur->next && cur->next->priority >= priority) {
            cur = cur->next;
        }
        node->next = cur->next;
        cur->next = node;
        if (!node->next) q.tail = node;
    }
    q.size++;
}

Node* dequeue() {
    if (!q.head) return NULL;
    Node* node = q.head;
    q.head = q.head->next;
    if (!q.head) q.tail = NULL;
    q.size--;
    q.total_processed++;
    return node;
}

void clear_queue() {
    Node* cur = q.head;
    while (cur) {
        Node* nxt = cur->next;
        free(cur);
        cur = nxt;
    }
    q.head = q.tail = NULL;
    q.size = 0;
}

/* ── JSON Builders ──────────────────────────────── */

void build_list_json(char* buf, int buf_len) {
    int offset = 0;
    offset += snprintf(buf + offset, buf_len - offset,
        "{\"size\":%d,\"total_processed\":%d,\"tasks\":[",
        q.size, q.total_processed);

    Node* cur = q.head;
    int first = 1;
    while (cur) {
        /* Escape quotes in task name */
        char escaped[MAX_TASK_LEN * 2];
        int ei = 0;
        for (int i = 0; cur->task[i] && ei < (int)sizeof(escaped) - 2; i++) {
            if (cur->task[i] == '"') escaped[ei++] = '\\';
            escaped[ei++] = cur->task[i];
        }
        escaped[ei] = '\0';

        if (!first) offset += snprintf(buf + offset, buf_len - offset, ",");
        offset += snprintf(buf + offset, buf_len - offset,
            "{\"id\":\"%s\",\"task\":\"%s\",\"priority\":%d,\"timestamp\":\"%s\"}",
            cur->id, escaped, cur->priority, cur->timestamp);
        first = 0;
        cur = cur->next;
    }
    offset += snprintf(buf + offset, buf_len - offset, "]}");
}

/* ── HTTP Response Sender ───────────────────────── */

void send_response(int client_fd, int status, const char* content_type, const char* body) {
    char header[512];
    const char* status_text = (status == 200) ? "OK" :
                              (status == 400) ? "Bad Request" : "Not Found";

    snprintf(header, sizeof(header),
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %zu\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
        "Access-Control-Allow-Headers: Content-Type\r\n"
        "Connection: close\r\n"
        "\r\n",
        status, status_text, content_type, strlen(body));

    send(client_fd, header, strlen(header), 0);
    send(client_fd, body, strlen(body), 0);
}

/* ── Request Router ─────────────────────────────── */

void handle_request(int client_fd, char* request) {
    /* Extract method and URL */
    char method[8], url[1024];
    sscanf(request, "%7s %1023s", method, url);

    printf("[%s] %s %s\n", method, url,
           strncmp(method, "GET", 3) == 0 ? "" : "");

    /* CORS preflight */
    if (strcmp(method, "OPTIONS") == 0) {
        send_response(client_fd, 200, "text/plain", "");
        return;
    }

    /* ── GET /add?task=...&priority=... ── */
    if (strncmp(url, "/add", 4) == 0) {
        char task_raw[MAX_TASK_LEN] = {0};
        char prio_str[16] = "1";
        if (!get_param(url, "task", task_raw, sizeof(task_raw)) || strlen(task_raw) == 0) {
            send_response(client_fd, 400, "application/json",
                          "{\"status\":\"error\",\"message\":\"Task cannot be empty\"}");
            return;
        }
        get_param(url, "priority", prio_str, sizeof(prio_str));
        int priority = atoi(prio_str);
        if (priority < 1) priority = 1;
        if (priority > 5) priority = 5;

        enqueue(task_raw, priority);

        char resp[256];
        snprintf(resp, sizeof(resp),
            "{\"status\":\"added\",\"queue_size\":%d}", q.size);
        send_response(client_fd, 200, "application/json", resp);

    /* ── GET /process ── */
    } else if (strncmp(url, "/process", 8) == 0) {
        Node* node = dequeue();
        if (!node) {
            send_response(client_fd, 200, "application/json",
                          "{\"status\":\"empty\",\"message\":\"Queue is empty\"}");
            return;
        }
        char resp[2048];
        char escaped[MAX_TASK_LEN * 2];
        int ei = 0;
        for (int i = 0; node->task[i] && ei < (int)sizeof(escaped) - 2; i++) {
            if (node->task[i] == '"') escaped[ei++] = '\\';
            escaped[ei++] = node->task[i];
        }
        escaped[ei] = '\0';

        snprintf(resp, sizeof(resp),
            "{\"status\":\"processed\",\"task\":{\"id\":\"%s\",\"task\":\"%s\",\"priority\":%d},\"remaining\":%d,\"total_processed\":%d}",
            node->id, escaped, node->priority, q.size, q.total_processed);
        free(node);
        send_response(client_fd, 200, "application/json", resp);

    /* ── GET /list ── */
    } else if (strncmp(url, "/list", 5) == 0) {
        char body[BUFFER_SIZE * 4];
        build_list_json(body, sizeof(body));
        send_response(client_fd, 200, "application/json", body);

    /* ── GET /clear ── */
    } else if (strncmp(url, "/clear", 6) == 0) {
        clear_queue();
        send_response(client_fd, 200, "application/json",
                      "{\"status\":\"cleared\",\"message\":\"Queue cleared\"}");

    /* ── GET /stats ── */
    } else if (strncmp(url, "/stats", 6) == 0) {
        char resp[256];
        snprintf(resp, sizeof(resp),
            "{\"queue_size\":%d,\"total_processed\":%d,\"total_added\":%d}",
            q.size, q.total_processed, task_counter - 1);
        send_response(client_fd, 200, "application/json", resp);

    /* ── 404 ── */
    } else {
        send_response(client_fd, 404, "application/json",
                      "{\"status\":\"error\",\"message\":\"Route not found\"}");
    }
}

/* ── Main Server Loop ───────────────────────────── */

int main() {
    int server_fd, client_fd;
    struct sockaddr_in addr;
    char buffer[BUFFER_SIZE];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) { perror("socket"); exit(1); }

    /* Allow port reuse immediately after restart */
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind"); exit(1);
    }

    listen(server_fd, 10);

    printf("\n");
    printf("  ╔══════════════════════════════════════╗\n");
    printf("  ║   TASK QUEUE SERVER — C Backend      ║\n");
    printf("  ║   Running on http://localhost:%d    ║\n", PORT);
    printf("  ╠══════════════════════════════════════╣\n");
    printf("  ║  GET /add?task=X&priority=1-5        ║\n");
    printf("  ║  GET /process                        ║\n");
    printf("  ║  GET /list                           ║\n");
    printf("  ║  GET /clear                          ║\n");
    printf("  ║  GET /stats                          ║\n");
    printf("  ╚══════════════════════════════════════╝\n\n");

    while (1) {
        socklen_t addr_len = sizeof(addr);
        client_fd = accept(server_fd, (struct sockaddr*)&addr, &addr_len);
        if (client_fd < 0) { perror("accept"); continue; }

        memset(buffer, 0, BUFFER_SIZE);
        recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
        handle_request(client_fd, buffer);
        close(client_fd);
    }

    close(server_fd);
    return 0;
}
