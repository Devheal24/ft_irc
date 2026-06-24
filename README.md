*This project has been created as part
of the 42 curriculum by jhubier, jodone and mgarnier.*

# <h1 align="center"><p style="font-size: 70px;"><span style="color:purple">ft_irc</span></h1>

# <span style="color:white">Description</span>

This project is to create a server while the purpose is to allow a text-based chat with the Internet Relay Chat (IRC) protocol.

# <span style="color:white">Instructions</span>

`make`

`./ircserv <port> <password>`

<h2 align="center">WITH HEXCHAT</h2>

- Launch Hexchat and connect with the ip of the server and port used by the server. We limited the range of the ports between 6665 and 6669.

- Set a password in profile if needed.

<h3>USERS COMMANDS:</h3>

- `/join #channelname` to join a channel. If it not exist, the command create one and you'll be channel's operator .

- `/nick <new nickname>` to change your nickname.

- `/user <new username>` to change your username.

- `/user <username> 0 * :<new realname>` to set your realname.

- `/msg <nickname> :<message>` to send a private message to a user.

- `/topic exemple` to change the channel's topic. Only possible if operator remove the limitation.

- `/topic` to display actual topic.

- `/part` to leave the current channel.

- `/close` to leave and close the current channel.

- `/quit` to leave the server.

<h3>OPERATOR COMMANDS:</h3>

- `/invite nickname` to invite a user in channel, if channel is in invite only.

- `/kick nickname` to kick a user from channel.

- `/mode +t` to limit the changement of the topic by the operators only. `/mode -t` to remove the limit, all the users can now change the channel's topic.

- `/topic exemple` to change the channel's topic. Use `/topic` to display actual topic.

- `/mode +i` to set channel in invite only. Remove with `/mode -i`. Use `/mode +t` to set channel in topic restricted.

- `/invite <nickname>` to invite a user to the current channel if the invite mode is activated.

- `/mode +o <nickname>` to set a user in the channel as operator. Use `/mode -o <nickname>` to remove the status operator to the user.

- `/mode +l <n>` to set the limit of member of the current channel to `n`. Use `/mode -l` to unset the limit.

- `/mode +k <password>` to set a password to the current channel. Use `/mode -k` to unset the password.

<h2 align="center">VIDEO</h2>

<h1 align="center"><video src="images/hexchat.mp4" autoplay loop muted width="1000"></video></h1>



# <span style="color:white">Resources</span>

We found the majority of our informations with this sources:

[beej.us](https://beej.us/guide/bgnet/pdf/bgnet_usl_c_1.pdf) -> explanations of differents functions (socket, bind, poll, accept, recv, send, ...)

[frameip.com](https://www.frameip.com/c-mode-connecte/?video=346#video-346) -> protocol between client and server and a minimalist example in C++

[modern.ircdocs.horse](https://modern.ircdocs.horse/#rplwelcome-001) -> communication between client and server with RPL

# AI

ChatGPT -> provide us some explanations and examples with new concepts.

Copilot -> help us to set the right syntax with new functions
