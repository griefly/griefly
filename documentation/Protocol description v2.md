Packet structure
====

Message structure
----
`[header][body]`

Header structure
---
`[body size][message type]`
`body size` and `message type` are **4 bytes** variables *(it means that the header size is 8 bytes)*.
`body size` and `message type` are **big endian**

Body structure
---
`JSON (utf-8)`

##### Note
`body` should **not be empty**.
If there isn't any data then a body should contain empty json object (`{}`).

Examples
--------

### System type

```json
[[0x00000031][0x00000001]]
{"login" : "retard", "password" : "iamnotretard"}
```
```json
[[0x00000031][0x00000001]]
{"url_to_upload_map" : "grief.ly/FFFFFFFFFFFFFFFF", "tick": 42}
```
```json
[[0x00000035][0x00000001]]
{"url_to_download_map" : "grief.ly/FFFFFFFFFFFFFFFF"}
```

### Ordinary typa (aka keyboard type)

```json
[[0x0000001C][0x00000002]]
{"id" : 1, "key" : "KEY_UP"}
```
```json
[[0x00000020][0x00000002]]
{"id" : 0, "key" : "CREATE_NEW"}
```

### Message type

```json
[[0x00000032][0x00000003]]
{"id" : 1, "type" : "OOC", "text" : "Hello world"}
```

List of types
===
Defined types
-------------

### Message client -> server:

##### 1 - INITAL LOGIN MESSAGE
Size < 512
```json
{"login" : "Someone", "password" : "12345", "game_version" : "v0.2.1", "guest" : false}
```

##### 2 - EXIT_CLIENT
Empty json body
```json
{}
```

##### 3 - HASH MESSAGE
Size < 512
```json
{"hash":42,"tick":42}
```

##### 4 - RESTART ROUND
Empty json body
```json
{}
```

##### 5 - NEXTTICK
Empty json body
```json
{}
```

### Message server -> client:

#### Error messages:

##### 401 - WRONG GAME VERSION
Size < 512
```json
{"correct_game_version" : "v0.2.2"}
```

##### 402 - WRONG AUTHENTICATION
Empty json body
```json
{}
```

##### 403 - UNDEFINED ERROR
Empty json body
```json
{}
```

##### 404 - EXIT_SERVER
Empty json body
```json
{}
```

##### 405 - MASTER CLIENT IS NOT CONNECTED
Empty json body
```json
{}
```

##### 406 - CLIENT IS OUT OF SYNC
Empty json body
```json
{}
```

##### 407 - CLIENT TOO SLOW
Empty json body
```json
{}
```

##### 408 - INTERNAL SERVER ERROR
Size undefined
```json
{"message": "i'm ill"}
```

##### 409 - SERVER IS RESTARTING
Empty json body
```json
{}
```

#### NON-ERROR MESSAGES:

##### 201 - SUCCESS CONNECTION
Size < 512
```json
{"map" : "no_map", "your_id" : 42}
```

##### 202 - MAP UPLOAD
Size < 512
```json
{"url_to_upload_map" : "grief.ly/blah", "tick": 42}
```

##### 203 - NEW TICK
`(empty)`

##### 204 - NEW CLIENT
*Size:* < 128
```json
{"id" : 42}
```

##### 205 - CURRENT CONNECTIONS
Size < 128
```json
{"amount" : 42}
```

##### 206 - REQUEST HASH
```json
{"tick": 42}
```

#### GAME (BROADCASTED) TYPES:
They are mostly used only in the client.

##### 1001 - oridnary/keyboard
Size < 512
```json
{"key":"ENTER"}
```

##### 1002 - message 
Size < 2048
```json
{"text":"some text"}
```

##### 1003 - some complicated gui message (not used currently)

##### 1004 - action over object
Size < 512
```json
{"action":"rclick", "obj":42}
```

##### 1005 - ooc message
Size < 2048
```json
{"login":"login", "text":"some text"}
```

#### 1102: ping message
Size < 512
```json
{"ping_id":"some_unique_string"}
```

#### X - NOT DEFINED TYPE (X may be 1000, 2000 etc)
`message type` >=1000
If other action is not defined for this kind of the message then the server should broadcast the message.

PROTOCOL
====
### I.  LOGIN STAGE

 1. **Client -> Server**:  
Protocol version, **4** bytes: `S132`

 2. **Client -> Server**:  
