document.addEventListener('DOMContentLoaded', function() {
  document.querySelector('.button').addEventListener('click', function() {
    var email = document.querySelector('input[placeholder="Почта"]').value;
    var password = document.querySelector('input[placeholder="Пароль"]').value;
    var confirm = document.querySelector('input[placeholder="Подтвердить пароль"]').value;
    var emailError = document.getElementById('emailError');
    var passwordError = document.getElementById('passwordError');
    var confirmError = document.getElementById('confirmError');
    var ok = 1;

    if (!email.includes('@')) {
      emailError.innerText = 'Некорректный адрес электронной почты';
      event.preventDefault();
    } else {
      ok = 0;
      emailError.innerText = '';
    }
    
    if (password.length < 8) {
      passwordError.innerText = 'Пароль должен содержать не менее 8 символов';
      event.preventDefault();
    } else {
      ok = 0;
      passwordError.innerText = '';
    }
    
    if (password !== confirm) {
      confirmError.innerText = 'Пароли не совпадают';
      event.preventDefault();
    } else {
      ok = 0;
      confirmError.innerText = '';
    }

    if (ok) {
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
    }
  });
});


document.addEventListener('DOMContentLoaded', function() {
  document.getElementById('button').addEventListener('click', function(event) {
    var email = document.querySelector('input[placeholder="Почта"]').value;
    var password = document.querySelector('input[placeholder="Пароль"]').value;
    var confirm = document.querySelector('input[placeholder="Подтвердить пароль"]').value;
    var emailError = document.getElementById('emailError');
    var passwordError = document.getElementById('passwordError');
    var confirmError = document.getElementById('confirmError');
    
    if (!email.includes('@')) {
      emailError.innerText = 'Некорректный адрес электронной почты';
      event.preventDefault();
    } else {
      emailError.innerText = '';
    }
    
    if (password.length < 8) {
      passwordError.innerText = 'Пароль должен содержать не менее 8 символов';
      event.preventDefault();
    } else {
      passwordError.innerText = '';
    }
    
    if (password !== confirm) {
      confirmError.innerText = 'Пароли не совпадают';
      event.preventDefault();
    } else {
      confirmError.innerText = '';
    }
  });
});