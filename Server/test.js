var msg    = require('./message');
var recv   = require('./receiver');
var client = require('./client');

console.log("\nBegin message.js test:\n")

console.log("\nBegin check_message_size test:\n");

console.log( msg.check_message_size(30));
console.log( msg.check_message_size(3000));
console.log(!msg.check_message_size(30000000000));

console.log("\nBegin parseMessage test:\n");

console.log(msg.parseMessage("20 -1 -1 ordinary yu"));
console.log(msg.parseMessage("20 -1 -1 ordinary yu gfdgfd fdf fdgsgfdsgfds gssgsfsgs s"));
console.log(msg.parseMessage("20 -1 -1 ordinary"));
console.log(msg.parseMessage("43243 ffff -1 ordinary") === null);
console.log(msg.parseMessage("ddd 1 -1 ordinary") === null);
console.log(msg.parseMessage("43243 3 prere ordinary 333333") === null);
console.log(msg.parseMessage("432433213-1ordinary") === null);

console.log("\nBegin test test:\n");

console.log( msg.test(msg.parseMessage("23 -1 -1 ordinary fdsfs"), msg.ordinary_type));
console.log(!msg.test(msg.parseMessage("23 -1 -1 ord1nary fdsfs"), msg.ordinary_type));
console.log( msg.test(msg.parseMessage("23 -1 -1 ord1nary fdsfs"), msg.any_type));
console.log(!msg.test(msg.parseMessage("23 -1 -1 ord1nary fdsfs"), msg.map_type));
console.log( msg.test(msg.parseMessage("23 -1 -1 map fdsfs"), msg.map_type));
console.log(!msg.test(msg.parseMessage("23 -1 -1 ord1nary fdsfs"), msg.system_type));
console.log( msg.test(msg.parseMessage("23 -1 -1 system fdsfs"), msg.system_type));
console.log( msg.test(msg.parseMessage("23 -1 -1 system fdsfs"), msg.system_type, /fdsfs/));
console.log(!msg.test(msg.parseMessage("23 -1 -1 system fdsfs"), msg.any_type, /f1sfs/));

console.log("\nBegin test client.js\n");


