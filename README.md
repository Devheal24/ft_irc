*This project has been created as part
of the 42 curriculum by jhubier, jodone and mgarnier.*

# <h1 align="center"><p style="font-size: 70px;"><span style="color:purple">ft_irc</span></h1>

# <span style="color:white">Description</span>

This project is to create a server while the purpose is to allow a text-based chat with the Internet Relay Chat (IRC) protocol.

# <span style="color:white">Instructions</span>

`make`

`./ircserv <port> <password>`

	- Launch Hexchat and connect with the ip of the server and port used by the server.
	- Set a password in profile if needed.
	- `/join #channelname` to join a channel. If it not exist, the command create one and you'll be channel's operator .
	- As operator, use `/invite nickname` to invite a user in channel, if channel is in invite only.
	- As operator, use `/kick nickname` to kick a user from channel.
	- As operator, if the channel is topic restricted, use `/topic exemple` to change the channel's topic. Use `/topic` to display actual topic.
	- As operator, use `/mode +i` to set channel in invite only. Remove with `/mode -i`. Use `/mode +t` to set channel in topic restricted.

<h1 align="center"><video src="images/hexchat.mp4" autoplay loop muted width="1000"></video></h1>



# <span style="color:white">Resources</span>

We found the majority of our informations with this sources:

[beej.us](https://beej.us/guide/bgnet/pdf/bgnet_usl_c_1.pdf) -> explanations of differents functions (socket, bind, poll, accept, recv, send, ...)

[frameip.com](https://www.frameip.com/c-mode-connecte/?video=346#video-346) -> protocol between client and server and a minimalist example in C++

[modern.ircdocs.horse](https://modern.ircdocs.horse/#rplwelcome-001) -> communication between client and server with RPL

# AI

ChatGPT -> provide us some explanations and examples with new concepts.

Copilot -> help us to set the right syntax with new functions
