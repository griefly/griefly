var recv = require('./receiver');
var msg  = require('./message');
var hold = require('./holder');

/*
In this file defines class of client 
and some functions for work with all
clients (like broadcast, or add/delere 
client from list)
*/

// Generate unique client id
var unique_client_id = 0;
function get_new_client_id() {
    return ++unique_client_id;
}

var default_jid = "default@default.default";

// All clients here
var clients = [];

function add(client) {
    clients.push(client);
}

function del(client) {
    clients.splice(clients.indexOf(client), 1);
}

// Send message to all clients
function broadcast(message) {
    for (var i = 0; i < clients.length; ++i) {
        clients[i].send(message);
    }
}

// Return trusted client.
// Trusted means what we believe this client doesnt
// make errors or cheating
function get_trusted() {
    for (var i = 0; i < clients.length; ++i)
        if (clients[i].trusted == true)
            return clients[i];
    return null;
}

function client(socket) {
    // TCP socket
    this.socket  = socket;
    
    // Is closed?
    this.socket_closed = false;
    
    // Client id
    this.id      = get_new_client_id();
    // Jabber id (for authorization)
    this.jid     = default_jid;
    
    // Specific game id
    this.game_id = 0; 
    
    // Can we trust client or not?
    // If we trust, we may ask this client 
    // for send us map, or something like that
    this.trusted = false;
    
    // Buffer for incoming message
    this.buffer = "";
    
    // Size of message
    this.size   = 0;
    
    // Message accepter (first read size)
    this.reader = recv.read_message_size;
    
    // This callbacks calls many times
    this.callbacks_on_message = new hold.holder();
    
    // This callback calls once, when client
    // will remove it from self
    this.callbacks_on_message_once = new hold.holder();
    
    // Jist send message throw socket
    this.send = function (message) {
        var raw_message =
              message.number + " "
            + message.from   + " "
            + message.to     + " "
            + message.type   + " "
            + message.text;
        raw_message = raw_message.length + " " + raw_message;
        this.socket.write(raw_message);
    };  
    // Add once-callback
    // Args: 1st - callback, 2nd and 3rd - regular expressions
    // for type and content of messagr
    this.add_callback_once = function (callback, type, content) {
         this.callbacks_on_message_once.add(
            { callback : callback,
              type     : type,
              content  : content });   
    }
    
    // Add usuall callback
    // Args: 1st - callback, 2nd and 3rd - regular expressions
    // for type and content of messagr
    this.add_callback = function (callback, type, content) {
        this.callbacks_on_message.add(
            { callback : callback,
              type     : type,
              content  : content });
    }
    
    // Delete usuall callback
    // Args: 1st - callback, 2nd and 3rd - regular expressions
    // for type and content of messagr
    this.del_callback = function (callback, type, content) {
        this.callbacks_on_message.del(
                { callback : callback,
                  type     : type,
                  content  : content });
    }
    
    // Check all callback for incoming messages
    this.check_message = function (message) {
        var master_this = this;
        // Check usuall callbacks (no delete callbacks inside allowed)
        // If you add new callback inside other callback, message will
        // test for new callback
        this.callbacks_on_message.for_each(function (callback) {
            if (msg.test(message, callback.type, callback.content)) {
                callback.callback(master_this, message);
            }       
        })
        // Check once-callbacks (find first accepted and check
        // what callback return true. When delete callback, and return)
        // Add new callbacks allowed
        this.callbacks_on_message_once.for_each(function (callback) {
             if (msg.test(message, callback.type, callback.content)) {
                console.error("Client begin" +master_this + "Client end");
                if (callback.callback(master_this, message)) {
                    master_this.callbacks_on_message_once.del(callback);
                }
            }           
        });
    }
    
    // Pass to all callback null message. That means, what
    // we never receive message
    this.close = function () {
        var master_this = this;
        if (this.socket_closed === false) {
            this.socket_closed = true;
            console.log("Close client: " + this.id + " " + this.jid);
            this.callbacks_on_message.for_each(function (callback) {
                callback.callback(master_this, null)
            });
            this.callbacks_on_message_once.for_each(function (callback) {
                callback.callback(master_this, null)
            });
            this.socket.destroy();
        }
    }
}

exports.client = client;
exports.add = add;
exports.del = del;
exports.clients = clients; 
exports.broadcast = broadcast;
exports.get_trusted = get_trusted;
