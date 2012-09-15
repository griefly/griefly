var net  = require('net');
var client = require('./client');
var msg = require('./message');
var login = require('./login');
var msg_acpt = require('./message_accepter');

/*
This is entry point of server.
*/

net.createServer(function (socket) {

    // Client is class
    var new_client = new client.client(socket);
    
    // Begin process messages - first message is login data
    // from client to server
    new_client.add_callback_once(login.on_login_data);
    
    // When some data has been received, we
    // pass this data into special handler
    socket.on('data', function (data) {
        new_client.reader(new_client, data);
    });
    
    socket.on('error', function (exception) {
        console.log(new_client.id + " error: " + exception);
    });
    
    socket.on('end', function () {
        console.log(new_client.id + " end");
        socket.end();
    });

    socket.on('close', function (error) {
        client.del(new_client);
        new_client.close();
        console.log(new_client.id + " exit")
    });
    
    console.log(new_client.id + " login");

}).listen(1111);

// "Metronome" for clients
setInterval(function () {
   var message = new msg.message();
   message.number = msg_acpt.get_new_message_id();
   message.text = "nexttick";
   client.broadcast(message);
}, 500);
