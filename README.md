# client-server-chat
A client-server network application that uses TCP connections to send short messages with user handles.
This program is best demoed using two adjacent terminal windows. One for the server and one for the client.

Usage: To run this program, do:

```bash
  ./chatserve
  ./chatclient
```
Both the client and server can be run on localhost, or any combination of flip servers.
The chatclient can close it's connection with the server by entering: \quit, the server will
remain on and can accept a new connection.

The chatserver can close it's connection with chat client by entering: \quit, this will close
both the server and the client connection.  
