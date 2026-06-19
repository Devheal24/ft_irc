# 📡 Réseau, Signaux et Gestion de Fichiers en C++

Ce document présente les principales fonctions système utilisées pour développer des applications réseau sous Linux/Unix, notamment un serveur IRC.

---

# 🔌 Création et gestion des sockets

## **socket()**

Crée un point de communication réseau.

### Prototype

```cpp
int socket(int domain, int type, int protocol);
```

### Exemple

```cpp
int fd = socket(AF_INET, SOCK_STREAM, 0);
```

### Paramètres

| Paramètre | Description |
|------------|-------------|
| `AF_INET` | IPv4 |
| `AF_INET6` | IPv6 |
| `SOCK_STREAM` | TCP |
| `SOCK_DGRAM` | UDP |
| `protocol` | Généralement `0` |

### Retour

- Descripteur de fichier valide
- `-1` en cas d'erreur

### Utilisation

Première étape de toute communication réseau.

---

## **close()**

Ferme un descripteur de fichier.

### Prototype

```cpp
int close(int fd);
```

### Exemple

```cpp
close(fd);
```

### Utilisation

Libère les ressources associées à :

- une socket
- un fichier
- un pipe

---

# ⚙️ Configuration des sockets

## **setsockopt()**

Configure des options sur une socket.

### Prototype

```cpp
int setsockopt(
	int sockfd,
	int level,
	int optname,
	const void *optval,
	socklen_t optlen
);
```

### Exemple

```cpp
int yes = 1;

setsockopt(
	fd,
	SOL_SOCKET,
	SO_REUSEADDR,
	&yes,
	sizeof(yes)
);
```

### Utilisations courantes

| Option | Description |
|----------|-------------|
| `SO_REUSEADDR` | Réutiliser un port immédiatement |
| `SO_KEEPALIVE` | Détecter les connexions mortes |
| `SO_RCVBUF` | Taille du buffer de réception |
| `SO_SNDBUF` | Taille du buffer d'envoi |

### Utilisation

Très souvent appelée avant `bind()`.

---

## **getsockname()**

Récupère l'adresse locale associée à une socket.

### Prototype

```cpp
int getsockname(
	int sockfd,
	struct sockaddr *addr,
	socklen_t *addrlen
);
```

### Exemple

```cpp
sockaddr_in addr;
socklen_t len = sizeof(addr);

getsockname(fd, (sockaddr *)&addr, &len);
```

### Utilisation

Permet de connaître :

- l'adresse locale utilisée
- le port choisi automatiquement

---

# 🌍 Résolution DNS

## **getprotobyname()**

Recherche un protocole réseau par son nom.

### Prototype

```cpp
struct protoent *getprotobyname(const char *name);
```

### Exemple

```cpp
protoent *proto = getprotobyname("tcp");
```

### Résultat

```cpp
proto->p_proto; // 6
```

### Utilisation

Ancienne fonction utilisée pour obtenir le numéro associé à un protocole.

---

## **gethostbyname()**

Résout un nom DNS.

### Prototype

```cpp
struct hostent *gethostbyname(const char *name);
```

### Exemple

```cpp
hostent *host = gethostbyname("google.com");
```

### Utilisation

Convertit un nom de domaine en adresse IP.

⚠️ Fonction obsolète.

---

## **getaddrinfo()**

Version moderne de résolution DNS.

### Prototype

```cpp
int getaddrinfo(
	const char *node,
	const char *service,
	const struct addrinfo *hints,
	struct addrinfo **res
);
```

### Exemple

```cpp
addrinfo hints;
addrinfo *result;

memset(&hints, 0, sizeof(hints));

hints.ai_family = AF_INET;
hints.ai_socktype = SOCK_STREAM;

getaddrinfo(
	"google.com",
	"80",
	&hints,
	&result
);
```

### Avantages

- Support IPv4
- Support IPv6
- Plusieurs résultats possibles
- Portable

### Utilisation

Fonction recommandée aujourd'hui.

---

## **freeaddrinfo()**

Libère la mémoire allouée par `getaddrinfo()`.

### Prototype

```cpp
void freeaddrinfo(struct addrinfo *res);
```

### Exemple

```cpp
freeaddrinfo(result);
```

---

