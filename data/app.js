async function answer(value) {

    fetch("checkAnswer?answer="+value)
    .then(
      function(response) {
        if (response.status !== 200) {
          alert('Looks like there was a problem. Status Code: ' + response.status);
          return;
        }
  
        response.json().then(function(data) {
            responseEl = document.getElementById("response");
            responseEl.style.display = 'block';
            messageEl = document.getElementById("message");
            messageEl.innerHTML = data.message;
        });
      }
    )
    .catch(function(err) {
      alert('Fetch Error :-S', err);
    });
}