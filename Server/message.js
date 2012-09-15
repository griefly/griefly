var system_type = /system/;
var map_type = /map/;
var ordinary_type = /ordinary/;
var any_type = /[\s\S]*/;

var any_content = /[\s\S]*/;


// Constructor for message class
function message(from, to, type, text) { 
    this.number = 0;
    this.from = from || -1;
    this.to = to || -1;
    this.type = type || "ordinary";
    this.text = text || "";
}

// Max length message (against spam)
var max_message_length = (1024 * 1024 * 16);
function check_message_length(length)
{
    if (length < max_message_length)
        return true;
    return false;
}

// From string to message
// Return error value - null
// On success - new message
function parseMessage(data) {
    console.log("Begin parse string: " + data);    
    
    if (!check_message_length(data.length))
        return null;
    var retval = new message();
    var words = data.split(" ", 4);
    if (words.length != 4) {
        return null;
    }
    retval.number = parseInt(words[0]);
    retval.from   = parseInt(words[1]);
    retval.to     = parseInt(words[2]);
    
    retval.type   = words[3];
    
    if (isNaN(retval.number) ||
        isNaN(retval.from)   ||
        isNaN(retval.to)) {
        return null;    
    }
    
    retval.text = data.substr(words.join(" ").length + 1);
    return retval;
}

// Test message for this type and content
// Type and content are regular expressions
// Return bool value
function test(message, type, content) {
    type = type || any_type;
    content = content  || any_content;
    return type.test(message.type) && content.test(message.text);
}

exports.system_type = system_type;
exports.map_type = map_type;
exports.ordinary_type = ordinary_type;
exports.any_type = any_type;

exports.any_content = any_content;

exports.parseMessage = parseMessage;
exports.message = message;
exports.test = test;
exports.check_message_size = check_message_length;