# 🖥️ Création d'un serveur TCP

## **bind()**

Associe une socket à une adresse IP et un port.

### Prototype

```cpp
int bind(
	int sockfd,
	const struct sockaddr *addr,
	socklen_t addrlen
);
```

### Exemple

```cpp
bind(
	fd,
	(sockaddr *)&addr,
	sizeof(addr)
);
```

### Utilisation

Permet à un serveur d'écouter sur :

```text
127.0.0.1:6667
```

---

## **listen()**

Place une socket en mode écoute.

### Prototype

```cpp
int listen(int sockfd, int backlog);
```

### Exemple

```cpp
listen(fd, SOMAXCONN);
```

### Paramètre important

| Paramètre | Description |
|------------|-------------|
| `backlog` | Nombre maximal de connexions en attente |

---

## **accept()**

Accepte une connexion entrante.

### Prototype

```cpp
int accept(
	int sockfd,
	struct sockaddr *addr,
	socklen_t *addrlen
);
```

### Exemple

```cpp
int client_fd = accept(
	server_fd,
	(sockaddr *)&client_addr,
	&len
);
```

### Retour

- Nouveau descripteur client
- `-1` en cas d'erreur

### Utilisation

Crée une socket dédiée au client connecté.

---

# 🌐 Connexion client

## **connect()**

Établit une connexion vers un serveur.

### Prototype

```cpp
int connect(
	int sockfd,
	const struct sockaddr *addr,
	socklen_t addrlen
);
```

### Exemple

```cpp
connect(
	fd,
	(sockaddr *)&addr,
	sizeof(addr)
);
```

### Utilisation

Permet à un client TCP de se connecter à un serveur.

---

# 🔄 Conversion Host ↔ Network

Internet utilise le format **Big Endian**.

La plupart des processeurs utilisent **Little Endian**.

---

## **htons()**

### Signification

**Host TO Network Short**

### Prototype

```cpp
uint16_t htons(uint16_t hostshort);
```

### Exemple

```cpp
addr.sin_port = htons(6667);
```

### Utilisation

Convertit un entier 16 bits vers le format réseau.

---

## **htonl()**

### Signification

**Host TO Network Long**

### Prototype

```cpp
uint32_t htonl(uint32_t hostlong);
```

Convertit un entier 32 bits.

---

## **ntohs()**

### Signification

**Network TO Host Short**

### Exemple

```cpp
int port = ntohs(addr.sin_port);
```

---

## **ntohl()**

### Signification

**Network TO Host Long**

Convertit un entier 32 bits du format réseau vers le format machine.

---

# 📍 Manipulation des adresses IP

## **inet_addr()**

Convertit une IPv4 texte en format binaire.

### Prototype

```cpp
in_addr_t inet_addr(const char *cp);
```

### Exemple

```cpp
in_addr_t ip = inet_addr("127.0.0.1");
```

⚠️ Fonction ancienne.

---

## **inet_ntoa()**

Convertit une IPv4 binaire en texte.

### Prototype

```cpp
char *inet_ntoa(struct in_addr in);
```

### Exemple

```cpp
char *ip = inet_ntoa(addr.sin_addr);
```

Résultat :

```text
127.0.0.1
```

⚠️ Fonction ancienne.

---

## **inet_ntop()**

Version moderne.

### Prototype

```cpp
const char *inet_ntop(
	int af,
	const void *src,
	char *dst,
	socklen_t size
);
```

### Exemple

```cpp
char buffer[INET6_ADDRSTRLEN];

inet_ntop(
	AF_INET,
	&addr.sin_addr,
	buffer,
	sizeof(buffer)
);
```

### Utilisation

Compatible IPv4 et IPv6.

---

# 📨 Échange de données

## **send()**

Envoie des données.

### Prototype

```cpp
ssize_t send(
	int sockfd,
	const void *buf,
	size_t len,
	int flags
);
```

### Exemple

```cpp
send(
	fd,
	msg.c_str(),
	msg.size(),
	0
);
```

### Retour

Nombre d'octets envoyés.

---

## **recv()**

Lit des données.

### Prototype

```cpp
ssize_t recv(
	int sockfd,
	void *buf,
	size_t len,
	int flags
);
```

### Exemple

