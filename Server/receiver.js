var msg = require('./message.js');

// Just append new piece of data to message buffer
function append_data(client, data) {
    //console.log("Recieve: " + data.toString() + "\n");
    client.buffer = client.buffer.concat(data.toString());
}

function read_message_size(client, data) {
    append_data(client, data);
    read_message_size_impl(client);
}

// Accept message size
function read_message_size_impl(client) {
    // Message format : NUMBER[WHITESPACE]SOMETHINGELSE
    var index = client.buffer.indexOf(' ');
    if (index === -1)
        return;
    var raw_size = client.buffer.substr(0, index);
    client.size = parseInt(raw_size);
    
    // Check for parse error
    if (isNaN(client.size) || !msg.check_message_size(client.size)) {
        console.log("Too long message from " + client.id + " "
                    + client.jid + " " + raw_size + " " + client.size);
        client.close();
        return;
    }
    client.size = client.size + raw_size.length + 1;
    
    console.log("client.size: " + client.size + "\n");
    
    // Read message tail
    client.reader = accept_message;
    accept_message_impl(client);
}

function accept_message(client, data) {
    append_data(client, data);
    accept_message_impl(client);
}

// Accept all message
function accept_message_impl(client) {
    if (client.size > client.buffer.length) {
        console.log("client.size" + client.size + "; client.buffer.length + " + client.buffer.length);
        return;
    }
    var index = client.buffer.indexOf(' ');
    console.log("Begin parse message");
    var message = msg.parseMessage(client.buffer.slice(index + 1, client.size));
    console.log("End parse messge");
    if (message === null) {
        console.log("Error parse message from " + client.id + " " + client.jid);
        client.close();
        return;
    }
    
    client.buffer = client.buffer.slice(client.size);
    client.size = 0;
    
    //console.log(message);
    
    // Okey, we receive message, now check all callback on this client
    client.check_message(message);
    
    // Read new message size
    client.reader = read_message_size;
    read_message_size_impl(client);
}

exports.accept_message = accept_message;
exports.read_message_size = read_message_size;
