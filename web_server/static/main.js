function toggleFullScreen() {
    let fsButton = document.getElementById('fullscreenButton');
    if (!document.fullscreenElement) {
        document.documentElement.requestFullscreen();
        fsButton.style.display = 'none'; // Hide the button when full screen
    } else {
      if (document.exitFullscreen) {
        document.exitFullscreen();
        fsButton.style.display = 'block'; // Show the button when not full screen
      }
    }
  }

  // Event listener to detect when exiting full screen
  document.addEventListener('fullscreenchange', (event) => {
    let fsButton = document.getElementById('fullscreenButton');
    if (!document.fullscreenElement) {
        fsButton.style.display = 'block'; // Show the button when not full screen
    }
  });

  document.getElementById('fullscreenButton').addEventListener('click', toggleFullScreen, false);
