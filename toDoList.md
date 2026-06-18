# Si password faux puis vrai:
# Fix : NO
# Working on : ...
# end

# Si ...
# Fix : ...
# Working on : ...
comprendre l'utilisation de errno apres le poll (pour le sujet de correction)
embellir le code.
bot.
parsing du nick, et du password et du channel.
Enlever les debugs.

ctr+c un nc doesnt clear client
need to check if user.nick.pwd all set before handling any other input than PASS NICK USER
# end

norm : 

Nickname:

Traditionnellement :

Longueur maximale souvent entre 9 et 30 caractères selon l'implémentation.
Premier caractère :
lettre (A-Z, a-z) ou certains caractères spéciaux ([ ] \ ^ { }`)
Caractères suivants :
lettres
chiffres
caractères spéciaux autorisés

Exemple valide :

Alice
Jean42
[Admin]
User_Test

Exemple souvent invalide :

42Alice     (commence par un chiffre)
Mon Pseudo  (espace)
2. Username (ident)

L'« username » IRC (partie avant le @ dans nick!user@host) est généralement plus restrictive :

lettres
chiffres
_ ou -
pas d'espace
longueur souvent 8 à 32 caractères


password:
longueur minimale
minimum : 8 caractères
recommandé : 12 à 16 caractères ou plus
Longueur maximale
au moins 64 caractères acceptés
idéalement 128+
Autoriser
tous les caractères Unicode valides
espaces
symboles
Ne pas imposer
obligation de majuscule
obligation de chiffre
obligation de caractère spécial

Les recommandations modernes suivent les travaux du NIST (SP 800-63B).

Exemple de bon mot de passe