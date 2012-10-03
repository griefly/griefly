var clnt = require('./client');
var msg  = require('./message');
var msg_acpt = require('./message_accepter');

/*
This file describes process of login client
*/


// Check must client be trusted or not
function check_trusted(client) {
    if (clnt.get_trusted() != null)
        return;
    if (clnt.clients.length == 0) {
        if (client) {
            client.trusted = true;
        }
        return;
    }
    clnt.clients[0].trusted = true;
}

// Every jid has special token
var jids_and_tokens = []
// And game id
var jids_and_ids = []

function check_jid_token(jid, token) {
    // TODO: do it
    return true;
}

function check_jid_id(jid, id) {
    // TODO: do it
    return jids_and_ids[jid] == id;
}

var create_text = /create/;
var give_creator_text = "give_creator";
var nomap_text = "nomap";
var map_text = "map";
var need_map_text = "need_map";
var make_new_mob = "make_new_mob";

// Receive first message from client
function on_login_data(client, message) {
    console.error("Client begin" + client + "Client end");
    // If socket suddenly closed, we do nothing
    if (message === null)
        return true;
    
    // Right client or not
    if (check_jid_token(message.text, message.to) === false) {
        client.close();
        return true;
    }
    
    client.jid = message.text;
    
    // Can we trust this client
    check_trusted(client);
    if (client.trusted/* || check_jid_id(client.jid, message.from) TODO*/) {
        // Not send map to this client, he already has it
        jids_and_ids[client.jid] = message.from;
        client.game_id = message.from;
        message.text = nomap_text;
        client.send(message);
        // Ok, true client
        final_login(client);
        return true;
    }

    console.log("New client");
    // We must create new mob for this
    var creator_client = clnt.get_trusted();
    // Send all client: create new client (and answer when it will)
    msg_acpt.process_ordinary_message(
            creator_client,
            new msg.message(client.id, -1, "ordinary", make_new_mob));
    
    // Catch answer from trusted client (about client)
    creator_client.add_callback_once(function (trusted_client, message) {
        if (message === null) {
            client.close();
            return false;
        }
        if (message.from !== client.id) {
            // Not our client
            return false;
        }
        // Request map from trusted client
        console.log("Request map")
        var request_map = new msg.message(-1, client.id, "system", need_map_text);
        trusted_client.send(request_map);
        
        get_new_id(client, message, creator_client);
        return true;
    }, msg.system_type, create_text);
    console.log("Wait for creation answer");
    return true;
}

function get_new_id(client, message, trusted_client) {
    // Send new id to client
    // And what he must wait map after that
    jids_and_ids[client.jid] = message.to;
    client.game_id = message.to;
    message.text = map_text;
    client.send(message);
    
    // Wait for map from trusted client
    trusted_client.add_callback_once(function (trusted_client, message) {
        if (message === null) {
            client.close();
            return false;
        }
        if (message.to !== client.id) {
            return false;
        }
        // Resend message (map) to client
        client.send(message);
        
        // And send all messages that was received after map creation
        // to new client
        msg_acpt.send_old_messages(client, message.from);
        
        // End login
        final_login(client);
        return true;
    }, msg.map_type);
    // Ok, true client 
    console.log("Wait map");
}

// Add client to list all clients
// and add broadcaster
function final_login(client) {
    clnt.add(client);
    client.add_callback(msg_acpt.process_ordinary_message, msg.ordinary_type);
    client.add_callback(msg_acpt.process_system_message,   msg.system_type);  
}
exports.on_login_data = on_login_data;

