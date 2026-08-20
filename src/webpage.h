#ifndef WEBPAGE_H
#define WEBPAGE_H

// HTML und CSS liegen als zwei getrennte PROGMEM-Strings vor. Der ESP32
// liefert das CSS über eine eigene Route (/style.css, siehe main.cpp) aus,
// genau wie eine "echte" externe CSS-Datei - nur eben offline vom Gerät
// selbst statt von einem CDN.

const char* INDEX_HTML = R"rawliteral(
<!DOCTYPE html>
<html lang="de">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Habit Companion</title>
    <link rel="stylesheet" href="/style.css">
</head>
<body>
    <header>
        <div>
            <h1>Habit Companion</h1>
            <p class="status" id="statusText">
                <span class="status-dot-wrap">
                    <span class="status-dot-ping"></span>
                    <span class="status-dot"></span>
                </span>
                Connecting...
            </p>
        </div>
        <div class="avatar">
            <!-- device-mobile Icon -->
            <svg width="20" height="20" viewBox="0 0 24 24" fill="none">
                <rect x="7" y="2" width="10" height="20" rx="2" stroke="currentColor" stroke-width="1.8"/>
                <circle cx="12" cy="18.3" r="0.9" fill="currentColor"/>
            </svg>
        </div>
    </header>

    <main>
        <section class="card">
            <h2>
                <!-- plus-circle Icon -->
                <svg class="icon-blue" width="22" height="22" viewBox="0 0 24 24" fill="none">
                    <circle cx="12" cy="12" r="10" fill="currentColor"/>
                    <path d="M12 8v8M8 12h8" stroke="#fff" stroke-width="2" stroke-linecap="round"/>
                </svg>
                Set New Habit
            </h2>
            <form id="habitForm">
                <div class="field">
                    <label>What do you want to track?</label>
                    <input type="text" id="habitName" required placeholder="e.g., Drink Water">
                </div>
                <div class="grid-2">
                    <div class="field">
                        <label>Time</label>
                        <input type="time" id="habitTime" required value="14:00">
                    </div>
                    <div class="field">
                        <label>Melody</label>
                        <select id="habitMelody">
                            <option value="Reminder">Reminder</option>
                            <option value="Success">Success</option>
                        </select>
                    </div>
                </div>
                <button type="submit">Sync to Device</button>
            </form>
        </section>

        <section>
            <div class="list-header">
                <h2>Active Habits</h2>
                <span class="badge" id="habitCount">0/6</span>
            </div>
            <div id="habitList"></div>
        </section>
    </main>

    <script>
        document.addEventListener('DOMContentLoaded', () => {
            // Date.now() liefert UTC. Wir rechnen den Zeitzonen-Offset schon
            // hier heraus, damit der ESP32 (der keine Zeitzone kennt) direkt
            // mit der lokalen Wanduhrzeit rechnen kann - siehe main.cpp (gmtime_r).
            const tzOffsetSeconds = new Date().getTimezoneOffset() * 60;
            const currentUnixTime = Math.floor(Date.now() / 1000) - tzOffsetSeconds;

            // 1. Synchronisiere die echte Uhrzeit mit dem ESP32 (Offline-Trick!)
            fetch('/sync-time', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ timestamp: currentUnixTime })
            }).then(() => {
                document.getElementById('statusText').innerHTML =
                    '<span class="status-dot-wrap"><span class="status-dot"></span></span> Time Synced!';
                loadHabits();
            });

            function loadHabits() {
                fetch('/habits')
                    .then(res => res.json())
                    .then(data => renderHabits(data));
            }

            function renderHabits(data) {
                const list = document.getElementById('habitList');
                list.innerHTML = '';
                document.getElementById('habitCount').innerText = `${data.length}/6`;

                data.forEach(habit => {
                    list.innerHTML += `
                        <div class="habit-card">
                            <div>
                                <h3>${habit.name} <span class="streak">🔥 ${habit.currentStreak}</span></h3>
                                <p>${habit.time} &bull; ${habit.melody} ${habit.completedToday ? '<span class="done-badge">✓ Done</span>' : ''}</p>
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

            window.deleteHabit = function(id) {
                fetch(`/habits?id=${id}`, { method: 'DELETE' }).then(() => loadHabits());
            }

            document.getElementById('habitForm').addEventListener('submit', (e) => {
                e.preventDefault();
                const newHabit = {
                    id: Date.now(),
                    name: document.getElementById('habitName').value,
                    time: document.getElementById('habitTime').value,
                    melody: document.getElementById('habitMelody').value
                };
                fetch('/habits', {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/json' },
                    body: JSON.stringify(newHabit)
                }).then(() => {
                    document.getElementById('habitForm').reset();
                    document.getElementById('habitTime').value = "14:00";
                    loadHabits();
                });
            });
        });
    </script>
</body>
</html>
)rawliteral";

const char* STYLE_CSS = R"rawliteral(
:root {
    --gray-50:  #f9fafb;
    --gray-100: #f3f4f6;
    --gray-200: #e5e7eb;
    --gray-400: #9ca3af;
    --gray-500: #6b7280;
    --gray-600: #4b5563;
    --gray-800: #1f2937;
    --gray-900: #111827;
    --blue-500: #3b82f6;
    --emerald-400: #34d399;
    --emerald-500: #10b981;
    --orange-500: #f97316;
    --green-500: #22c55e;
    --red-400: #f87171;
}

* { box-sizing: border-box; }

body {
    margin: 0;
    font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif;
    -webkit-font-smoothing: antialiased;
    background-color: var(--gray-100);
    color: var(--gray-800);
    min-height: 100vh;
    display: flex;
    flex-direction: column;
    align-items: center;
    padding-bottom: 80px;
}

/* ---------- Header ---------- */
header {
    width: 100%;
    max-width: 28rem;
    margin: 0 auto;
    position: sticky;
    top: 0;
    z-index: 50;
    background: rgba(243, 244, 246, 0.8);
    -webkit-backdrop-filter: blur(12px);
    backdrop-filter: blur(12px);
    padding: 16px 24px;
    display: flex;
    align-items: center;
    justify-content: space-between;
    border-bottom: 1px solid var(--gray-200);
}
header h1 {
    font-size: 1.25rem;
    font-weight: 700;
    letter-spacing: -0.01em;
    margin: 0;
}
.status {
    font-size: 0.75rem;
    color: var(--gray-500);
    font-weight: 500;
    margin: 2px 0 0;
    display: flex;
    align-items: center;
    gap: 6px;
}
.status-dot-wrap {
    position: relative;
    display: inline-flex;
    height: 8px;
    width: 8px;
}
.status-dot-ping {
    position: absolute;
    display: inline-flex;
    height: 100%;
    width: 100%;
    border-radius: 9999px;
    background-color: var(--emerald-400);
    opacity: 0.75;
    animation: ping 1.4s cubic-bezier(0, 0, 0.2, 1) infinite;
}
.status-dot {
    position: relative;
    display: inline-flex;
    border-radius: 9999px;
    height: 8px;
    width: 8px;
    background-color: var(--emerald-500);
}
@keyframes ping {
    75%, 100% { transform: scale(2.2); opacity: 0; }
}
.avatar {
    height: 40px;
    width: 40px;
    background: #fff;
    border-radius: 9999px;
    display: flex;
    align-items: center;
    justify-content: center;
    box-shadow: 0 1px 2px 0 rgba(0,0,0,0.05);
    color: var(--gray-800);
}

/* ---------- Layout ---------- */
main {
    width: 100%;
    max-width: 28rem;
    margin: 0 auto;
    padding: 24px 24px 0;
    flex: 1;
    display: flex;
    flex-direction: column;
    gap: 32px;
}

/* ---------- Card ---------- */
.card {
    background: #fff;
    border-radius: 1.5rem;
    padding: 24px;
    box-shadow: 0 1px 2px 0 rgba(0,0,0,0.05);
    border: 1px solid rgba(243, 244, 246, 0.5);
}
.card h2 {
    font-size: 1.125rem;
    font-weight: 600;
    margin: 0 0 20px;
    display: flex;
    align-items: center;
    gap: 8px;
}
.icon-blue { color: var(--blue-500); }

/* ---------- Form ---------- */
form { display: flex; flex-direction: column; gap: 16px; }
.field { display: flex; flex-direction: column; gap: 6px; }
.field label {
    font-size: 0.875rem;
    font-weight: 500;
    color: var(--gray-600);
    margin-left: 4px;
}
.field input, .field select {
    width: 100%;
    padding: 12px 16px;
    background: var(--gray-50);
    border: 1px solid var(--gray-200);
    border-radius: 1rem;
    font-size: 1rem;
    color: var(--gray-800);
    font-family: inherit;
    appearance: none;
}
.field input:focus, .field select:focus {
    outline: none;
    border-color: var(--blue-500);
    box-shadow: 0 0 0 3px rgba(59, 130, 246, 0.2);
}
.grid-2 {
    display: grid;
    grid-template-columns: 1fr 1fr;
    gap: 16px;
}
button[type="submit"] {
    margin-top: 8px;
    width: 100%;
    background: var(--gray-900);
    color: #fff;
    font-weight: 500;
    font-size: 1rem;
    padding: 14px;
    border: none;
    border-radius: 1rem;
    box-shadow: 0 4px 6px -1px rgba(0,0,0,0.1);
    font-family: inherit;
}
button[type="submit"]:active { transform: scale(0.98); }

/* ---------- Habit-Liste ---------- */
.list-header {
    display: flex;
    align-items: center;
    justify-content: space-between;
    margin: 0 4px 16px;
}
.list-header h2 { font-size: 1.125rem; font-weight: 600; margin: 0; }
.badge {
    font-size: 0.75rem;
    font-weight: 600;
    background: var(--gray-200);
    color: var(--gray-600);
    padding: 4px 8px;
    border-radius: 9999px;
}
#habitList { display: flex; flex-direction: column; gap: 12px; }
.habit-card {
    background: #fff;
    padding: 16px;
    border-radius: 1rem;
    box-shadow: 0 1px 2px 0 rgba(0,0,0,0.05);
    border: 1px solid var(--gray-100);
    display: flex;
    justify-content: space-between;
    align-items: center;
}
.habit-card h3 {
    font-weight: 600;
    color: var(--gray-800);
    margin: 0;
    font-size: 1rem;
}
.streak { color: var(--orange-500); font-size: 0.875rem; margin-left: 8px; }
.habit-card p {
    font-size: 0.75rem;
    color: var(--gray-500);
    margin: 2px 0 0;
}
.done-badge { color: var(--green-500); margin-left: 4px; font-weight: 700; }
.delete-btn {
    background: none;
    border: none;
    color: var(--red-400);
    padding: 8px;
    cursor: pointer;
    display: flex;
}
)rawliteral";

#endif // WEBPAGE_H