import * as socket from "/socket.js"

window.addEventListener("load", (event) => {
    let sendBtn = document.querySelector("#payloadSender");
    let generatePayloadBtn = document.querySelector("#generatePayloadBtn");
    let payloadArea = document.querySelector("#payloadArea");
    let payloadText = document.querySelector("#payloadText");
    let lipsum = new LoremIpsum();

    socket.connect("main");

    setInterval(() => {
        socket.send("ping");
    }, 1000);

    sendBtn.addEventListener('click', function () {
        socket.send(payloadArea.value);
    }, false);
    
    generatePayloadBtn.addEventListener('click', function () {
        let payloadCount = randomIntFromInterval(10, 1000);
        payloadArea.value = lipsum.generate(payloadCount);
        payloadText.innerHTML = `Generated ${payloadCount} words.`;
    }, false);
});

function randomIntFromInterval(min, max) { // min and max included 
    return Math.floor(Math.random() * (max - min + 1) + min)
}

