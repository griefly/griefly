
function last_messages(size) {
    this.size = size;
    this.tail_messages = [];
    this.head_messages = [];
    
    this.push = function (message) {
        if (this.head_messages.length === this.size) {
            this.tail_messages = this.head_messages;
            this.head_messages = [];
        }
        this.head_messages.push(message);
    }
    
    this.for_each = function (callback) {
        for (var tail_counter = 0; tail_counter < this.tail_messages.length; ++tail_counter) {
            callback(this.tail_messages[tail_counter]);
        }
        for (var head_counter = 0; head_counter < this.head_messages.length; ++head_counter) {
            callback(this.head_messages[head_counter]);
        }
    }
}

exports.last_messages = last_messages;