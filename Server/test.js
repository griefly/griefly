var msg    = require('./message');
var recv   = require('./receiver');
var client = require('./client');
var holder = require('./holder');
var last   = require('./last_messages');

console.log("Begin message.js test:")

console.log("Begin check_message_size test:");

console.log( msg.check_message_size(30));
console.log( msg.check_message_size(3000));
console.log(!msg.check_message_size(30000000000));

console.log("Begin parseMessage test:");

console.log(msg.parseMessage("20 -1 -1 ordinary yu"));
console.log(msg.parseMessage("20 -1 -1 ordinary yu gfdgfd fdf fdgsgfdsgfds gssgsfsgs s"));
console.log(msg.parseMessage("20 -1 -1 ordinary"));
console.log(msg.parseMessage("43243 ffff -1 ordinary") === null);
console.log(msg.parseMessage("ddd 1 -1 ordinary") === null);
console.log(msg.parseMessage("43243 3 prere ordinary 333333") === null);
console.log(msg.parseMessage("432433213-1ordinary") === null);

console.log("Begin test test:");

console.log( msg.test(msg.parseMessage("23 -1 -1 ordinary fdsfs"), msg.ordinary_type));
console.log(!msg.test(msg.parseMessage("23 -1 -1 ord1nary fdsfs"), msg.ordinary_type));
console.log( msg.test(msg.parseMessage("23 -1 -1 ord1nary fdsfs"), msg.any_type));
console.log(!msg.test(msg.parseMessage("23 -1 -1 ord1nary fdsfs"), msg.map_type));
console.log( msg.test(msg.parseMessage("23 -1 -1 map fdsfs"), msg.map_type));
console.log(!msg.test(msg.parseMessage("23 -1 -1 ord1nary fdsfs"), msg.system_type));
console.log( msg.test(msg.parseMessage("23 -1 -1 system fdsfs"), msg.system_type));
console.log( msg.test(msg.parseMessage("23 -1 -1 system fdsfs"), msg.system_type, /fdsfs/));
console.log(!msg.test(msg.parseMessage("23 -1 -1 system fdsfs"), msg.any_type, /f1sfs/));

console.log("Begin test holder.js");

var massive = new holder.holder();

massive.add(1);
massive.add(2);
massive.add(3);
massive.add(4);
massive.add(5);
massive.add(6);
massive.add(7);

massive.for_each(function (number) {
    if (number > 7)
        console.log(false);
    massive.add(8);
});

var str = "";
massive.for_each(function (number) {
    str = str + " " + number.toString();
});

//console.log(str);
console.log(str === " 1 2 3 4 5 6 7 8 8 8 8 8 8 8");

massive.del(8);
str = "";
massive.for_each(function (number) {
    str = str + " " + number.toString();
});
console.log(str === " 1 2 3 4 5 6 7 8 8 8 8 8 8");

str = "";
massive.for_each(function (number) {
    str = str + " " + number.toString();
    massive.del(8);
});
console.log(str === " 1 2 3 4 5 6 7 8 8 8 8 8 8");

str = "";
massive.for_each(function (number) {
    str = str + " " + number.toString();
});
//console.log(str);
console.log(str === " 1 2 3 4 5 6 7");

var lmess = new last.last_messages(10);

lmess.push(0);
lmess.push(1);
lmess.push(2);
lmess.push(3);
lmess.push(4);
lmess.push(5);
lmess.push(6);
lmess.push(7);
lmess.push(8);
lmess.push(9);

lmess.for_each(function (number) {
    console.log(number)
});
console.log("");

lmess.push(9);
lmess.push(8);
lmess.push(7);
lmess.push(6);
lmess.push(5);
lmess.push(4);
lmess.push(3);
lmess.push(2);
lmess.push(1);
lmess.push(0);

lmess.for_each(function (number) {
    console.log(number)
});
console.log("");

lmess.push(0);
lmess.for_each(function (number) {
    console.log(number)
});



