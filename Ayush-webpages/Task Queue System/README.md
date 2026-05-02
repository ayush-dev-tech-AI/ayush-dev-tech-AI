# ⚡ Task Queue System — C Backend + HTML/JS Frontend

A full-stack task queue built with **pure C HTTP server** and a sleek frontend.

---

## Architecture

```
┌──────────────────────┐         ┌─────────────────────────┐
│   Frontend (Browser) │ ──GET──▶│  C HTTP Server :8080     │
│  index.html          │ ◀─JSON─ │  Linked List Queue       │
│  style.css           │         │  Priority Scheduling     │
│  app.js              │         │  No frameworks, pure C   │
└──────────────────────┘         └─────────────────────────┘
```

## Features

- ✅ **Pure C backend** — raw socket HTTP, zero dependencies
- ✅ **Priority Queue** — tasks sorted by priority (1–5), higher goes first
- ✅ **Linked List** — dynamic memory, O(n) priority insert
- ✅ **CORS enabled** — frontend can talk to backend freely
- ✅ **URL decode** — spaces and special chars handled correctly
- ✅ **Server status indicator** — live ping every 5s
- ✅ **Toast notifications** — non-intrusive feedback
- ✅ **Auto-refresh** — queue updates automatically

---

## Setup

### 1. Build & Run the C Backend

```bash
cd backend
make run
# Server starts on http://localhost:8080
```

Or manually:
```bash
gcc -Wall -O2 -o server server.c
./server
```

### 2. Open the Frontend

```bash
cd frontend
# Just open index.html in your browser
open index.html
# OR serve with any static server:
python3 -m http.server 3000
```

---

## API Endpoints

| Method | Endpoint | Description |
|--------|----------|-------------|
| GET | `/add?task=NAME&priority=1-5` | Add task to queue |
| GET | `/process` | Dequeue & process top task |
| GET | `/list` | Get all tasks as JSON |
| GET | `/clear` | Clear entire queue |
| GET | `/stats` | Get queue statistics |

### Example Requests

```bash
# Add a critical task
curl "http://localhost:8080/add?task=Deploy+Server&priority=5"

# Add a low priority task
curl "http://localhost:8080/add?task=Send+Report&priority=1"

# List all tasks
curl "http://localhost:8080/list"

# Process the highest priority task
curl "http://localhost:8080/process"
```

---

## Priority Levels

| Level | Label    | Color  |
|-------|----------|--------|
| 1     | Low      | Grey   |
| 2     | Med-Low  | Blue   |
| 3     | Medium   | Cyan   |
| 4     | High     | Yellow |
| 5     | Critical | Red    |

---

## Project Structure

```
taskqueue/
├── backend/
│   ├── server.c     ← HTTP server + queue logic (pure C)
│   └── Makefile
└── frontend/
    ├── index.html   ← UI structure
    ├── style.css    ← Industrial terminal theme
    └── app.js       ← API client + render logic
```

---

*Built with C, raw sockets, and zero regrets.*

## Github project permalink :-

https://github.com/ayush-dev-tech-AI/ayush-dev-tech-AI/tree/28f24249acb40d21d3d1ba05a778c28d41fbc026/Ayush-webpages/Task%20Queue%20System