`type: INITAL LOGIN MESSAGE`

    If `guest` is `true` then server should interpret this login as random not used login
     with empty (`""`) password, for example **"Guest4242"**. 
     Authentication will be always successful.

 3. **Server -> Client**  
	  If `client version` is **wrong**:  
		  `type: WRONG GAME VERSION`
	  If `login` or `password` are **wrong**:    
		  `type: WRONG AUTHENTICATION`
      If *master client* does not exist and login is not **admin**:  
	      `type: MASTER CLIENT IS NOT CONNECTED`
	
	  If something from above list happened then **close connection**.

	  If master client does not exist and `login` can be found as **admin**:  
	  Server creates new client and gives `ID` to him
	  `type: SUCCESS CONNECTION`
	 
	 **New client** now is master client.

	If master client exists and `login` does not have `ID`:   
		Server creates new client and gives `ID` to it
		**Server -> All Clients**: `type: NEW CLIENT`
	
	**Server -> Master client**:  
	`type: MAP UPLOAD`
	
	**Server -> Client**:  
	`type: SUCCESS CONNECTION`

	If master client exists and `login` does have matching **ID**:  
	Server uses existing client info, takes `ID` from that info and
	If there is an old alive connection that is associated with `login` then  
	server should drop the old connection.
	
	**Server -> Master client**:  
	` type: MAP UPLOAD`

	**Server -> Client**:  
	`type: SUCCESS CONNECTION`

	For example, during the process master client is disconnected, or something else:  
	` type: UNDEFINED ERROR`



### II. ACTIVE CONNECTION STAGE

##### Overall game cycle in the client:
1. Map generation/Map downloading
2. Broadcasted messages processing 
(*Note*: new client message is basically broadcasted message and can be processed here)
3. New tick message processing 
(*Note*: the tick counter increment)
4. Processing of system messages which depend from worlds state 
(*Note*: a request for hash, a request for map upload, etc.)
5. Jump to 2

System messages which doesn't depend from the world state (for example, pings) can be processed and sent in any moment. So, all special system messages (from p2) should be sent by the server only right after a tick message and before any broadcasted message. 

##### No specific order for requests

**Client -> Server**:  
Firstly, validate message, If succeeded then broadcast message.  
```
type: X
{some json}
```
**Server -> All Clients**:  
```
type: X
{"id" : 42, some json}
```
`id`: number of initial client (*message sender*)  
Additionaly,  
If message type is `1005` (ooc message) then the server places client login in the `login` json key.

 **Server -> Client**:   
Each T milliseconds (now T is 100, 10 messages per second)  
`type: NEW TICK`

**Server -> Client**:  
`type: REQUEST HASH`

**Client -> Server**:  
`type: HASH MESSAGE`

Server saves all hash messages. If some client is out of sync (his hash for some tick does not match
the hash for same tick for master client) then the server:  

**Server -> Master Client** :  
 `type: MAP UPLOAD`

 **Server -> Unsync Client**:  
` type: MAP UPLOAD`

**Server -> Unsync Client**:  
`type: CLIENT IS OUT OF SYNC` -> **connection closed**

Server should keep the data about each unsync incident: the unsync map, the master client map,
all hash messages up to the unsync moment

##### 205: CURRENT CONNECTIONS
Sometimes the server sends every client the message with current amount of active connections
(the amount of active players on server now)

##### 4: RESTART ROUND
**Admin Client -> Server**:  
If from non ADMIN client then do nothing else:  
**Server -> All Clients**:  
 `Error: SERVER IS RESTARTING`  
Restart server (basically, break all connections)  

##### 5: NEXTTICK
**Admin Client -> Server**:  
if from non ADMIN client then do nothing else:  
Server should perform `NEW_TICK` event (send `NEW_TICK` to every client even if time has not yet come). 

### III. END CONNECTION STAGE

**Client -> Server**:  
Client should send message on exit:  
`type: EXIT_CLIENT`

**Server -> Client**:  
Server if it closes connection (not WRONG GAME VERSION/etc cases) should send message on exit:  
`type: EXIT_SERVER`

IF CLIENT CLOSES HIS CONNECTION:


-  It is a regular client:
    Server should remove this *client* from the client list.
    Note that when any new client will be created server should not reuse numbers for clients.
    So we don't send to this client messages anymore, but we still keep information about it (`login` + `id`, maybe something else later (ip?)). 
    
- It is master client, and there is at least one other client.
Make a) with master client, but make some client (in future here will be some heuristic probably) master client.
If master client happens during a new client connection - we do not care.
The master client disconnection should be really rare case.

- It is master client, and there is no any other clients.
Forget everything. Basically, restart server (well, at least do not keep information about `login`s + `id`s)

##### HOW CLIENT UPLOADS MAP TO SERVER:

POST request to some url ("grief.ly/blah")
Content-Length as header
Content-Type: application/octet-stream or application/zip
Binary map in the body of the POST request.
Response 200 OK
Other responses - error.

##### HOW CLIENT DOWNLOADS MAP FROM SERVER

GET request to some url ("grief.ly/blah")
Response 200 OK
Content-Length in the response
Content-Type: <type> - type that was in uploaded map
Body is the binary map
Response 200 OK
Other responses - error.
