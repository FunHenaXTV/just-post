// const jsdom = require('jsdom')
// const dom = new jsdom.JSDOM("")
// const $ = require('jquery')(dom.window)
// const XMLHttpRequest = require('xhr2');

function get_post() {
    const post_id = document.getElementById("post_id_input").value;
    if (post_id < 1) {
        console.log("Bad request!");
        return;
    }

    const Http = new XMLHttpRequest();
    const url = "http://localhost:8080/v1/post/get_post_info?post_id=";
    Http.open("GET", url + post_id);
    Http.send();
    Http.onreadystatechange = (e)=>{
        if (Http.status != 200) {
            console.log("Bad request!");
            return;
        }

        if (Http.responseText) {
            const obj = JSON.parse(Http.responseText);
            document.getElementById("post_body").innerHTML = obj.post_body;
            document.getElementById("date").innerHTML = obj.date_of_post;
            document.getElementById("email").innerHTML = obj.email;
        }
    }
}