```cpp
char buffer[512];

recv(
	fd,
	buffer,
	sizeof(buffer),
	0
);
```

### Retour

| Valeur | Signification |
|---------|--------------|
| `> 0` | Données reçues |
| `0` | Connexion fermée |
| `-1` | Erreur |

---

# 🚨 Gestion des signaux

Un signal est une notification envoyée à un processus.

| Signal | Description |
|----------|-------------|
| `SIGINT` | Ctrl+C |
| `SIGTERM` | Arrêt demandé |
| `SIGQUIT` | Ctrl+\ |
| `SIGKILL` | Arrêt forcé |

---

## **signal()**

Associe une fonction à un signal.

### Prototype

```cpp
void (*signal(int sig, void (*handler)(int)))(int);
```

### Exemple

```cpp
signal(SIGINT, handler);
```

⚠️ Ancienne API.

---

## **sigaction()**

Version moderne recommandée.

### Prototype

```cpp
int sigaction(
	int signum,
	const struct sigaction *act,
	struct sigaction *oldact
);
```

### Exemple

```cpp
struct sigaction sa;

sa.sa_handler = handler;

sigaction(SIGINT, &sa, NULL);
```

---

# 🧩 Ensembles de signaux

Toutes les fonctions suivantes utilisent :

```cpp
sigset_t
```

---

## **sigemptyset()**

Vide complètement un ensemble.

```cpp
sigemptyset(&set);
```

---

## **sigfillset()**

Ajoute tous les signaux.

```cpp
sigfillset(&set);
```

---

## **sigaddset()**

Ajoute un signal.

```cpp
sigaddset(&set, SIGINT);
```

---

## **sigdelset()**

Retire un signal.

```cpp
sigdelset(&set, SIGINT);
```

---

## **sigismember()**

Teste la présence d'un signal.

```cpp
if (sigismember(&set, SIGINT))
{
}
```

---

# 📂 Gestion avancée des fichiers

## **lseek()**

Déplace le curseur d'un fichier.

### Prototype

```cpp
off_t lseek(
	int fd,
	off_t offset,
	int whence
);
```

### Valeurs possibles

| Valeur | Description |
|----------|-------------|
| `SEEK_SET` | Depuis le début |
| `SEEK_CUR` | Depuis la position actuelle |
| `SEEK_END` | Depuis la fin |

### Exemple

```cpp
lseek(fd, 0, SEEK_END);
```

---

## **fstat()**

Obtient les informations d'un fichier.

### Prototype

```cpp
int fstat(
	int fd,
	struct stat *buf
);
```

### Exemple

```cpp
struct stat st;

fstat(fd, &st);
```

### Informations utiles

```cpp
st.st_size
st.st_mode
st.st_uid
st.st_gid
```

---

## **fcntl()**

Contrôle le comportement d'un descripteur.

### Prototype

```cpp
int fcntl(
	int fd,
	int cmd,
	...
);
```

### Exemple

```cpp
fcntl(
	fd,
	F_SETFL,
	O_NONBLOCK
);
```

### Utilisation

Active le mode non bloquant.

Les appels suivants ne bloquent plus :

```cpp
accept();
recv();
read();
```

---

# 📊 Multiplexage des sockets

## **poll()**

Surveille plusieurs descripteurs simultanément.

### Prototype

```cpp
int poll(
	struct pollfd *fds,
	nfds_t nfds,
	int timeout
);
```

### Structure utilisée

```cpp
struct pollfd
{
	int fd;
	short events;
	short revents;
};
```

### Exemple

```cpp
pollfd p;

p.fd = server_fd;
p.events = POLLIN;

poll(&p, 1, -1);
```

### Événements principaux

| Événement | Description |
|------------|-------------|
| `POLLIN` | Données disponibles |
| `POLLOUT` | Écriture possible |
| `POLLERR` | Erreur |
| `POLLHUP` | Déconnexion |
| `POLLNVAL` | FD invalide |

### Utilisation

Fonction centrale des serveurs multi-clients non bloquants.

---

# 🔁 Cycle classique d'un serveur TCP

```text
socket()
	↓
setsockopt()
	↓
bind()
	↓
listen()
	↓
poll()
	↓
accept()
	↓
recv()
	↓
send()
	↓
close()
```

