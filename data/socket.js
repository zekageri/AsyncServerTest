var socket = null;
var timer = null;
var connected = false;
var endpoint = "";
var isDebugOn = true;
const SOCKET_RECONNECT_INTERVAL = 2000;

var onHandlers = {};

var connect = function (_endpoint) {
    if (connected) { return false; }
    endpoint = _endpoint;
    socket = new WebSocket(`ws://${window.location.hostname}/${endpoint}`);
    events();
    return true;
}

var events = function () {

    socket.addEventListener('open', (event) => {
        connected = true;
        clearInterval(timer);
        if (isDebugOn) {
            console.log(`[SOCKET] - Connected to ${endpoint}`);
        }
    });

    socket.addEventListener('close', function (event) {
        if (isDebugOn) {
            console.log(`[SOCKET] - Disconnected from ${endpoint}`);
        }
        clearInterval(timer);
        timer = setInterval(() => {
            connect(endpoint);
        }, SOCKET_RECONNECT_INTERVAL);
        connected = false;
    });

    socket.addEventListener('message', (event) => {
        let json = isJsonString(event.data);
        if (json) {
            if (json.hasOwnProperty("type")) {
                let callbacksForType = onHandlers[json.type];
                if (callbacksForType) {
                    for (const cb of callbacksForType) {
                        cb(json);
                    }
                }
            }
        } else {
            console.log('Non json message from server:', event.data);
        }
    });
}

var on = function (type, cb) {
    if( !onHandlers.hasOwnProperty(type) ){
        onHandlers[type] = [];
    }
    onHandlers[type].push(cb);
}

var isConnected = function () {
    return connected;
}

var close = function () {
    if (!connected) { return; }
    socket.close();
}

var send = function (msg) {
    if (!connected) { return false; }
    socket.send(msg);
    return true;
}


/**
 * If you don't care about primitives and only objects then this function
 * is for you, otherwise look elsewhere.
 * This function will return `false` for any valid json primitive.
 * EG, 'true' -> false
 *     '123' -> false
 *     'null' -> false
 *     '"I'm a string"' -> false
 */
function isJsonString(jsonString) {
    try {
        var o = JSON.parse(jsonString);

        // Handle non-exception-throwing cases:
        // Neither JSON.parse(false) or JSON.parse(1234) throw errors, hence the type-checking,
        // but... JSON.parse(null) returns null, and typeof null === "object", 
        // so we must check for that, too. Thankfully, null is falsey, so this suffices:
        if (o && typeof o === "object") {
            return o;
        }
    }
    catch (e) { }

    return false;
};

export {
    connect,
    close,
    send,
    isConnected,
    on
}