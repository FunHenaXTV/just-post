document.addEventListener('DOMContentLoaded', function() {
  document.querySelector('.button').addEventListener('click', function() {
    var email = document.querySelector('input[placeholder="Почта"]').value;
    var password = document.querySelector('input[placeholder="Пароль"]').value;
    var confirm = document.querySelector('input[placeholder="Подтвердить пароль"]').value;
    var emailError = document.getElementById('emailError');
    var passwordError = document.getElementById('passwordError');
    var confirmError = document.getElementById('confirmError');

    var url = 'http://localhost:8080/v1/user/create?почта=' + email+ '&passwd=' + password;

    fetch(url, {method: 'POST'})
    .then(function(response) {
      if (response.ok) {
       window.location.href = 'mainpage.html';
      } else {
        alert("Ошибка регистрации");
      }
    })
    .catch(function(error) {
      console.log('Ошибка', error);
    });
  });
});