---

# ⭐ Fonctions indispensables pour ft_irc

1. **socket**
2. **setsockopt**
3. **bind**
4. **listen**
5. **accept**
6. **recv**
7. **send**
8. **close**
9. **fcntl**
10. **poll**
11. **sigaction**

Ces fonctions constituent le cœur d'un serveur TCP multi-clients non bloquant.


Bonne idée — voici un récap clair (sans toucher au code) des messages/séquences que HexChat (et la plupart des clients IRC) attend pour se « mettre en place » correctement. Respecte strictement les formats et terminaisons CRLF ("\r\n").

Principes clés

Le client UI (/join, /nick) convertit en commandes IRC sans / (ex: JOIN #chan\r\n).
Le serveur doit envoyer les numerics (001/332/353/366 ...) et réponses standard pour que le client considère la connexion « registered » et affiche correctement canaux / membres.
Toutes les lignes réseau doivent finir par \r\n.
À la connexion (accept)
Après accept(), pour que le client considère la session « enregistrée », envoie au moins:
RPL_WELCOME (001) : confirme l’enregistrement
Exemple:
:server 001 <nick> :Welcome to the IRC server\r\n
Optionnel mais recommandé: 002, 003, 004 (yourhost/created/myinfo)
Exemple:
:server 002 <nick> :Your host is server, running version X\r\n
:server 003 <nick> :This server was created ...\r\n
:server 004 <nick> servername version usermodes chanmodes\r\n
Si tu n’as pas encore reçu NICK/USER, tu peux fournir un placeholder nick (client<fd>) puis remplacer plus tard.
Enregistrement (NICK / USER)
Client envoie:
NICK newnick\r\n
USER username mode unused :Real Name\r\n
Serveur: quand reçu NICK+USER, marque registered et envoie 001..004.
Sur JOIN (client envoie JOIN #chan)
Serveur doit:
a) Ajouter le membre au channel.
b) Broadcast à tous les membres (y compris le joineur) le JOIN avec prefix complet:
:nick!user@host JOIN #chan\r\n
(idéal : inclure !user@host; au minimum :nick JOIN #chan\r\n).
c) Envoyer au client qui join:
RPL_TOPIC (332) si topic présent:
:server 332 <nick> <channel> :<topic>\r\n
sinon RPL_NOTOPIC (331):
:server 331 <nick> <channel> :No topic is set\r\n
RPL_NAMREPLY (353): liste des nicks dans le channel
:server 353 <nick> = <channel> :nick1 nick2 nick3\r\n
(le caractère après 353 — ici = — indique public/secret)
RPL_ENDOFNAMES (366):
:server 366 <nick> <channel> :End of /NAMES list\r\n
d) Optionnel: RPL_TOPICWHOTIME (333), MODE (324), creation time (329).
Pour les messages (PRIVMSG)
Quand un client envoie un message depuis HexChat, il enverra:
PRIVMSG #channel :le message\r\n
Serveur doit broadcast au channel avec prefix:
:nick!user@host PRIVMSG #channel :le message\r\n
Autres points importants
Préfixe (nick!user@host) : utile pour que le client affiche correctement l’auteur. Implémente NICK/USER pour construire ce préfixe proprement.
Numériques d’erreur : renvoie 401/403/442/482/... quand nécessaire (client non enregistré, channel n’existe pas, pas op, pas dans channel, etc.)
PING/PONG : implémente PING → PONG pour garder la connexion alive:
Client: PING :abcd\r\n → Server: PONG :abcd\r\n
Terminaison CRLF obligatoire — HexChat dépend de ça.
Encodage des messages: échappe/traite correctement : en début de param terminal (le protocole utilise : pour le param libre final).
Checklist minimal pour que HexChat affiche/autorise le channel

envoyer 001 au client après connexion (ou après NICK/USER) — indispensable ;
après JOIN, envoyer JOIN broadcast + 332/331 + 353 + 366 ;
renvoyer les messages (PRIVMSG) préfixés :nick!user@host PRIVMSG ... ;
répondre aux PING.
Souhaite-tu que j’écrive un court exemple de session (séquence client→serveur et serveur→client) complète ? Ou que j’implémente proprement NICK/USER et remplacements de placeholders dans le code ?