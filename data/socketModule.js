var states = {
    NOT_CONNECTED:0,
    CONNECTING:1,
    CONNECTED:2
}
const RECONNECT_INTERVAL_MS = 1000;

var state = states.NOT_CONNECTED;
var socket = null;
var endpoint = null;
var reconnectInterval = null;
var connectCB = null;
var rawMsgCB = null;
var closeCB = null;
var connectingCB = null;
var onHandlers = {};

var connect = function (_endpoint,cb) {
    if(state == states.CONNECTED){ return {status:false,message:"Already connected."}; }
    connectCB = cb;
    state = states.CONNECTING;
    if(connectingCB){
        connectingCB();
    }
    endpoint = _endpoint;
    socket = new WebSocket(`ws://${window.location.hostname}/${endpoint}`);
    setupEvents();
    startReconnect();
    return {status:true,message:"Connecting"};
}

var setupEvents = function(){

    socket.addEventListener('open', (event) => {
        clearInterval(reconnectInterval);
        state = states.CONNECTED;
        if(connectCB){
            connectCB(event);
        }
    });

    socket.addEventListener('close', function (event) {
        state = states.NOT_CONNECTED;
        if(closeCB){
            closeCB(event);
        }
        startReconnect();
    });

    socket.addEventListener('message', (event) => {
        let jsonData = isJsonString(event.data);
        if( jsonData ){
            processJsonData(jsonData);
        }else{
            processRawData(event.data);
        }
    });
}

var startReconnect = function(){
    console.log("[SOCKET] - Starting reconnect interval.");
    clearInterval(reconnectInterval);
    reconnectInterval = setInterval(() => {
        if( state.CONNECTING || state.CONNECTED ){ return; }
        socket.close();
        connect(endpoint,connectCB);
    }, RECONNECT_INTERVAL_MS);
}

var processJsonData = function(data){
    console.log("[SOCKET] - Json data: ", data);
    if (json.hasOwnProperty("type")) {
        let callbacksForType = onHandlers[json.type];
        if (callbacksForType) {
            for (const cb of callbacksForType) {
                cb(json);
            }
        }
    }
}

var processRawData = function(data){
    console.log("[SOCKET] - Raw data: ", data);
    if(rawMsgCB){
        rawMsgCB(data);
    }
}

var on = function (type, cb) {
    if(type === "close"){
        closeCB = cb;
        return;
    }else if( type === "connect" ){
        connectCB = cb;
        return;
    }else if( type == "connecting" ){
        connectingCB = cb;
        return;
    }else if( type === "raw" ){
        rawMsgCB = cb;
        return;
    }
    if( !onHandlers.hasOwnProperty(type) ){
        onHandlers[type] = [];
    }
    onHandlers[type].push(cb);
}

var isConnected = function () {
    return state === states.CONNECTED?true:false;
}

var close = function () {
    if (state === states.CONNECTED) { return {status:false,message:"Socket is not connected."}; }
    socket.close();
    return {status:true,message:"Socket closed."};
}

var send = function (msg) {
    if (state !== states.CONNECTED) { return {status:false,message:"Socket is not connected."}; }
    socket.send(msg);
    return {status:true,message:"Message sent."};
}

function isJsonString(jsonString) {
    try {
        var o = JSON.parse(jsonString);
        if (o && typeof o === "object") { return o; }
    }
    catch (e) { }
    return false;
};


export{
    send,
    close,
    isConnected,
    connect,
    on
}