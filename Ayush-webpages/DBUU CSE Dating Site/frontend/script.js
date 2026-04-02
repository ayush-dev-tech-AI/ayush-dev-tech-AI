    const cards = document.querySelectorAll('.card');
    const nav = document.querySelector('header nav');
    const toggle = document.querySelector('.menu-toggle');
    const likeBtn = document.querySelector('.like');
    const dislikeBtn = document.querySelector('.dislike');


    const revealCards = () => {
      cards.forEach(card => {
        const cardTop = card.getBoundingClientRect().top;
        if (cardTop < window.innerHeight - 100) {
          card.classList.add('visible');
        }
      });
    };

    toggle.addEventListener('click', () => {
      nav.classList.toggle('show');
    });

    window.addEventListener('scroll', revealCards);
    window.addEventListener('load', revealCards);

    // Swipe animations
    cards.forEach(card => {
      let isDragging = false;
      let startX, currentX;

      card.addEventListener('mousedown', (e) => {
        isDragging = true;
        startX = e.clientX;
      });

      card.addEventListener('mousemove', (e) => {
        if (!isDragging) return;
        currentX = e.clientX - startX;
        card.style.transform = `translateX(${currentX}px) rotate(${currentX/15}deg) scale(1.05)`;
      });

      card.addEventListener('mouseup', () => {
        isDragging = false;
        handleRelease(card, currentX);
      });

      card.addEventListener('touchstart', (e) => {
        isDragging = true;
        startX = e.touches[0].clientX;
      });

      card.addEventListener('touchmove', (e) => {
        if (!isDragging) return;
        currentX = e.touches[0].clientX - startX;
        card.style.transform = `translateX(${currentX}px) rotate(${currentX/20}deg)`;
      });

      card.addEventListener('touchend', () => {
        isDragging = false;
        handleRelease(card, currentX);
      });
    });

    function handleRelease(card, currentX) {

  if (!currentX) currentX = 0; // safety fix

  if (Math.abs(currentX) > 150) {

    // MATCH LOGIC
    if (currentX > 0 && Math.random() > 0.7) {
      showMatch();
    }

    card.style.transition = 'transform 0.5s ease, opacity 0.5s ease';
    card.style.transform = `translateX(${currentX > 0 ? 500 : -500}px) rotate(${currentX/10}deg)`;
    card.style.opacity = '0';

    setTimeout(() => card.remove(), 500);

  } else {
    card.style.transition = 'transform 0.3s ease';
    card.style.transform = 'translateX(0) rotate(0)';
  }
}
    function swipeCard(direction) {
      const card = document.querySelector('.card');
      if (!card) return;
      card.style.transition = 'transform 0.5s ease, opacity 0.5s ease';
      card.style.transform = `translateX(${direction === 'right' ? 500 : -500}px) rotate(${direction === 'right' ? 20 : -20}deg)`;
      card.style.opacity = '0';
      setTimeout(() => card.remove(), 500);
    }

    likeBtn.addEventListener('click', () => {
  swipeCard('right');

  // MATCH TRIGGER FOR BUTTON
  if (Math.random() > 0.7) {
    showMatch();
  }
});
    dislikeBtn.addEventListener('click', () => swipeCard('left'));

setInterval(() => {
  const count = Math.floor(Math.random() * 200 + 50);
  const el = document.getElementById("online-count");
  if (el) el.innerText = `🔥 ${count} users online`;
}, 2000);


function showProfileMatch() {
  const screen = document.getElementById("match-screen");
  if (screen) {
    screen.classList.add("show");
  }
}
function showMatch() {
  const popup = document.getElementById("match-popup");
  if (!popup) return;

  popup.classList.add("show");

  setTimeout(() => {
    popup.classList.remove("show");
  }, 2000);
}

let matchCount = 0;

const matchEl = document.getElementById("match-count");

const interval = setInterval(() => {
  
  if (matchCount >= 100) {
    matchEl.innerText = "100+";
    clearInterval(interval);
    return;
  }

  // random increment (1–5)
  matchCount += Math.floor(Math.random() * 5) + 1;

  matchEl.innerText = matchCount;
  matchEl.style.transform = "scale(1.2)";
setTimeout(() => {
  matchEl.style.transform = "scale(1)";
}, 200);

}, 800);

function attachCardButtonEvents() {
  const cards = document.querySelectorAll('.card');

  cards.forEach(card => {

    const likeBtn = card.querySelector('.like-btn');
    const dislikeBtn = card.querySelector('.dislike-btn');
    const connectBtn = card.querySelector('.connect-btn');

    // LIKE
    likeBtn.addEventListener('click', () => {
      swipeCardElement(card, 'right');

      // optional match trigger 😏
      if (Math.random() > 0.7) {
        showMatch();
      }
    });

    // DISLIKE
    dislikeBtn.addEventListener('click', () => {
      swipeCardElement(card, 'left');
    });

    

  });
}

function swipeCardElement(card, direction) {
  if (!card) return;

  card.style.transition = 'transform 0.5s ease, opacity 0.5s ease';

  card.style.transform =
    `translateX(${direction === 'right' ? 500 : -500}px) rotate(${direction === 'right' ? 20 : -20}deg)`;

  card.style.opacity = '0';

  setTimeout(() => {
    card.remove();
  }, 500);
}
attachCardButtonEvents();

document.querySelectorAll('.connect-btn').forEach(btn => {
  btn.addEventListener('click', () => {

    const card = btn.closest('.card');

    const name = card.querySelector('h3').innerText;
    const bio = card.querySelector('p').innerText;
    const img = card.querySelector('img').src;

    // store data
    localStorage.setItem("chatName", name);
    localStorage.setItem("chatBio", bio);
    localStorage.setItem("chatImg", img);

    // redirect
    window.location.href = "connect.html";
  });
});