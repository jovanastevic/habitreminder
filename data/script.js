document.addEventListener("DOMContentLoaded", () => {
  // Date.now() liefert UTC. Wir rechnen den Zeitzonen-Offset schon
  // hier heraus, damit der ESP32 (der keine Zeitzone kennt) direkt
  // mit der lokalen Wanduhrzeit rechnen kann - siehe main.cpp (gmtime_r).
  const tzOffsetSeconds = new Date().getTimezoneOffset() * 60;
  const currentUnixTime = Math.floor(Date.now() / 1000) - tzOffsetSeconds;

  // 1. Synchronisiere die echte Uhrzeit mit dem ESP32 (Offline-Trick!)
  fetch("/sync-time", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ timestamp: currentUnixTime }),
  }).then(() => {
    document.getElementById("statusText").innerHTML =
      '<span class="status-dot-wrap"><span class="status-dot"></span></span> Time synced!';
    loadHabits();
  });

  function loadHabits() {
    fetch("/habits")
      .then((res) => res.json())
      .then((data) => renderHabits(data));
  }

  function renderHabits(data) {
    const list = document.getElementById("habitList");
    list.innerHTML = "";
    document.getElementById("habitCount").innerText = `${data.length}/6`;

    data.forEach((habit) => {
      list.innerHTML += `
                        <div class="habit-card">
                            <div>
                                <h3>${habit.name} <span class="streak">🔥 ${habit.currentStreak}</span></h3>
                                <p>${habit.time} &bull; ${habit.melody} ${habit.completedToday ? '<span class="done-badge">✓ Done</span>' : ""}</p>
                            </div>
                            <button class="delete-btn" onclick="deleteHabit(${habit.id})">
                                <svg width="18" height="18" viewBox="0 0 24 24" fill="currentColor">
                                    <path d="M9 3h6a1 1 0 0 1 1 1v1h4a1 1 0 0 1 0 2h-1l-1 13a2 2 0 0 1-2 2H8a2 2 0 0 1-2-2L5 7H4a1 1 0 0 1 0-2h4V4a1 1 0 0 1 1-1zm1 2h4V4h-4v1zm-1 4a1 1 0 0 1 1 1v8a1 1 0 0 1-2 0v-8a1 1 0 0 1 1-1zm4 0a1 1 0 0 1 1 1v8a1 1 0 0 1-2 0v-8a1 1 0 0 1 1-1z"/>
                                </svg>
                            </button>
                        </div>
                    `;
    });
  }

  window.deleteHabit = function (id) {
    fetch(`/habits?id=${id}`, { method: "DELETE" }).then(() => loadHabits());
  };

  document.getElementById("habitForm").addEventListener("submit", (e) => {
    e.preventDefault();
    const newHabit = {
      id: Date.now(),
      name: document.getElementById("habitName").value,
      time: document.getElementById("habitTime").value,
      melody: document.getElementById("habitMelody").value,
    };
    fetch("/habits", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify(newHabit),
    }).then(() => {
      document.getElementById("habitForm").reset();
      document.getElementById("habitTime").value = "14:00";
      loadHabits();
    });
  });
});
