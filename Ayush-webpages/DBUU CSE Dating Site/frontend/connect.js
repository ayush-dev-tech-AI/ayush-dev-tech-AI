// load data
document.getElementById("chat-name").innerText =
  localStorage.getItem("chatName");

document.getElementById("chat-bio").innerText =
  localStorage.getItem("chatBio");

document.getElementById("chat-img").src =
  localStorage.getItem("chatImg");


// send message
function sendMessage() {
  const input = document.getElementById("message-input");
  const chatBox = document.getElementById("chat-box");

  if (input.value.trim() === "") return;

  const msg = document.createElement("div");
  sendToBackend(input.value);
  msg.classList.add("message", "sent");
  msg.innerText = input.value;

  chatBox.appendChild(msg);

  input.value = "";
  chatBox.scrollTop = chatBox.scrollHeight;
}

const input = document.getElementById("message-input");

input.addEventListener("keypress", function(e) {
  if (e.key === "Enter") {
    sendMessage();
  }
});

setTimeout(() => {
  const reply = document.createElement("div");
  reply.classList.add("message", "received");
  reply.innerText = "Hey 😄";

  document.getElementById("chat-box").appendChild(reply);
}, 1000);

function sendToBackend(message) {
  fetch("http://localhost:8080", {
    method: "POST",
    body: message
  });
}