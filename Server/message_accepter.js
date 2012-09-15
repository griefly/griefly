var msg = require('./message');
var clnt = require('./client');

/*
 In this file we write function 
 what process messages from client after
 message
*/
var unique_message_id = 0;
function get_new_message_id() {
    return ++unique_message_id;
}

// Message with ordinary type
function process_ordinary_message(client, message) {
    console.log("Process ordinary message");
    if (message == null) {
        return;
    }
    message.number = get_new_message_id();
    message.to = client.game_id;
    
    // Save and resend message
    push_message(message);
    clnt.broadcast(message);
    console.log(message);
}

function change_message(message) {
    message.number = get_new_message_id();
    message.to = client.game_id;
    
    // Save and resend message
    push_message(message);    
}

var messages = [];

var last_messages = 1000;
var mult_lm = 2;

// Save message
function push_message(message) {
    messages.push(message);
}

// Send old messages - for new clients
function send_old_messages(client, first_id) {
    if (messages[0].number > first_id)
        return false;

    for (var i = 0; i < messages.length; ++i) {
        if (messages[i].number >= first_id) {
            client.send(messages[i]);
        }
    }
}

exports.process_ordinary_message = process_ordinary_message;
exports.send_old_messages = send_old_messages;
exports.get_new_message_id = get_new_message_id;
exports.unique_message_id = unique_message_id;
exports.change_message = change_message;
exports.messages = messages;
exports.last_messages = last_messages;
exports.mult_lm = mult_lm;

exports.unique_message_id = unique_message_id;
