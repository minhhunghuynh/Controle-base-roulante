# Code V1
## Description
Encoders donné par michel osée. Fonctions permettant la lecture de la position d'une roue.

## Etat 
Fonctionnel

# Code V2
## Description 
Ajout de fonction setup pour PWM et pour transmission en UART.
Permet de connaitre la position d'une roue via message par UART. (lecture sur pin 4)
Attention quand UART est activé, changement de la fréquence de l'horloge.

## Etat 
Fonctionnel

# Code V3
## Description
Mesure de la différence entre deux pos de la roue. Ecartée grace à un timer de 32 768 tics sur horloge de 40MHz avec un prescaler de 1:256.
Permet de tracer un graphe vitesse de la roue en fonction de OCxRS. (feuille roue 1 et roue 2)

## Etat 
Fonctionnel, testé pour roue 1 avec PWM = 1/2. Comparaison avec mesure manuelle de 10 tours.
Très peu d'ecart entre les mesures :
- Manuel : 0.91 tour/s
- Code : 0.92 tour/s

Petits glich entre mesure mais dans les bits de poids plus faible.


# Code V4 
## Description
Code permettant de faire avancer la voiture en ligne droite avec fonction de correction via rétroaction. Avant de déposer la voiture, actionner le moteur et attendre que la LED s'allume. 
(circuit R = 82 et LED en serie en sortie de pin RB4)
Différent test ont été fait pour determiner les meilleurs paramètres (qualitativement) dans données expériementales.
## Note 
Le moteur 2 ne démare pas seul, il faut donc le démarer manuellement ce qui ne permet pas de le placer et l'ajuster sur une trajectoire fixée auparavent.

## Etat 
Fonctionnel, la voiture semble suivre une ligne droite.
Problème de vérification car quand la voiture est posée elle a deja "choisi" sa trajectoire (dû a un départ pas sur le sol).

# Code V5 
## Description
Code permettant de faire avancer la voiture en ligne droite jusqu'a une distance voulue.
Vitesse de la voiture : 0.25m/s (~0.6*PR2)
Mesure dans donnée experimentales, feuille "donnée retro V5"

## Etat
Fonctionnel mais trop d'erreur jusqu'à 4cm en longeur et jusqu'à 4cm en largeur.(Testé sur 180cm)

# Code V6
## Description 
Code qui fait avancer la voiture jusqu'a distance donnée avec correction proportionnelle et dérivée.

## Etat
Régulièrement 5cm d'erreur en longueur.

# Code V7 
## Description
Coreection proportionnelle et intégrale.
## Etat 
Régulièrement 5cm d'erreur en longueur.

# Code V8
???

# Code V9 
## Description
Code qui fait avancer la voiture jusqu'a une distance donnée avec régulateur proportionnel. 
Ajout d'une d'une distance (en longueur) pour ne plus avoir d'arret trop tôt.
## Etat
Testé sur 120cm et 240cm. 
Erreur de -1 à 1 cm dans l'axe de la trajectoire. Erreur de 0 à 7 cm dans l'axe perpendiculaire à la trajectoire. 

# Code V'10
## Description
Code qui fait avance la voiture avec régulateur proportionnel et intégral.

## Etat 
Testé sur 240 cm.
Erreur toujours dans le même sens, +-2.4 cm d'erreur dans l'axe perpendiculaire. Erreur de 1 cm dans l'axe vertical 

# Code V'11
## Description
Code qui fait avancer la voiture avec régulateur proportionnel et intégral. Ajout d'une codition de démarage (connection du pin RA3 et RB4)

## Etat 
Meme resultats que V'10

# Code V'12

## Descritption 
Modification pour rendre le code plus lisible, pas de changement de méthode par rapport à V'11. 
## Etat 
Non testé


# Données expériementales

- Roue 1 et roue 2 : Mesure de la caractéristique des deux roues (avec le nouveau moteur) et approximation par une droite

- Donnée retro : Mesure de la déviation de la voiture. Voiture placée à la main donc les erreur µs de mesures sont possible. Les mesures sont donc prisent plusieurs fois si jugé nécessaire.





# Note 
Problème au niveau du moteur pour toutes les mesures jusqu'au test fait avec V11

(Plus à jour)
Si utilise le deuxième moteur change : _QEB2R = 6; -> _QEB2R = 7;
Car un des deux canal ne focntionne pas.
Impact : Plus de notion de sens de lecture, tout en négatif 




