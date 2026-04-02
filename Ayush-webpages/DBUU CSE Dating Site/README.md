# 💖 DBUU CSE Dating Site

A modern, interactive dating web application built for B.Tech CSE students — powered by a custom **C-based backend** and **Data Structures & Algorithms (DSA)**.

---

## 🚀 Project Vision

This project is not just a simple website — it's an attempt to **build a backend system from scratch using C**, without relying on high-level frameworks.

> Goal: Understand how real backend systems work internally by implementing everything manually.

---

## 🧠 Core Concept

* Frontend handles UI/UX (cards, swipe, animations)
* Backend (in C) handles:

  * User management
  * Matching logic
  * Data storage
* DSA is used to simulate real-world backend behavior

---

## 🏗️ Tech Stack

### 🎨 Frontend

* HTML
* CSS
* JavaScript

### ⚙️ Backend

* C (Socket Programming)
* File Handling
* Custom Routing System

### 🧩 DSA Concepts Used

* Linked List → User storage
* Queue → Profile recommendation
* Graph → Match system (user connections)
* Arrays → Data handling

---

## 📂 Project Structure

```
DBUU Dating Site/
│
├── frontend/
│   ├── index.html
│   ├── style.css
│   ├── script.js
│
├── backend/
│   ├── main.c
│   ├── server.c
│   ├── router.c
│   ├── router.h
│
│   ├── models/
│   ├── data/
│   ├── dsa/
│   ├── utils/
│
└── README.md
```

---

## ✨ Features

* 💌 Swipe-based UI (Like / Dislike)
* 👤 User profiles
* ❤️ Match detection system
* 📊 Compatibility scoring (basic algorithm)
* 🔐 Session handling (basic)
* 📁 File-based data storage (custom database)

---

## 🔥 Highlight Feature

### 💘 Match Engine

If:

```
User A likes User B
AND
User B likes User A
```

→ It's a Match!

Implemented using **graph-based relationships** between users.

---

## 🌐 API Design (Planned)

| Endpoint    | Method | Description    |
| ----------- | ------ | -------------- |
| `/signup`   | POST   | Register user  |
| `/login`    | POST   | Login user     |
| `/profiles` | GET    | Fetch profiles |
| `/like`     | POST   | Like a user    |
| `/matches`  | GET    | Get matches    |

---

## 📌 Learning Outcomes

* Understanding backend without frameworks
* Working with low-level memory and files
* Implementing real-world logic using DSA
* Building a full-stack system from scratch

---

## ⚡ Future Enhancements

* Password hashing
* Better recommendation algorithm
* Real-time updates
* Chat system (advanced)
* UI improvements

---

## 🎯 Conclusion

This project demonstrates how a full backend system can be built using **core programming concepts** without relying on modern frameworks.

> “Not just using technology — understanding it from the ground up.”

---

## 🧑‍💻 Author

* Ayush (B.Tech CSE - Data Science)

---

## 🌀 Status

🚧 In Development — Building step by step
