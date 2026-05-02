/* ──────────────────────────────────────────────
   Task Queue Frontend — C Backend Client
   All communication via pure fetch to C server
────────────────────────────────────────────── */

const BASE = "http://localhost:8080";
let selectedPriority = 3;

/* ── Priority Button Logic ── */
document.querySelectorAll(".prio-btn").forEach(btn => {
  btn.addEventListener("click", () => {
    document.querySelectorAll(".prio-btn").forEach(b => b.classList.remove("active"));
    btn.classList.add("active");
    selectedPriority = parseInt(btn.dataset.p);
  });
});

/* ── Enter key to add ── */
document.getElementById("taskInput").addEventListener("keydown", e => {
  if (e.key === "Enter") addTask();
});

/* ── Toast System ── */
let toastTimer = null;
function showToast(msg, type = "info") {
  const t = document.getElementById("toast");
  t.textContent = msg;
  t.className = `toast show ${type}`;
  clearTimeout(toastTimer);
  toastTimer = setTimeout(() => t.classList.remove("show"), 2800);
}

/* ── Server Status Ping ── */
async function checkServer() {
  const dot    = document.getElementById("statusDot");
  const label  = document.getElementById("serverStatus");
  try {
    const res = await fetch(`${BASE}/stats`, { signal: AbortSignal.timeout(2000) });
    if (res.ok) {
      dot.className = "dot online";
      label.textContent = "Server Online";
      return true;
    }
  } catch {
    dot.className = "dot offline";
    label.textContent = "Server Offline";
  }
  return false;
}

/* ── Render Queue ── */
function renderQueue(data) {
  const list  = document.getElementById("taskList");
  const empty = document.getElementById("emptyState");
  const labels = ["", "LOW", "MED-LO", "MED", "HIGH", "CRITICAL"];

  list.innerHTML = "";

  if (!data.tasks || data.tasks.length === 0) {
    empty.style.display = "block";
    return;
  }
  empty.style.display = "none";

  data.tasks.forEach(task => {
    const li = document.createElement("li");
    li.className = "task-item";
    li.setAttribute("data-p", task.priority);
    li.innerHTML = `
      <span class="task-id">${task.id}</span>
      <span class="task-name">${escapeHtml(task.task)}</span>
      <span class="task-prio prio-${task.priority}">${labels[task.priority] || task.priority}</span>
      <span class="task-ts">${task.timestamp}</span>
    `;
    list.appendChild(li);
  });
}

/* ── Update Stats ── */
function updateStats(data) {
  animateNumber("statSize",      data.size       ?? data.queue_size ?? 0);
  animateNumber("statProcessed", data.processed  ?? data.total_processed ?? 0);
  animateNumber("statTotal",     data.total      ?? data.total_added ?? 0);
}

function animateNumber(id, val) {
  const el = document.getElementById(id);
  const prev = parseInt(el.textContent) || 0;
  if (prev === val) return;
  el.style.transform = "scale(1.2)";
  el.textContent = val;
  setTimeout(() => el.style.transform = "scale(1)", 200);
}

/* ── Load Tasks & Stats ── */
async function loadAll() {
  try {
    const [listRes, statsRes] = await Promise.all([
      fetch(`${BASE}/list`),
      fetch(`${BASE}/stats`)
    ]);
    const listData  = await listRes.json();
    const statsData = await statsRes.json();

    renderQueue(listData);
    updateStats({
      size:      listData.size,
      processed: listData.total_processed,
      total:     statsData.total_added
    });
  } catch (err) {
    console.error("Load error:", err);
  }
}

/* ── Add Task ── */
async function addTask() {
  const input = document.getElementById("taskInput");
  const task  = input.value.trim();

  if (!task) {
    showToast("⚠️ Task cannot be empty", "error");
    input.focus();
    return;
  }

  try {
    const res  = await fetch(`${BASE}/add?task=${encodeURIComponent(task)}&priority=${selectedPriority}`);
    const data = await res.json();

    if (data.status === "added") {
      input.value = "";
      showToast(`✅ Task enqueued (P${selectedPriority})`, "success");
      await loadAll();
    } else {
      showToast(`❌ ${data.message || "Error"}`, "error");
    }
  } catch {
    showToast("❌ Cannot reach C server", "error");
  }
}

/* ── Process Task ── */
async function processTask() {
  try {
    const res  = await fetch(`${BASE}/process`);
    const data = await res.json();

    if (data.status === "processed") {
      showToast(`⚙️ Processed: ${data.task.task}`, "success");
    } else {
      showToast("📭 Queue is empty", "error");
    }
    await loadAll();
  } catch {
    showToast("❌ Cannot reach C server", "error");
  }
}

/* ── Clear Queue ── */
async function clearQueue() {
  if (!confirm("Clear all tasks from the queue?")) return;
  try {
    await fetch(`${BASE}/clear`);
    showToast("🗑️ Queue cleared", "info");
    await loadAll();
  } catch {
    showToast("❌ Cannot reach C server", "error");
  }
}

/* ── Escape HTML ── */
function escapeHtml(s) {
  return s.replace(/&/g, "&amp;").replace(/</g, "&lt;").replace(/>/g, "&gt;").replace(/"/g, "&quot;");
}

/* ── Init ── */
(async () => {
  await checkServer();
  await loadAll();

  // Poll for updates every 5s
  setInterval(async () => {
    await checkServer();
    await loadAll();
  }, 5000);
})();
