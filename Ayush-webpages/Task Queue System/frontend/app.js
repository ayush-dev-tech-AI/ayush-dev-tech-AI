let tasks = [];

// render UI
function renderTasks() {
  const list = document.getElementById("taskList");
  list.innerHTML = "";

  tasks.forEach(task => {
    const li = document.createElement("li");
    li.innerText = task;
    list.appendChild(li);
  });
}

// add task
function addTask() {
  const input = document.getElementById("taskInput");
  const task = input.value.trim();

  if (!task) return;

  tasks.push(task);
  input.value = "";

  document.getElementById("status").innerText = "Task added ✅";
  renderTasks();
}

// process task
function processTask() {
  if (tasks.length === 0) {
    document.getElementById("status").innerText = "Queue empty ❌";
    return;
  }

  const removed = tasks.shift();
  document.getElementById("status").innerText = `Processed: ${removed}`;

  renderTasks();
}


const BASE_URL = "http://localhost:8080";

// ADD TASK
async function addTask() {
  const task = document.getElementById("taskInput").value;

  await fetch(`${BASE_URL}/add?task=${task}`);
  document.getElementById("taskInput").value = "";

  loadTasks();
}

// LOAD TASKS
async function loadTasks() {
  const res = await fetch(`${BASE_URL}/list`);
  const data = await res.text();

  const list = document.getElementById("taskList");
  list.innerHTML = "";

  data.split("\n").forEach(task => {
    if (task.trim()) {
      const li = document.createElement("li");
      li.innerText = task;
      list.appendChild(li);
    }
  });
}

// PROCESS TASK
async function processTask() {
  await fetch(`${BASE_URL}/process`);
  loadTasks();
}

// AUTO LOAD
loadTasks();